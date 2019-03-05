#pragma once

#include "ofMain.h"
#include "ofLog.h"
#include "lsl_cpp.h"

struct ofxLSLSample {
	float timestamp = 0.0;
	std::vector<float> sample;
};

struct ofxStability {
	string color = "none";
	std::vector<int> sample;
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
	
	std::vector< pair<string, string> > getMapping() {
		std::lock_guard<std::mutex> lock(mutex);
		return sample_mapping;
	};
	
	std::vector< ofxStability > getStability() {
		std::lock_guard<std::mutex> lock(mutex);
		return stabilities;
	};
	
private:
	
	void update();
	void connect();
	void disconnect();
	void pullSamples();
	void pullStability();
	bool active;
	std::vector<float> sample_buffer;
	std::vector< pair<string, string> > sample_mapping;
	
	std::vector< std::unique_ptr<lsl::stream_inlet> > stability_inlets;
	std::vector< ofxStability > stabilities;
	
	std::mutex mutex;
	std::unique_ptr<std::thread> thread;
	std::unique_ptr<lsl::stream_inlet> inlet;
	std::vector<ofxLSLSample> buffer;
};

