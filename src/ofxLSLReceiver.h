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

  bool isConnected();

  vector<stream_info> getStreamNames();

  std::vector<ofxLSLSample> flush(stream_info info);

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

  std::shared_ptr<ofxLSLContainer> getContainer(stream_info _info);

  bool isEqual(stream_info _infoA, stream_info _infoB);
};
