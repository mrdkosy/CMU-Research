#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    ofSetBackgroundColor(150);
    ofSetFrameRate(60);
    
    //osc
    osc.setup("localhost", 12345);
    
    //plotter controller
    controller.init();
}

//--------------------------------------------------------------
void ofApp::update(){
    if(ofGetFrameNum() % 60 == 0) cout << ofGetFrameRate() << endl;
    
    //plotter controller
    controller.update();
    
}

//--------------------------------------------------------------
void ofApp::draw(){
    
    //plotter controller
    controller.draw();
    
    //osc
    ofxOscMessage msg;
    msg.setAddress("/position");
    msg.addIntArg(0);
    osc.sendMessage(msg);
    
    //test
    
    
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){

}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

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
