#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
  ofSetLogLevel(OF_LOG_VERBOSE);
  receive.start();
}

//--------------------------------------------------------------
void ofApp::update(){
  auto names = receive.getStreamNames();

  for (auto n: names) {
    auto samples = receive.flush(n);
    if (samples.size()) {
      cout << samples[0].timestamp <<  " " << samples[0].sample[0] << endl;
    }
  }

  vector<float> v = {ofGetElapsedTimef()};
}

//--------------------------------------------------------------
void ofApp::draw(){

}
