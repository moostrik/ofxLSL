#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
  ofSetLogLevel(OF_LOG_VERBOSE);
  receive.start();
}

//--------------------------------------------------------------
void ofApp::update(){
  vector<float> v = {ofGetElapsedTimef()};
}

//--------------------------------------------------------------
void ofApp::draw(){

}
