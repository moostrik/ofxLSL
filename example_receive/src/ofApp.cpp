#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup() {
  ofSetLogLevel(OF_LOG_VERBOSE);

  receive = std::make_unique<ofxLSL::Receiver>("stream1", "uid");
}

//--------------------------------------------------------------
void ofApp::update() {
  auto samples = receive->flush();
  if (samples.size()) {
    cout << samples[0].timestamp << " " << samples[0].sample[0] << endl;
  }
}

//--------------------------------------------------------------
void ofApp::draw() {}
