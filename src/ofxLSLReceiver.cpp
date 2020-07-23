#include "ofxLSLReceiver.h"

ofxLSLReceiver::ofxLSLReceiver() : active(false), sampleCapacity(100){}

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

        cout << "joinConnect" << endl;
        connectThread->join();
        connectThread = nullptr;
        //disconnect();
        cout << "joinPull" << endl;
        pullThread->join();
        pullThread = nullptr;
        cout << "Resolver" << endl;
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
        ofLogNotice("ofxLSLReceiver::connect") << "open stream'" << uniqueId << "'";
        {
          std::lock_guard<std::mutex> lock(connectMutex);
          // insert when not in map
          inlets.insert({uniqueId, std::make_unique<stream_inlet>(info)});
        }
        inlets[uniqueId]->open_stream();
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
        ofLogNotice("ofxLSLReceiver::connect") << "close stream '" << inlet.first << "'";
        auto it = inlets.find(inlet.first);
        std::lock_guard<std::mutex> lock(connectMutex);
        inlets.erase (it);
      }
    }

    sleep(500);
  }
}

void ofxLSLReceiver::disconnect() {
  {
    std::lock_guard<std::mutex> lock(connectMutex);
    for (auto& inlet: inlets) {
      inlet.second->close_stream();
    }
    inlets.clear();
  }
  {
    std::lock_guard<std::mutex> lock(pullMutex);
    samples.clear();
  }
}

void ofxLSLReceiver::pull() {
  while(active) {
    std::lock_guard<std::mutex> lock(connectMutex);
    for (auto& inlet: inlets) {
      std::vector<float> sampleBuffer;
      std::string uID = inlet.first;
      double ts = inlet.second->pull_sample(sampleBuffer, 0.01);

      ofxLSLSample sample;
      sample.timestamp = ts;

      if (ts > 0) {
        sample.sample = std::vector<float>(sampleBuffer.begin(), sampleBuffer.end());

        std::lock_guard<std::mutex> lock(pullMutex);
        samples[uID].push_back(sample);

        while(samples[uID].size() && samples[uID].size() > sampleCapacity) {
  //        ofLogWarning() << "Buffer capacity reached, erasing samples";
          samples[uID].erase(samples[uID].begin());
        }
      }
    }
  }
}
