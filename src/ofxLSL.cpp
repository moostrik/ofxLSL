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
    if(inlet == nullptr) {
      connect();
    } else {
      pull();
    }
  }
}

void ofxLSL::connect() {
  auto streams = lsl::resolve_stream("desc/correlation", "R", 1, 2.f);
  if(streams.size() == 0) return;

  std::lock_guard<std::mutex> lock(mutex);
  inlet = std::make_unique<lsl::stream_inlet>(streams.front());
	
  auto info = inlet->info();
//	ofLogNotice() << "Got XML: " << info.desc().child("mappings").first_child().value();

  ofLogNotice() << "Connecting to " << info.name() << " at " << info.nominal_srate() << "hz";
	buffer.reserve(int(info.nominal_srate() * 30));
	sample_buffer.reserve(inlet->info().channel_count());
}

void ofxLSL::pull() {
  try {
    float ts = inlet->pull_sample(sample_buffer, 1.0);
    if(ts) {
      ofLogVerbose() << "Received sample from " << inlet->info().name();

      ofxLSLSample sample;
      sample.timestamp = ts;
	  sample.sample = sample_buffer;

	  std::lock_guard<std::mutex> lock(mutex);
		while(buffer.size() && buffer.size() >= buffer.capacity()) {
			ofLogWarning() << "Buffer capacity reached " << inlet->info().name();
			buffer.erase(buffer.begin());
		}
		
	  buffer.push_back(sample);
    }
  } catch(lsl::lost_error& e) {
    ofLogWarning() << "Disconnected from " << inlet->info().name();

    std::lock_guard<std::mutex> lock(mutex);

    inlet = nullptr;
    buffer.clear();
	sample_buffer.clear();
  }
}
