#pragma once

#include "lsl_cpp.h"
#include "ofLog.h"
#include "ofMain.h"
#include "ofxLSLResolver.h"

using namespace lsl;

namespace ofxLSL {

struct Sample {
  double timestamp = 0.0;
  std::vector<float> sample;
};

struct Container {
  stream_info info;
  std::vector<Sample> samples;
};

class Receiver : public ofThread {
  friend Resolver;
 public:
  Receiver(std::string _streamName, std::string _sourceId);
  ~Receiver();

  bool isConnected();
  std::string getStreamName() { return streamName; }
  std::string getSourceId()   { return sourceId; }

  std::vector<Sample> flush();

protected:
  void handleConnect(const std::shared_ptr<lsl::stream_inlet>& value);
  void handleDisconnect(const std::shared_ptr<lsl::stream_inlet>& value);

 private:
  std::shared_ptr<Resolver> resolver;
  std::mutex resolverMutex;

  bool start();
  bool stop();
  void pull();

  atomic<bool> active;
  std::mutex pullMutex;
  std::unique_ptr<std::thread> pullThread;
  std::condition_variable pullSignal;

  std::string streamName;
  std::string sourceId;
  std::shared_ptr<lsl::stream_inlet> inlet;

  int sampleCapacity;
  std::vector<Sample> samples;
};
}
