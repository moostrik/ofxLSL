#pragma once

#include "lsl_cpp.h"
#include "ofLog.h"
#include "ofMain.h"

using namespace lsl;

namespace ofxLSL {

class Resolver {
public:
  // Disable copy and move
  Resolver(Resolver const&) = delete;             // Copy construct
  Resolver(Resolver&&) = delete;                  // Move construct
  Resolver& operator=(Resolver const&) = delete;  // Copy assign
  Resolver& operator=(Resolver&&) = delete;       // Move assign

  // Allow public destruction by the shared pointer
  ~Resolver();

  static std::shared_ptr<Resolver> getInstance() {
    static std::weak_ptr<Resolver> _weakInstance;
    if (auto existingPtr = _weakInstance.lock()) return existingPtr;
    auto newPtr = std::shared_ptr<Resolver>(new Resolver());
    _weakInstance = newPtr;
    return newPtr;
  }

  ofEvent<const std::shared_ptr<lsl::stream_inlet>&> onConnect;
  ofEvent<const std::shared_ptr<lsl::stream_inlet>&> onDisconnect;

private:
  Resolver();

  void run();
  void disconnect();

  bool isEqual(stream_info _infoA, stream_info _infoB);

  atomic<bool> running;
  std::unique_ptr<std::thread> runThread;
  std::condition_variable runSignal;
  std::mutex runMutex;

  std::unique_ptr<continuous_resolver> resolver;
  std::vector<std::shared_ptr<lsl::stream_inlet>> inlets;
};
}
