#pragma once

#include "ofMain.h"
#include "ofxGui.h"
#include "ofxLSL.h"

class ofApp : public ofBaseApp {
 public:
  void setup();
  void update();
  void draw();

  std::shared_ptr<ofxLSL::Receiver<float>>  receiveFloat;
  std::shared_ptr<ofxLSL::Receiver<int>>    receiveIntA;
  std::shared_ptr<ofxLSL::Receiver<int>>    receiveIntB;
  std::shared_ptr<ofxLSL::Receiver<string>> receiveString;

  ofxGuiGroup gui;
  ofParameter<string> lastTime;
  ofParameter<int> numChannelsConnected;
  ofParameter<glm::vec3> floatParameter;
  ofParameter<int> intParameterA1;
  ofParameter<int> intParameterA2;
  ofParameter<int> intParameterB;
  ofParameter<string> stringParameter;
};
