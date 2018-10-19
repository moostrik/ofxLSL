#pragma once

#include "ofMain.h"
#include "ofLog.h"
#include "lsl_cpp.h"

struct ofxLSLSample {
  float timestamp = 0.0;
  std::vector<float> sample;
};

class ofxLSL : public ofThread {
  
public:
  ofxLSL();
	~ofxLSL() { stop(); };
  
  bool start();
  bool stop();
  bool isConnected() {
	  std::lock_guard<std::mutex> lock(mutex);
	  return inlet != nullptr;
	
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
  void pull();
  bool active;
  std::vector<float> sample_buffer;
  
  std::mutex mutex;
  std::unique_ptr<std::thread> thread;
  std::unique_ptr<lsl::stream_inlet> inlet;
	std::vector<ofxLSLSample> buffer;
};
