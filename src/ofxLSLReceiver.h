#pragma once

#include "ofMain.h"
#include "ofLog.h"
#include "lsl_cpp.h"


using namespace lsl;

struct ofxLSLSample {
        float timestamp = 0.0;
        std::vector<float> sample;
};

class ofxLSLReceiver : public ofThread {

public:
        ofxLSLReceiver();
        ~ofxLSLReceiver() { stop(); };

        bool start();
        bool stop();
        bool isConnected() {
                std::lock_guard<std::mutex> lock(mutex);
                return inlets.size() > 0;

        }

        std::vector<ofxLSLSample> flush() {
                std::lock_guard<std::mutex> lock(mutex);
                auto currentBuffer = std::vector<ofxLSLSample>(buffer.begin(), buffer.end());
                buffer.clear();
                return currentBuffer;
        };
private:

        void update();
        void connect();
        void disconnect();
        void pull();

        bool active;

        std::mutex mutex;
        std::unique_ptr<std::thread> connectThread;
        std::unique_ptr<std::thread> pullThread;
        std::unique_ptr<lsl::stream_inlet> inlet;
        std::vector<ofxLSLSample> buffer;

        std::unique_ptr<continuous_resolver> resolver;
        
        std::map<std::string, std::unique_ptr<stream_info>> infos;
        std::map<std::string, std::unique_ptr<lsl::stream_inlet>> inlets;

        std::string uniqueIDFromInfo(stream_info _info) {
          std::string uID = _info.name()+_info.type()+_info.source_id()+_info.hostname();
          return uID;
        }
};

