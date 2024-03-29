﻿#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup() {
  ofSetLogLevel(OF_LOG_VERBOSE);

  useAppTime.addListener(this, &ofApp::useAppTimeListener);

  gui.setup();
  gui.setName("LSL SENDER");
  gui.add(useAppTime.set("app time", true));
  gui.add(numChannelsConsumed.set("channels consumed", 0, 0, 4));
  gui.add(floatParameter.set("floats", glm::vec3(0, 1, 2), glm::vec3(0, 0, 0), glm::vec3(4, 4, 4)));
  gui.add(intParameterA1.set("int A1", 0, 0, 3));
  gui.add(intParameterA2.set("int A2", 0, 0, 3));
  gui.add(intParameterB.set("int B", 0, 0, 3));
  gui.add(stringParameter.set("string", "string"));
  ofSetWindowShape(gui.getWidth() + 20, gui.getHeight() + 20);

  floatParameter.addListener(this, &ofApp::floatParameterListener);
  intParameterA1.addListener(this, &ofApp::intParameterAListener);
  intParameterA2.addListener(this, &ofApp::intParameterAListener);
  intParameterB.addListener(this, &ofApp::intParameterBListener);
  stringParameter.addListener(this, &ofApp::stringParameterListener);

  sender.addStream("stream1", "floats", 3, IRREGULAR_RATE, cf_float32, "uid");
  sender.addStream("stream2", "ints", 2, IRREGULAR_RATE, cf_int8, "uid");
  sender.addStream("stream3", "ints", 1, IRREGULAR_RATE, cf_int8, "uid");
  sender.addStream("stream4", "string", 1, IRREGULAR_RATE, cf_string, "uid");
}

//--------------------------------------------------------------
void ofApp::update() {
  numChannelsConsumed = 0;
  if (sender.isConsumed("stream1", "floats", "uid")) numChannelsConsumed++;
  if (sender.isConsumed("stream2", "ints", "uid")) numChannelsConsumed++;
  if (sender.isConsumed("stream3", "ints", "uid")) numChannelsConsumed++;
  if (sender.isConsumed("stream4", "string", "uid")) numChannelsConsumed++;
}

void ofApp::useAppTimeListener(bool &_value) {
  sender.useAppTime(_value);
}

void ofApp::floatParameterListener(glm::vec3& _value) {
  vector<float> floatVec = {_value.x, _value.y, _value.z};
  sender.addSample(floatVec, "stream1", "floats", "uid");
}

void ofApp::intParameterAListener(int& _value) {
  vector<int> intVec = {intParameterA1, intParameterA2};
  sender.addSample(intVec, "stream2", "ints", "uid");
}

void ofApp::intParameterBListener(int& _value) {
  vector<int> intVec2 = {intParameterB};
  sender.addSample(intVec2, "stream3", "ints", "uid");
}

void ofApp::stringParameterListener(string& _value) {
  vector<string> stringVec = {stringParameter};
  sender.addSample(stringVec, "stream4", "string", "uid");
}

//--------------------------------------------------------------
void ofApp::draw() { gui.draw(); }
