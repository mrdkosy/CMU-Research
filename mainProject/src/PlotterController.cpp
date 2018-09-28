//
//  PlotterController.cpp
//  main
//
//  Created by Maya Atsuki on 2018/09/28.
//

#include "PlotterController.hpp"
//#define DEBUG
#define REALTIME_CAPTURE_PEOPLE
#define WIDTH 640
#define HEIGHT 480
//--------------------------------------------------------------
void PlotterController::init(){
    
#ifdef REALTIME_CAPTURE_PEOPLE
    //default aspect 4:3
    capturePeople.setVerbose(true);
    capturePeople.setDeviceID(1);
    capturePeople.initGrabber(WIDTH, HEIGHT);
#else
    imagePeople.load("circle.png");
#endif
    
    
    //default aspect 9:5 , fixed to 4:3
    captureSand.setVerbose(true);
    captureSand.setDeviceID(0);
    captureSand.initGrabber(WIDTH, HEIGHT);
    

    shader.load("", "");
    
#ifdef DEBUG
    capturePeople.listDevices();
#endif
    

}
//--------------------------------------------------------------
void PlotterController::update(){
    
#ifdef REALTIME_CAPTURE_PEOPLE
    capturePeople.update();
#endif
    
    captureSand.update();
    
}

//--------------------------------------------------------------
void PlotterController::draw(){
    
#ifdef REALTIME_CAPTURE_PEOPLE
    capturePeople.draw(0,0);
#else
    imagePeople.draw(0,0, WIDTH, HEIGHT);
#endif
    
    
    ofPushMatrix();
    ofTranslate(0, HEIGHT);
    captureSand.draw(0, 0);
    ofPopMatrix();
    
}
//--------------------------------------------------------------
void PlotterController::imageFilterShader(){
    
    shader.begin();
    float coef[] = {
        1.0,  1.0,  1.0,
        1.0, -8.0,  1.0,
        1.0,  1.0,  1.0
    };

    shader.setUniform2f("resolution", ofGetWidth(), ofGetHeight());
    ofSetColor(255,255);
    glBegin(GL_TRIANGLE_STRIP);
    glTexCoord2d(0, 0);
    glVertex2d(0, 0);
    
    glTexCoord2d(WIDTH, 0);
    glVertex2d(WIDTH, 0);
    
    glTexCoord2d(0, HEIGHT);
    glVertex2d(0, HEIGHT);
    
    glTexCoord2d(WIDTH, HEIGHT);
    glVertex2d(WIDTH, HEIGHT);
    glEnd();
    
    shader.end();
}
//--------------------------------------------------------------
ofVec2f PlotterController::getPosition(){
    return position;
}

