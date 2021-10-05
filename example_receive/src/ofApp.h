#pragma once

#include "ofMain.h"
#include "ofxLSL.h"

class ofApp : public ofBaseApp {
 public:
  void setup();
  void update();
  void draw();

  std::shared_ptr<ofxLSL::Receiver<float>> receive;
};
