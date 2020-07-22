#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
  ofSetLogLevel(OF_LOG_VERBOSE);

  send.addStream("test", "time");


//  receive.start();
}

//--------------------------------------------------------------
void ofApp::update(){
  vector<float> v = {ofGetElapsedTimef()};
  send.addSample("test", v);
	
  /*
//	if (lsl.isConnected()) {
	
                auto buffer = receive.flush();
		
		if(buffer.size()) {
			auto sampleToUse = buffer.back();
			
			std::stringstream ss;
			for(auto channel : sampleToUse.sample) {
				ss << ofToString(channel) << ", ";
			}
			
			ofLogWarning() << "Flushed buffer contains " << ss.str();
		}
//	}
  */
}

//--------------------------------------------------------------
void ofApp::draw(){

}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){

}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}
