#pragma once

#include "ofMain.h"
#include "ofLog.h"
#include "lsl_cpp.h"


using namespace lsl;

struct ofxLSLSample {
        double timestamp = 0.0;
        std::vector<float> sample;
};

class ofxLSLReceiver : public ofThread {

public:
        ofxLSLReceiver();
        ~ofxLSLReceiver() { stop(); };

        bool start();
        bool stop();
        bool isConnected() {
                std::lock_guard<std::mutex> lock(connectMutex);
                return inlets.size() > 0;
        }

        vector<std::string> getStreamNames() {
          std::lock_guard<std::mutex> lock(pullMutex);
          vector<std::string> names;
          for (auto& s: samples) {
            names.push_back(s.first);
          }
          return names;
        }

        std::vector<ofxLSLSample> flush(std::string _streamName) {
          std::lock_guard<std::mutex> lock(pullMutex);

          std::vector<ofxLSLSample> currentBuffer;
          auto it = samples.find(_streamName);
          if (it == samples.end()) {
            return currentBuffer;
          }

          currentBuffer = std::vector<ofxLSLSample>(samples[_streamName].begin(), samples[_streamName].end());
          samples[_streamName].clear();
          return currentBuffer;
        };

private:
        void connect();
        void disconnect();
        void pull();

        bool active;
        std::mutex connectMutex;
        std::unique_ptr<std::thread> connectThread;
        std::mutex pullMutex;
        std::unique_ptr<std::thread> pullThread;
        std::unique_ptr<lsl::stream_inlet> inlet;
        std::unique_ptr<continuous_resolver> resolver;        
        std::map<const std::string, std::unique_ptr<lsl::stream_inlet>> inlets;

        int sampleCapacity;
        std::map<const std::string, vector<ofxLSLSample>> samples;

        std::string uniqueIDFromInfo(stream_info _info) {
          std::string uID = _info.name()+_info.type()+_info.source_id()+_info.hostname();
          return uID;
        }
};

