#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
  ofSetLogLevel(OF_LOG_VERBOSE);
  send.addStream("test", "time", 1, IRREGULAR_RATE, cf_float32, "unique");
}

//--------------------------------------------------------------
void ofApp::update(){
  vector<float> v = {ofGetElapsedTimef()};
  send.addSample(v, "test", "time");
}

//--------------------------------------------------------------
void ofApp::draw(){

}
