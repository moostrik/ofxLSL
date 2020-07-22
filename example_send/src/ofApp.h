#pragma once

#include "ofMain.h"
#include "ofxLSL.h"


class ofApp : public ofBaseApp{
public:
	void setup();
	void update();
	void draw();

        ofxLSLSender send;
};
