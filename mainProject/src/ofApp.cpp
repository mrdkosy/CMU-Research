#include "ofApp.h"
#define DEBUG
//--------------------------------------------------------------
void ofApp::setup(){
    ofSetBackgroundColor(255);
    ofSetFrameRate(30);
    ofEnableAlphaBlending();
    
    //osc
    osc.setup("128.237.170.35", 12345);
    
    //plotter controller
    controller.init();
    ofSetLineWidth(0.1);
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
    ofVec2f p = controller.getPosition();
    if(p != position){
        
        ofxOscMessage msg;
        msg.setAddress("/plotter/position/");
        msg.addFloatArg(p.x);
        msg.addFloatArg(p.y);
        osc.sendMessage(msg);
        position = p;
#ifdef DEBUG
        cout << "OSC send : ";
        cout << position << endl;
#endif
    }
    
    
    
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    controller.keyPressed(key);
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
    controller.mousePressed(x, y);
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
