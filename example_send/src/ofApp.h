#pragma once

#include "ofMain.h"
#include "ofxGui.h"
#include "ofxLSL.h"

class ofApp : public ofBaseApp {
 public:
  void setup();
  void update();
  void draw();

  ofxLSL::Sender sender;

  ofParameter<int> numChannelsConsumed;

  ofxGuiGroup gui;
  ofParameter<bool> useAppTime;
  ofParameter<glm::vec3> floatParameter;
  ofParameter<int> intParameterA1;
  ofParameter<int> intParameterA2;
  ofParameter<int> intParameterB;
  ofParameter<string> stringParameter;

  void useAppTimeListener(bool& _value);
  void floatParameterListener(glm::vec3& _value);
  void intParameterAListener(int& _value);
  void intParameterBListener(int& _value);
  void stringParameterListener(string& _value);
};
