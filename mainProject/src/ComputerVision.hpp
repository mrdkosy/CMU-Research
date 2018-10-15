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
    ofFbo realIronFilingsImage, cvIronFilingsImage, realPeopleImage, goalImage;
    ofImage peopleTestImage, ironfilingsTestImage;
    ofxCvGrayscaleImage grayIronFilingsImage, grayPeopleImage;
    ofxCvColorImage colorIronFilingsImage, colorPeopleImage;
    ofPixels grayScaleIronFilingsPixels, grayScalePeoplePixels;
    
//--------------------------------------------------------------
    void init(){
        realIronFilingsImage.allocate(WIDTH_PROCESS, HEIGHT_PROCESS);
        cvIronFilingsImage.allocate(WIDTH_PROCESS, HEIGHT_PROCESS);
        realPeopleImage.allocate(WIDTH_PROCESS, HEIGHT_PROCESS);
        goalImage.allocate(WIDTH_PROCESS, HEIGHT_PROCESS);
        grayIronFilingsImage.allocate(WIDTH_PROCESS, HEIGHT_PROCESS);
        grayPeopleImage.allocate(WIDTH_PROCESS, HEIGHT_PROCESS);
        colorIronFilingsImage.allocate(WIDTH_PROCESS, HEIGHT_PROCESS);
        colorPeopleImage.allocate(WIDTH_PROCESS, HEIGHT_PROCESS);
        grayScalePeoplePixels.allocate(WIDTH_PROCESS, HEIGHT_PROCESS,OF_IMAGE_GRAYSCALE);

        
#ifdef REALTIME_CAPTURE_PEOPLE
        peopleCamera.setVerbose(true);
        peopleCamera.setDeviceID(0);
        peopleCamera.initGrabber(WIDTH_PROCESS, HEIGHT_PROCESS);
#else
        peopleTestImage.load("circle.png");
        peopleTestImage.resize(WIDTH_PROCESS, HEIGHT_PROCESS);
        peopleTestImage.setImageType(OF_IMAGE_COLOR);
#endif
        
        
        
#ifdef REALTIME_CAPTURE_IRONFILINGS
        ironFilingsCamera.setVerbose(true);
        ironFilingsCamera.setDeviceID(1);
        ironFilingsCamera.initGrabber(WIDTH_PROCESS, HEIGHT_PROCESS);
#else
        ironfilingsTestImage.load("noise.png");
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
        
        realPeopleImage.begin();
#ifdef REALTIME_CAPTURE_PEOPLE
        //peopleCamera.draw(0,0);
        colorPeopleImage.setFromPixels(peopleCamera.getPixels());

#else
        colorPeopleImage = peopleTestImage;
        //peopleTestImage.draw(0, 0, WIDTH_PROCESS, HEIGHT_PROCESS);
#endif
        colorPeopleImage.draw(0,0);
        realPeopleImage.end();
        realPeopleImage.draw(0,0, WIDTH_VIEW, HEIGHT_VIEW);
        
        
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
        ofPopMatrix();
        
        
        
    }
    
//--------------------------------------------------------------
    void drawIronFilings(){
        

        /*******************
         real iron filings condition
         *******************/

        realIronFilingsImage.begin();
#ifdef REALTIME_CAPTURE_IRONFILINGS
        ironFilingsCamera.draw(0,0);
#else
        ironfilingsTestImage.draw(0,0, WIDTH_PROCESS, HEIGHT_PROCESS);
#endif
        realIronFilingsImage.end();
        
        ofPushMatrix();
        ofTranslate(0, HEIGHT_VIEW);
        realIronFilingsImage.draw(0,0,WIDTH_VIEW, HEIGHT_VIEW);
        ofPopMatrix();
        
        
        /*******************
         cv iron filings
         *******************/

    }
    
    
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