#include "ofApp.h"
#define DEBUG
//--------------------------------------------------------------
void ofApp::setup(){
    ofSetBackgroundColor(255);
    ofSetFrameRate(30);
    ofEnableAlphaBlending();
    
    //osc
    osc.setup("128.237.130.218", 12345);
    
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
    float v = controller.getPlotValue();
    if(v != plotValue){
        sendOscMessage(v); //*0.35
        plotValue = v;
    }

    ofVec2f p = controller.getPosition();
    if(p != position && p.x >= 0 && p.y >= 0){
        sendOscMessage(p);
        position = p;
    }
    
    
    
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    controller.keyPressed(key);
    if(key == 'o'){
        sendOscMessage(ofVec2f(0,0));
    }
}
//--------------------------------------------------------------
void ofApp::sendOscMessage(ofVec2f p){
    ofxOscMessage msg;
    msg.setAddress("/plotter/position/");
    msg.addFloatArg(p.x);
    msg.addFloatArg(p.y);
    osc.sendMessage(msg);
#ifdef DEBUG
    cout << "OSC send : ";
    cout << p << endl;
#endif
}
void ofApp::sendOscMessage(float value){
    ofxOscMessage msg;
    msg.setAddress("/plotter/plotvalue/");
    msg.addFloatArg(value);
    osc.sendMessage(msg);
#ifdef DEBUG
    cout << "OSC send : ";
    cout << value << endl;
#endif
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
