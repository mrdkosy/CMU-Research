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
#define CELL 32 //2,4,8,10,16,20,32,40,50


class ComputerVision{
    
private:
    OscController osc;
    ofVideoGrabber ironFilingsCamera, peopleCamera;
    ofFbo realIronFilingsImage, goalImage;
    ofImage peopleTestImage, ironfilingsTestImage;
    ofxCvGrayscaleImage grayIronFilingsImage, grayPeopleImage;
    ofxCvColorImage colorIronFilingsImage, colorPeopleImage, trimmedIronFilingsImage;
    ofRectangle trimmedArea; //trimming the image from iron filings camera
    bool isTrimmingMode;
    vector<ofVec2f> trimmedPosition;
    vector<int> cellColor;
    ofVec2f plotterPosition; //plotter position
    bool plotterUp; //plotter stick iron filings or not
    int howHungryPerCell[5] = {0,0,0,0,0};
    
    //--------------------------------------------------------------
    void init(){
        
        realIronFilingsImage.allocate(WIDTH_PROCESS, HEIGHT_PROCESS);
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
        
        //plotter
        plotterPosition = ofVec2f(0,0);
        plotterUp = false;
        osc.reset();
        
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
        realIronFilingsImage.begin();
        if(trimmedArea.isEmpty() || isTrimmingMode) grayIronFilingsImage = colorIronFilingsImage;
        else grayIronFilingsImage = trimmedIronFilingsImage;
        grayIronFilingsImage.draw(0,0);
        realIronFilingsImage.end();
        
        ofPushMatrix();
        ofTranslate(WIDTH_VIEW, HEIGHT_VIEW);
        realIronFilingsImage.draw(0, 0, WIDTH_VIEW, HEIGHT_VIEW);
        drawGrid();
        drawPlotterInformation();
        drawCellColor();
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
                realIronFilingsImage.draw(0, 0, WIDTH_VIEW, HEIGHT_VIEW);
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
    void calculateImageColor(ofTexture& goal, ofTexture& real){
        
        ofPixels goalPixels, realPixels;
        goal.readToPixels(goalPixels);
        real.readToPixels(realPixels);
        
        int nx = floor(ofRandom(WIDTH_PROCESS/CELL))*CELL;
        int ny = floor(ofRandom(HEIGHT_PROCESS/CELL))*CELL;
        const ofVec2f nextPosition = ofVec2f(nx,ny);
        
        ofVec2f aroundCells[5] = {
            ofVec2f(0,0), //center
            ofVec2f(0,-1), //up
            ofVec2f(1,0), //right
            ofVec2f(0,1), //down
            ofVec2f(-1,0) //left
        };
        
        int goalGrayScalePerCell[5] = {0, 0, 0, 0, 0};
        int realGrayScalePerCell[5] = {0, 0, 0, 0, 0};

        
        for(int i=0; i<5; i++){ //check all cell
            for(int y=0; y<CELL; y++){
                for(int x=0; x<CELL; x++){
                    int px = nextPosition.x + aroundCells[i].x*CELL + x;
                    int py = nextPosition.y + aroundCells[i].y*CELL + y;
                    goalGrayScalePerCell[i] += 255 - goalPixels.getColor(px, py).r;
                    realGrayScalePerCell[i] += 255 - realPixels.getColor(px, py).r;
                }
            }
            howHungryPerCell[i] = goalGrayScalePerCell[i] - realGrayScalePerCell[i];
        }
        
        plotterPosition = nextPosition + ofVec2f(CELL/2, CELL/2);
        
    }
    //--------------------------------------------------------------
    void drawPlotterInformation(){
        ofPushStyle();
        ofSetColor(0, 180, 0);
        ofDrawCircle(plotterPosition.x, plotterPosition.y, 10);
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
    void drawCellColor(){
        ofPushStyle();
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
        if(key == 'n') calculateImageColor(goalImage.getTexture(), realIronFilingsImage.getTexture());
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