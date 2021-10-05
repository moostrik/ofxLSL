#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup() {
  ofSetLogLevel(OF_LOG_VERBOSE);

  receiveFloat  = std::make_shared<ofxLSL::Receiver<float>>("stream1", "uid");
  receiveIntA   = std::make_shared<ofxLSL::Receiver<int>>("stream2", "uid");
  receiveIntB   = std::make_shared<ofxLSL::Receiver<int>>("stream3", "uid");
  receiveString = std::make_shared<ofxLSL::Receiver<string>>("stream4", "uid");

  gui.setup();
  gui.setName("LSL RECEIVER");
  gui.add(lastTime.set("time", "0"));
  gui.add(numChannelsConnected.set("channels connected", 0, 0, 4));
  gui.add(floatParameter.set("floats", glm::vec3(0, 1, 2), glm::vec3(0, 0, 0), glm::vec3(4, 4, 4)));
  gui.add(intParameterA1.set("int A1", 0, 0, 3));
  gui.add(intParameterA2.set("int A2", 0, 0, 3));
  gui.add(intParameterB.set("int B", 0, 0, 3));
  gui.add(stringParameter.set("string", "string"));
  ofSetWindowShape(gui.getWidth() + 20, gui.getHeight() + 20);
}

//--------------------------------------------------------------
void ofApp::update() {
  int connected = 0;
  connected += receiveFloat->isConnected();
  connected += receiveIntA->isConnected();
  connected += receiveIntB->isConnected();
  connected += receiveString->isConnected();
  numChannelsConnected = connected;

  auto floatSamples = receiveFloat->flush();
  if (floatSamples.size()) {
    auto lastSample = floatSamples.back();
    lastTime = ofToString(lastSample->timeStamp);
    auto data = lastSample->sample;
    if (data.size() >= 3) {
      floatParameter.set(glm::vec3(data[0], data[1], data[2]));
    }
  }

  auto intSamplesA = receiveIntA->flush();
  if (intSamplesA.size()) {
    auto lastSample = intSamplesA.back();
    lastTime = ofToString(lastSample->timeStamp);
    auto data = lastSample->sample;
    if (data.size() >= 2) {
      intParameterA1.set(data[0]);
      intParameterA2.set(data[1]);
    }
  }

  auto intSamplesB = receiveIntB->flush();
  if (intSamplesB.size()) {
    auto lastSample = intSamplesB.back();
    lastTime = ofToString(lastSample->timeStamp);
    auto data = lastSample->sample;
    if (data.size() >= 1) {
      intParameterB.set(data[0]);
    }
  }

  auto stringSample = receiveString->flush();
  if (stringSample.size()) {
    auto lastSample = stringSample.back();
    lastTime = ofToString(lastSample->timeStamp);
    auto data = lastSample->sample;
    if (data.size() >= 1) {
      stringParameter.set(data[0]);
    }
  }
}

//--------------------------------------------------------------
void ofApp::draw() {
  gui.draw();
}
