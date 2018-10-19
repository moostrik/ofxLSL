#include "ofxLSL.h"

ofxLSL::ofxLSL() : active(false) {}

bool ofxLSL::start() {
  if(active) return false;

  thread = std::make_unique<std::thread>(&ofxLSL::update, this);
  active = true;
  return true;
}

bool ofxLSL::stop() {
  if(!active) return false;

  active = false;
  thread->join();
  thread = nullptr;
  return true;
}

void ofxLSL::update() {
  while(active) {
    try {
		if(inlet == nullptr) {
		  connect();
		} else {
		  pull();
		}
	} catch(lsl::timeout_error& e) {
		disconnect();
	} catch(lsl::lost_error& e) {
		disconnect();
	}
  }
}

void ofxLSL::disconnect() {
	ofLogWarning() << "Disconnected from stream";
	
	std::lock_guard<std::mutex> lock(mutex);
	inlet = nullptr;
	buffer.clear();
	sample_buffer.clear();
}

void ofxLSL::connect() {
  auto streams = lsl::resolve_stream("desc/correlation", "R", 1, 2.f);
  if(streams.size() == 0) return;

  std::lock_guard<std::mutex> lock(mutex);
  inlet = std::make_unique<lsl::stream_inlet>(streams.front(), 360, 0, false);
	
  auto info = inlet->info(1.0f);
//	ofLogNotice() << "Got XML: " << info.desc().child("mappings").first_child().value();

  ofLogNotice() << "Connecting to " << info.name() << " at " << info.nominal_srate() << "hz";
	buffer.reserve(250.0);
	sample_buffer.reserve(info.channel_count());
}

void ofxLSL::pull() {
    float ts = inlet->pull_sample(sample_buffer, 1.0);
    if(ts) {
      ofLogVerbose() << "Received sample";

      ofxLSLSample sample;
      sample.timestamp = ts;
	  sample.sample = std::vector<float>(sample_buffer.begin(), sample_buffer.end());

	  std::lock_guard<std::mutex> lock(mutex);
		while(buffer.size() && buffer.size() >= buffer.capacity()) {
			ofLogWarning() << "Buffer capacity reached";
			buffer.erase(buffer.begin());
		}
		
	  buffer.push_back(sample);
    }
}
