#include "ofxLSLResolver.h"

using namespace lsl;
using namespace ofxLSL;


Resolver::Resolver() {
  active = true;

  resolver = make_unique<continuous_resolver>();
  connectThread = std::make_unique<std::thread>(&Resolver::connect, this);
}

Resolver::~Resolver() {
  active = false;
  disconnect();

  connectThread->join();
  connectThread = nullptr;
  resolver = nullptr;
}


void Resolver::connect() {
  while (active) {
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
        ofLogNotice("ofxLSLReceiver::connect")
            << "open stream'" << info.name() << "' with type '" << info.type()
            << " and source ID '" << info.source_id() << "'";
        auto inlet  = std::make_shared<stream_inlet>(info);
        inlet->open_stream();

        std::lock_guard<std::mutex> lock(connectMutex);
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
        ofLogNotice("ofxLSLReceiver::connect")
            << "lost stream '" << iI.name() << "' with type '" << iI.type()
            << " and source ID '" << iI.source_id() << "'";
        std::lock_guard<std::mutex> lock(connectMutex);

        ofNotifyEvent(onDisconnect, inlet, this);
        inlets.erase(inlets.begin() + i);
      }
    }

    sleep(500);
  }
}

void Resolver::disconnect() {
  {
    std::lock_guard<std::mutex> lock(connectMutex);
    for (auto& inlet : inlets) {
      ofNotifyEvent(onDisconnect, inlet, this);
      inlet->close_stream();
    }
    inlets.clear();
  }
}


bool Resolver::isEqual(stream_info _infoA, stream_info _infoB) {
  if (_infoA.name() == _infoB.name() && _infoA.type() == _infoB.type() &&
      _infoA.source_id() == _infoB.source_id()) {
    return true;
  }
  return false;
}
