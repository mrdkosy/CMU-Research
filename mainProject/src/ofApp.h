#pragma once

#include "ofMain.h"
#include "PlotterController.hpp"
#include "ofxOscSender.h"

//#define DEBUG

class ofApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();

		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void mouseEntered(int x, int y);
		void mouseExited(int x, int y);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);
    
    void sendOscMessage(ofVec2f p);
    void sendOscMessage(float value);
    
    ofxOscSender osc;
    PlotterController controller;
    bool isConnectedOSC;
    
    ofVec2f position;
    float plotValue;
};
