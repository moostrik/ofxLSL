#pragma once

#include "lsl_cpp.h"
#include "ofLog.h"
#include "ofMain.h"

using namespace lsl;

struct ofxLSLSample {
  double timestamp = 0.0;
  std::vector<float> sample;
};

struct ofxLSLContainer {
  stream_info info;
  std::vector<ofxLSLSample> samples;
};

class ofxLSLReceiver : public ofThread {
 public:
  ofxLSLReceiver();
  ~ofxLSLReceiver() { stop(); };

  bool start();
  bool stop();
  bool isConnected() {
    std::lock_guard<std::mutex> lock(connectMutex);
    return inlets.size() > 0;
  }

  vector<stream_info> getStreamNames() {
    std::lock_guard<std::mutex> lock(pullMutex);
    vector<stream_info> infos;
    for (const auto& c : containers) {
      infos.push_back(c->info);
    }
    return infos;
  }

  std::vector<ofxLSLSample> flush(stream_info info) {
    std::vector<ofxLSLSample> currentBuffer;
    auto container = getContainer(info);
    if (container) {
      std::lock_guard<std::mutex> lock(pullMutex);
      currentBuffer = std::vector<ofxLSLSample>(container->samples.begin(),
                                                container->samples.end());
      container->samples.clear();
    }
    return currentBuffer;
  };

 private:
  void connect();
  void disconnect();
  void pull();

  atomic<bool> active;
  std::mutex connectMutex;
  std::unique_ptr<std::thread> connectThread;
  std::mutex pullMutex;
  std::unique_ptr<std::thread> pullThread;
  std::condition_variable pullSignal;

  std::unique_ptr<continuous_resolver> resolver;
  std::vector<std::unique_ptr<lsl::stream_inlet>> inlets;

  int containerCapacity;
  std::vector<std::shared_ptr<ofxLSLContainer>> containers;

  bool hadConsumers;

  std::shared_ptr<ofxLSLContainer> getContainer(stream_info _info) {
    for (auto container : containers) {
      if (isEqual(_info, container->info)) {
        return container;
      }
    }
    return nullptr;
  }

  bool isEqual(stream_info _infoA, stream_info _infoB) {
    if (_infoA.name() == _infoB.name() && _infoA.type() == _infoB.type() &&
        _infoA.source_id() == _infoB.source_id()) {
      return true;
    }
    return false;
  }
};
