#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
  ofSetLogLevel(OF_LOG_VERBOSE);
  send.addStream("test", "time", 1, IRREGULAR_RATE, cf_float32, "unique01");
}

//--------------------------------------------------------------
void ofApp::update(){
  vector<float> v = {ofGetElapsedTimef()};
  send.addSample("test", v);
}

//--------------------------------------------------------------
void ofApp::draw(){

}
