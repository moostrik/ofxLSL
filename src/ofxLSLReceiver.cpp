#include "ofxLSLReceiver.h"

ofxLSLReceiver::ofxLSLReceiver() : active(false), containerCapacity(100){}

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
        disconnect();

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
    std::lock_guard<std::mutex> lock(connectMutex);

    // add new streams
    for (auto& info: resolved_infos) {
      bool inletFound = false;
      for(auto& inlet: inlets) {
        if (isEqual(info, inlet->info())){
            inletFound = true;;
        }
      }

      if (!inletFound) {
        ofLogNotice("ofxLSLReceiver::connect") << "open stream'" << info.name() << "' with type '" << info.type() << " and source ID '" << info.source_id() << "'";
        inlets.emplace_back(std::make_unique<stream_inlet>(info));
        auto& inlet = inlets.back();
        inlet->open_stream();
      }
    }

    // remove disconnected streams
    for (int i = inlets.size()-1; i>=0; i--) {
      auto& inlet = inlets[i];
      bool found = false;
      const auto& iI = inlet->info();
      for (auto& rI: resolved_infos) {
        if (isEqual(iI, rI)) {
          found = true;
        }
      }
      if (!found) {
        ofLogNotice("ofxLSLReceiver::connect") << "lost stream '" << iI.name() << "' with type '" << iI.type() << " and source ID '" << iI.source_id() << "'";
        inlets.erase(inlets.begin()+i);
      }
    }

    sleep(500);
  }
}

void ofxLSLReceiver::disconnect() {
  {
    std::lock_guard<std::mutex> lock(connectMutex);
    for (auto& inlet: inlets) {
      inlet->close_stream();
    }
    inlets.clear();
  }
  {
    std::lock_guard<std::mutex> lock(pullMutex);
    containers.clear();
  }
}

void ofxLSLReceiver::pull() {
  while(active) {
    std::lock_guard<std::mutex> lock(connectMutex);
    for (auto& inlet: inlets) {
      std::vector<float> sampleBuffer;
      double ts = inlet->pull_sample(sampleBuffer, 0.01);
      const auto& info = inlet->info();

      ofxLSLSample sample;
      sample.timestamp = ts;

      if (ts > 0) {
        sample.sample = std::vector<float>(sampleBuffer.begin(), sampleBuffer.end());

        std::lock_guard<std::mutex> lock(pullMutex);

        auto container = getContainer(info);
        if (!container) {
          containers.emplace_back(std::make_shared<ofxLSLContainer>());
          container = containers.back();
          container->info = info;
        }
        auto& samples = container->samples;
        samples.push_back(sample);

        while(samples.size() && samples.size() > containerCapacity) {
  //        ofLogWarning() << "Buffer capacity reached, erasing samples";
          samples.erase(samples.begin());
        }
      }
    }
  }
}
