#include "ofxLSLReceiver.h"


using namespace lsl;
using namespace ofxLSL;

Receiver::Receiver(std::string _streamName, std::string _sourceId) :
  resolver(Resolver::getInstance()),
  active(false),
  sampleCapacity(100),
  streamName(_streamName),
  sourceId(_sourceId)
{
  ofAddListener(resolver->onConnect, this, &Receiver::handleConnect);
  ofAddListener(resolver->onDisconnect, this, &Receiver::handleDisconnect);
}


Receiver::~Receiver() {
  ofRemoveListener(resolver->onConnect, this, &Receiver::handleConnect);
  ofRemoveListener(resolver->onDisconnect, this, &Receiver::handleDisconnect);
  stop();
};


std::vector<Sample> Receiver::flush() {
  std::lock_guard<std::mutex> lock(pullMutex);
  std::vector<Sample> flushSamples = samples;
  samples.clear();
  return flushSamples;
};

// -- CONNECTIONS -------------------------------------------------------------------------------

void Receiver::handleConnect(const std::shared_ptr<lsl::stream_inlet>& value) {
  if (value->info().name() != streamName || value->info().source_id() != sourceId) return;
  std::lock_guard<std::mutex> lock(resolverMutex);
  inlet = value;
  start();
}

void Receiver::handleDisconnect(const std::shared_ptr<lsl::stream_inlet>& value) {
  if (value->info().name() != streamName || value->info().source_id() != sourceId) return;
  std::lock_guard<std::mutex> lock(resolverMutex);
  inlet = nullptr;
  stop();
}

bool Receiver::isConnected() {
  std::lock_guard<std::mutex> lock(resolverMutex);
  return inlet != nullptr;
}

// -- CONNECTIONS -------------------------------------------------------------------------------
bool Receiver::start() {
  if (active) return false;
  active = true;

  pullThread = std::make_unique<std::thread>(&Receiver::pull, this);
  return true;
}

bool Receiver::stop() {
  if (!active) return false;
  active = false;

  pullThread->join();
  pullThread = nullptr;

  return true;
}

void Receiver::pull() {
  std::unique_lock<std::mutex> pullLock(pullMutex);
  std::chrono::microseconds timeout(100);

  while (active) {
    std::vector<float> sampleBuffer;
    double ts;
    stream_info info;
    {
    std::lock_guard<std::mutex> resolverLock(resolverMutex);
    ts = inlet->pull_sample(sampleBuffer, 0.0);
    info = inlet->info();
    }
    Sample sample;
    sample.timestamp = ts;

    if (ts > 0) {
      sample.sample = std::vector<float>(sampleBuffer.begin(), sampleBuffer.end());
      samples.push_back(sample);
      while (samples.size() && samples.size() > sampleCapacity) {
        //        ofLogWarning() << "Buffer capacity reached, erasing
        //        samples";
        samples.erase(samples.begin());
      }
    }

    pullSignal.wait_for(pullLock, timeout, [&] { return !active.load(); });
  }
}
