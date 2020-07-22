#include "ofxLSLReceiver.h"

ofxLSLReceiver::ofxLSLReceiver() : active(false) {}

bool ofxLSLReceiver::start() {
        if(active) return false;
        active = true;
        
        resolver = make_unique<continuous_resolver>();
        connectThread = std::make_unique<std::thread>(&ofxLSLReceiver::connect, this);
        pullThread = std::make_unique<std::thread>(&ofxLSLReceiver::pull, this);
        
        return true;
}

bool ofxLSLReceiver::stop() {
        if(!active) return false;
        active = false;
        
        connectThread->join();
        connectThread = nullptr;
        pullThread->join();
        pullThread = nullptr;
        resolver = nullptr;
        
        return true;
}

void ofxLSLReceiver::connect() {

  while(active) {
    std::vector<stream_info> resolved_infos = resolver->results();

    // add new streams
    for (auto& info: resolved_infos) {
      std::string uniqueId = uniqueIDFromInfo(info);
      auto it = inlets.find(uniqueId);
      if (it == inlets.end()){
        ofLogNotice("ofxLSLReceiver::connect") << "connect to '" << uniqueId << "'";
        // insert when not in map
        inlets.insert({uniqueId, std::make_unique<stream_inlet>(info)});
      }
    }

    // remove disconnected streams

    for (auto& inlet: inlets) {
      bool found = false;
      for (auto& info: resolved_infos) {
        std::string uniqueId = uniqueIDFromInfo(info);
        if (uniqueId == inlet.first) {
          found = true;
        }
      }
      if (!found) {
        ofLogNotice("ofxLSLReceiver::connect") << "disconnect from '" << inlet.first << "'";
        auto it = inlets.find(inlet.first);
        inlets.erase (it);
      }
    }

    sleep(500);
  }
}

void ofxLSLReceiver::pull() {

}
