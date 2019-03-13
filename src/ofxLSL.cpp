#include "ofxLSL.h"

ofxLSL::ofxLSL() : active(false), mappingUpdated(false) {}

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
	
	mappingUpdated = true;
	std::lock_guard<std::mutex> lock(mutex);
	inlet = nullptr;
	buffer.clear();
	sample_buffer.clear();
}

void ofxLSL::connect() {
	//	std::vector<lsl::stream_info> resolvedStreams = lsl::resolve_streams(1.0);
	//
	//	for (auto &s : resolvedStreams)
	//		cout << s.source_id() << " " << s.name() + " (" + s.hostname() + ")" << endl;
	
	auto streams = lsl::resolve_stream("desc/correlation", "R", 1, 2.f);
	if(streams.size() == 0) return;
	
	std::lock_guard<std::mutex> lock(mutex);
	inlet = std::make_unique<lsl::stream_inlet>(streams.front(), 360, 0, false);
	
	auto info = inlet->info(1.0f);
	
	ofLogNotice() << "Connecting to " << info.name() << " at " << info.nominal_srate() << "hz";
	buffer.reserve(250.0);
	sample_buffer.reserve(info.channel_count());
	
	mapping.clear();
	auto mappingXML = info.desc().child("mappings");
	for (auto m = mappingXML.first_child(); !m.empty(); m = m.next_sibling()) {
		bool firstSet = false;
		pair<string,string> mapElement;
		for (auto ab = m.first_child(); !ab.empty(); ab = ab.next_sibling()) {
			for (auto uID = ab.first_child(); !uID.empty(); uID = uID.next_sibling()) {
				if (!firstSet) {
					firstSet =true;
					mapElement.first = uID.value();
				} else {
					mapElement.second = uID.value();
					mapping.emplace_back(mapElement);
				} } } }
	mappingUpdated = true;
}

void ofxLSL::pull() {
	float ts = inlet->pull_sample(sample_buffer, 1.0);
	if(ts) {
		
		ofxLSLSample sample;
		sample.timestamp = ts;
		sample.sample = std::vector<float>(sample_buffer.begin(), sample_buffer.end());
		
		
		ofLogVerbose() << "Received " << sample.sample.size() << " samples @ time " << ts;
		
		std::lock_guard<std::mutex> lock(mutex);
		while(buffer.size() && buffer.size() >= buffer.capacity()) {
			ofLogWarning() << "Buffer capacity reached";
			buffer.erase(buffer.begin());
		}
		
		buffer.push_back(sample);
	}
}

