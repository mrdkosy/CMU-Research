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
#define CELL 20 //2,4,8,10,16,20,32,40,50


class ComputerVision{
    
private:
    OscController osc;
    ofVideoGrabber ironFilingsCamera, peopleCamera;
    ofFbo cvIronFilingsImage, goalImage;
    ofImage peopleTestImage, ironfilingsTestImage;
    ofxCvGrayscaleImage grayIronFilingsImage, grayPeopleImage;
    ofxCvColorImage colorIronFilingsImage, colorPeopleImage, trimmedIronFilingsImage;
    ofRectangle trimmedArea; //trimming the image from iron filings camera
    bool isTrimmingMode;
    vector<ofVec2f> trimmedPosition;
    
    //--------------------------------------------------------------
    void init(){
        
        cvIronFilingsImage.allocate(WIDTH_PROCESS, HEIGHT_PROCESS);
        goalImage.allocate(WIDTH_PROCESS, HEIGHT_PROCESS);
        grayIronFilingsImage.allocate(WIDTH_PROCESS, HEIGHT_PROCESS);
        grayPeopleImage.allocate(WIDTH_PROCESS, HEIGHT_PROCESS);
        colorIronFilingsImage.allocate(WIDTH_PROCESS, HEIGHT_PROCESS);
        colorPeopleImage.allocate(WIDTH_PROCESS, HEIGHT_PROCESS);
        trimmedIronFilingsImage.allocate(WIDTH_PROCESS, HEIGHT_PROCESS);
        
        
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
        isTrimmingMode = false;
        trimmedPosition.clear();
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
        trimImage();
        drawText("real image of iron filings from camera");
        
        ofPopMatrix();
        
        
        /*******************
         cv iron filings
         *******************/
        cvIronFilingsImage.begin();
        if(trimmedArea.isEmpty() || isTrimmingMode) grayIronFilingsImage = colorIronFilingsImage;
        else grayIronFilingsImage = trimmedIronFilingsImage;
        grayIronFilingsImage.draw(0,0);
        cvIronFilingsImage.end();
        
        ofPushMatrix();
        ofTranslate(WIDTH_VIEW, HEIGHT_VIEW);
        cvIronFilingsImage.draw(0, 0, WIDTH_VIEW, HEIGHT_VIEW);
        //drawGrid();
        drawText("cv image of iron filings");
        ofPopMatrix();
        
    }
    //--------------------------------------------------------------
    void trimImage(){
        ofPushStyle();
        
        
        if(isTrimmingMode){
            ofSetColor(255, 255);
            colorIronFilingsImage.draw(0,0,WIDTH_VIEW, HEIGHT_VIEW);
            ofFill();
            ofSetColor(0, 100);
            ofDrawRectangle(0,0,WIDTH_VIEW, HEIGHT_VIEW);
            ofSetColor(255, 255);
            ofDrawBitmapString("select trimming area with mouse click", 180, HEIGHT_VIEW/2);
            
            ofSetColor(0,0,200);
            for(int i=0; i<trimmedPosition.size(); i++){
                ofDrawCircle(trimmedPosition[i].x, trimmedPosition[i].y, 5);
            }
            if(!trimmedArea.isEmpty()){
                ofNoFill();
                ofSetColor(0,0,200);
                ofDrawRectangle(trimmedArea);
                
            }
            
        }
        else{
            ofSetColor(255, 255);
            if(trimmedArea.isEmpty()){ // don't set the trimming area yet
                cvIronFilingsImage.draw(0, 0, WIDTH_VIEW, HEIGHT_VIEW);
            }else{
                trimmedIronFilingsImage = colorIronFilingsImage;
                trimmedIronFilingsImage.setROI(trimmedArea);
                trimmedIronFilingsImage.resize(WIDTH_PROCESS, HEIGHT_PROCESS);
                trimmedIronFilingsImage.drawROI(0, 0, WIDTH_VIEW, HEIGHT_VIEW);
            }
            
        }
        ofPopStyle();
    }
    //--------------------------------------------------------------
    void drawText(string st){
        ofDrawBitmapStringHighlight(st,ofVec2f(10,20), ofColor(170, 0, 0), ofColor(255));
    }
    //--------------------------------------------------------------
    void drawGrid(){
        
        ofPushStyle();
        ofSetColor(0, 0, 180);
        for(int x=0; x<=WIDTH_VIEW; x+=CELL){
            ofDrawLine(x, 0, x, HEIGHT_VIEW);
        }
        for(int y=0; y<=HEIGHT_VIEW; y+=CELL){
            ofDrawLine(0, y, WIDTH_VIEW, y);
        }
        ofPopStyle();
        
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
    
    //--------------------------------------------------------------
    void keyPressed(int key){
        if(key == 't') isTrimmingMode = !isTrimmingMode;
        if(key == 'c'){
            if(isTrimmingMode){
                trimmedPosition.clear();
                trimmedArea.setSize(0,0);
            }
        }
    }
    //--------------------------------------------------------------
    void mousePressed(int x, int y){
        
        if(isTrimmingMode){
            if(trimmedPosition.size() < 2){
                int _x = x;
                int _y = y-HEIGHT_VIEW;
                
                if(trimmedPosition.size() == 1){
                    _y = ((float)HEIGHT_PROCESS/WIDTH_PROCESS)*(_x-trimmedPosition[0].x) + trimmedPosition[0].y;
                }
                trimmedPosition.push_back(ofVec2f(_x, _y));
            }
            if(trimmedPosition.size() == 2){
                float _x = trimmedPosition[0].x;
                float _y = trimmedPosition[0].y;
                float _w = abs(trimmedPosition[1].x - trimmedPosition[0].x);
                float _h = abs(trimmedPosition[1].y - trimmedPosition[0].y);
                trimmedArea.setPosition(_x, _y);
                trimmedArea.setSize(_w, _h);
            }
        }
        
    }
    
};
#endif /* ComputerVision_hpp */