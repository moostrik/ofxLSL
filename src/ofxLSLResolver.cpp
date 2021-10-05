#include "ofxLSLResolver.h"

using namespace lsl;
using namespace ofxLSL;


Resolver::Resolver() {
  running = true;
  resolver = make_unique<continuous_resolver>();
  auto libMajor = library_version() / 100;
  auto libMinor = library_version() % 100;
  auto protMajor = lsl_protocol_version() / 100;
  auto protMinor = lsl_protocol_version() % 100;
  ofLogNotice("ofxLSL::Resolver") << "start resolving LSL version " << libMajor << "." << libMinor <<
                                     " and protocol " << protMajor << "." << protMinor;
  runThread = std::make_unique<std::thread>(&Resolver::run, this);
}

Resolver::~Resolver() {
  running = false;
  runSignal.notify_one();
  if (runThread->joinable()) runThread->join();
  runThread = nullptr;
//  disconnect();
  resolver = nullptr;
}


void Resolver::run() {
  std::unique_lock<std::mutex> lock(runMutex);
  std::chrono::milliseconds timeout(1000);
  while (running.load()) {
    std::vector<stream_info> resolved_infos = resolver->results();
    // add new streams
    for (auto& info : resolved_infos) {
      bool inletFound = false;
      for (auto& inlet : inlets) {
        if (isEqual(info, inlet->info())) {
          inletFound = true;
        }
      }

      if (!inletFound) {
        ofLogNotice("ofxLSL::Resolver")
            << "found stream '" << info.name() << "' from source '" << info.source_id() <<
               "' with "<< info.channel_count() << " " << info.type() <<
               " channels and a sampling rate of " << info.nominal_srate();
        auto inlet  = std::make_shared<stream_inlet>(info);
        inlet->open_stream();

        ofNotifyEvent(onConnect, inlet, this);
        inlets.emplace_back(std::move(inlet));
      }
    }

    // remove disconnected streams
    for (int i = inlets.size() - 1; i >= 0; i--) {
      auto& inlet = inlets[i];
      bool found = false;
      const auto& iI = inlet->info();
      for (auto& rI : resolved_infos) {
        if (isEqual(iI, rI)) {
          found = true;
        }
      }
      if (!found) {
        ofLogNotice("ofxLSL::Resolver")
            << "lost stream '" << iI.name() <<
               " and source ID '" << iI.source_id() << "'";

        ofNotifyEvent(onDisconnect, inlet, this);
        inlets.erase(inlets.begin() + i);
      }
    }
    runSignal.wait_for(lock, timeout, [&] { return !running.load(); });
  }
}

void Resolver::disconnect() {
  std::lock_guard<std::mutex> lock(runMutex);
  for (auto& inlet : inlets) {
    ofNotifyEvent(onDisconnect, inlet, this);
//    inlet->close_stream();
  }
  inlets.clear();
}


bool Resolver::isEqual(stream_info _infoA, stream_info _infoB) {
  if (_infoA.name() == _infoB.name() && _infoA.type() == _infoB.type() &&
      _infoA.source_id() == _infoB.source_id()) {
    return true;
  }
  return false;
}
