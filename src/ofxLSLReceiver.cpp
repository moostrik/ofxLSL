#include "ofxLSLReceiver.h"


using namespace lsl;
using namespace ofxLSL;

BaseReceiver::BaseReceiver(std::string _streamName, std::string _sourceId) :
  resolver(Resolver::getInstance()),
  active(false),
  sampleCapacity(100),
  streamName(_streamName),
  sourceId(_sourceId)
{
  ofAddListener(resolver->onConnect, this, &BaseReceiver::handleConnect);
  ofAddListener(resolver->onDisconnect, this, &BaseReceiver::handleDisconnect);
}


BaseReceiver::~BaseReceiver() {
  ofRemoveListener(resolver->onConnect, this, &BaseReceiver::handleConnect);
  ofRemoveListener(resolver->onDisconnect, this, &BaseReceiver::handleDisconnect);
  stop();
};

// -- CONNECTIONS -------------------------------------------------------------------------------

void BaseReceiver::handleConnect(const std::shared_ptr<lsl::stream_inlet>& value) {
  if (value->info().name() != streamName || value->info().source_id() != sourceId) return;
  std::lock_guard<std::mutex> lock(resolverMutex);
  inlet = value;
  start();
}

void BaseReceiver::handleDisconnect(const std::shared_ptr<lsl::stream_inlet>& value) {
  if (value->info().name() != streamName || value->info().source_id() != sourceId) return;
  std::lock_guard<std::mutex> lock(resolverMutex);
  inlet = nullptr;
  stop();
}

bool BaseReceiver::isConnected() {
  std::lock_guard<std::mutex> lock(resolverMutex);
  return inlet != nullptr;
}

// -- CONNECTIONS -------------------------------------------------------------------------------
bool BaseReceiver::start() {
  if (active) return false;
  active = true;
  pullThread = std::make_unique<std::thread>(&BaseReceiver::run, this);
  return true;
}

bool BaseReceiver::stop() {
  if (!active) return false;
  active = false;
  if (pullThread->joinable()) pullThread->join();
  pullThread = nullptr;
  return true;
}

void BaseReceiver::run() {
  std::unique_lock<std::mutex> pullLock(pullMutex);
  std::chrono::microseconds timeout(100);

  while (active) {
    {
    std::lock_guard<std::mutex> resolverLock(resolverMutex);
    pull();
    }
    pullSignal.wait_for(pullLock, timeout, [&] { return !active.load(); });
  }
}
