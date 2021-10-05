#pragma once

#include "lsl_cpp.h"
#include "ofLog.h"
#include "ofMain.h"
#include "ofxLSLResolver.h"

using namespace lsl;

namespace ofxLSL {

class BaseReceiver : public ofThread {
  friend Resolver;
 public:
  BaseReceiver(std::string _streamName, std::string _sourceId);
  ~BaseReceiver();

  void setCapacity(int value) { sampleCapacity.store(value); }
  bool isConnected();
  std::string getStreamName() { return streamName; }
  std::string getSourceId()   { return sourceId; }

protected:
  void handleConnect(const std::shared_ptr<lsl::stream_inlet>& value);
  void handleDisconnect(const std::shared_ptr<lsl::stream_inlet>& value);

  std::shared_ptr<Resolver> resolver;
  std::mutex resolverMutex;

  bool start();
  bool stop();
  void run();
  virtual void pull() = 0;

  atomic<bool> active;
  std::mutex runMutex;
  std::unique_ptr<std::thread> pullThread;
  std::condition_variable pullSignal;
  std::mutex pullMutex;

  std::string streamName;
  std::string sourceId;
  std::shared_ptr<lsl::stream_inlet> inlet;

  atomic<int> sampleCapacity;
};


template <typename T>
class Sample {
public:
  double timestamp = 0.0;
  std::vector<T> sample;
};

template <typename T>
class Receiver : public BaseReceiver {
public:
  Receiver(std::string _streamName, std::string _sourceId = "") :
    BaseReceiver(_streamName, _sourceId) {}

  std::vector<std::shared_ptr<Sample<T>>> flush() {
    std::lock_guard<std::mutex> lock(pullMutex);
    std::vector<std::shared_ptr<Sample<T>>> flushSamples;
    flushSamples.swap(samples);
    return flushSamples;
  }

  ofEvent<const std::shared_ptr<Sample<T>>&> onSample;

protected:
  void pull() override {
    auto sampleBuffer = std::make_shared<Sample<T>>();
    auto ts = inlet->pull_sample(sampleBuffer->sample, 0.0);
    sampleBuffer->timestamp = ts;
    if (ts > 0) {
      ofNotifyEvent(onSample, sampleBuffer, this);
      std::lock_guard<std::mutex> lock(pullMutex);
      samples.push_back(sampleBuffer);
      while (samples.size() && samples.size() > sampleCapacity) {
        samples.erase(samples.begin());
      }
    }
  }

  std::vector<std::shared_ptr<Sample<T>>> samples;
};

}
