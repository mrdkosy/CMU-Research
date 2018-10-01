#include "ofApp.h"
#define DEBUG
//--------------------------------------------------------------
void ofApp::setup(){
    ofSetBackgroundColor(255);
    ofSetFrameRate(60);
    
    //osc
    osc.setup("localhost", 12345);
    
    //plotter controller
    controller.init();
}

//--------------------------------------------------------------
void ofApp::update(){
#ifdef DEBUG
    if(ofGetFrameNum() % 60 == 0) cout << "fps : " << ofGetFrameRate() << endl;
#endif
    //plotter controller
    controller.update();
    
}

//--------------------------------------------------------------
void ofApp::draw(){
    
    //plotter controller
    controller.draw();
    
    //osc
    ofxOscMessage msg;
    int x = controller.getPosition().x;
    int y = controller.getPosition().y;
    msg.setAddress("/positionX");
    msg.addIntArg(x);
    msg.setAddress("/positionY");
    msg.addIntArg(y);
    osc.sendMessage(msg);
    

    
    
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
