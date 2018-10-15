//
//  ComputerVision.hpp
//  mainProject
//
//  Created by Maya Atsuki on 10/12/18.
//
//

#ifndef ComputerVision_hpp
#define ComputerVision_hpp

#include <stdio.h>
#include "OscController.hpp"
#include "ofxOpenCv.h"

#define DEBUG
//#define REALTIME_CAPTURE_PEOPLE
//#define REALTIME_CAPTURE_IRONFILINGS
#define WIDTH_PROCESS 640
#define HEIGHT_PROCESS 480
#define WIDTH_VIEW 640
#define HEIGHT_VIEW ((float)HEIGHT_PROCESS/WIDTH_PROCESS)*WIDTH_VIEW
#define UNIT_DISRANCE_PER_SECOND (WIDTH_PROCESS/10)


class ComputerVision{
    
private:
    OscController osc;
    ofVideoGrabber ironFilingsCamera, peopleCamera;
    ofFbo cvIronFilingsImage, goalImage;
    ofImage peopleTestImage, ironfilingsTestImage;
    ofxCvGrayscaleImage grayIronFilingsImage, grayPeopleImage;
    ofxCvColorImage colorIronFilingsImage, colorPeopleImage;
    
//--------------------------------------------------------------
    void init(){

        cvIronFilingsImage.allocate(WIDTH_PROCESS, HEIGHT_PROCESS);
        goalImage.allocate(WIDTH_PROCESS, HEIGHT_PROCESS);
        grayIronFilingsImage.allocate(WIDTH_PROCESS, HEIGHT_PROCESS);
        grayPeopleImage.allocate(WIDTH_PROCESS, HEIGHT_PROCESS);
        colorIronFilingsImage.allocate(WIDTH_PROCESS, HEIGHT_PROCESS);
        colorPeopleImage.allocate(WIDTH_PROCESS, HEIGHT_PROCESS);

        
#ifdef REALTIME_CAPTURE_PEOPLE
        peopleCamera.setVerbose(true);
        peopleCamera.setDeviceID(0);
        peopleCamera.initGrabber(WIDTH_PROCESS, HEIGHT_PROCESS);
#else
        peopleTestImage.load("circle.png");
        peopleTestImage.resize(WIDTH_PROCESS, HEIGHT_PROCESS);
        peopleTestImage.setImageType(OF_IMAGE_COLOR);
        colorPeopleImage = peopleTestImage;
#endif
        
        
        
#ifdef REALTIME_CAPTURE_IRONFILINGS
        ironFilingsCamera.setVerbose(true);
        ironFilingsCamera.setDeviceID(1);
        ironFilingsCamera.initGrabber(WIDTH_PROCESS, HEIGHT_PROCESS);
#else
        ironfilingsTestImage.load("noise.png");
        ironfilingsTestImage.resize(WIDTH_PROCESS, HEIGHT_PROCESS);
        ironfilingsTestImage.setImageType(OF_IMAGE_COLOR);
        colorIronFilingsImage = ironfilingsTestImage;
#endif
    }
    
//--------------------------------------------------------------
    void setup(){
        
    }
//--------------------------------------------------------------
    void update(){
#ifdef REALTIME_CAPTURE_PEOPLE
        peopleCamera.update();
        
#endif
        
#ifdef REALTIME_CAPTURE_IRONFILINGS
        ironFilingsCamera.update();
#endif
    }
    
//--------------------------------------------------------------
    void drawPeople(){
        
        /*******************
         real people image
         *******************/
        
#ifdef REALTIME_CAPTURE_PEOPLE
        colorPeopleImage.setFromPixels(peopleCamera.getPixels());
#endif
        colorPeopleImage.draw(0,0, WIDTH_VIEW, HEIGHT_VIEW);
        drawText("real image of people from camera");
        
        
        /*******************
         goal image
         *******************/
        goalImage.begin();
        grayPeopleImage = colorPeopleImage;
        grayPeopleImage.draw(0,0);
        goalImage.end();
        
        ofPushMatrix();
        ofTranslate(WIDTH_VIEW, 0);
        goalImage.draw(0, 0, WIDTH_VIEW, HEIGHT_VIEW);
        drawText("grayscale image of people from camera");

        ofPopMatrix();
        
        
        
    }
    
//--------------------------------------------------------------
    void drawIronFilings(){
        

        /*******************
         real iron filings condition
         *******************/

#ifdef REALTIME_CAPTURE_IRONFILINGS
        colorIronFilingsImage.setFromPixels(ironfilingsCamera.getPixels());
#endif
        ofPushMatrix();
        ofTranslate(0, HEIGHT_VIEW);
        colorIronFilingsImage.draw(0,0,WIDTH_VIEW, HEIGHT_VIEW);
        drawText("real image of iron filings from camera");

        ofPopMatrix();
        
        
        /*******************
         cv iron filings
         *******************/
        cvIronFilingsImage.begin();
        grayIronFilingsImage = colorIronFilingsImage;
        grayIronFilingsImage.draw(0,0);
        cvIronFilingsImage.end();
        
        ofPushMatrix();
        ofTranslate(WIDTH_VIEW, HEIGHT_VIEW);
        cvIronFilingsImage.draw(0, 0, WIDTH_VIEW, HEIGHT_VIEW);
        drawText("cv image of iron filings");

        ofPopMatrix();

    }
    
//--------------------------------------------------------------
    void drawText(string st){
        ofDrawBitmapStringHighlight(st,ofVec2f(10,20), ofColor(170, 0, 0), ofColor(255));

    }
//--------------------------------------------------------------
//--------------------------------------------------------------
public:
    ComputerVision(){
        init();
        setup();
    }
//--------------------------------------------------------------
    void draw(){
        update();
        
    
        drawPeople();
        drawIronFilings();
    }
    
};
#endif /* ComputerVision_hpp */