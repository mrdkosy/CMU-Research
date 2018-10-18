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
#define REALTIME_CAPTURE_IRONFILINGS
#define WIDTH_PROCESS 640
#define HEIGHT_PROCESS 480
#define WIDTH_VIEW 640
#define HEIGHT_VIEW ((float)HEIGHT_PROCESS/WIDTH_PROCESS)*WIDTH_VIEW
#define UNIT_DISRANCE_PER_SECOND (WIDTH_PROCESS/8.5)
#define CELL 25 //2,4,8,10,16,20,32,40,50
#define RANGE_SEARCH_CELL 10
#define NUM_CELLS_AROUND_TARGET 9 //5 or 9
#define HOW_TINY 4 //min:2 max:cell

class TimeManager{
    float startTime = 0;
    float timeLimit = 0;
public:
    void start(float t){
        startTime = ofGetElapsedTimef();
        timeLimit = t;
        cout << "set time : left -> " << timeLimit << endl;
    }
    float getLeftTime(){
        return MAX(0, timeLimit - (ofGetElapsedTimef() - startTime));
    }
};

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
    ofVec2f plotterPosition, moveToFirst, moveToSecond; //plotter position
    bool plotterUp; //plotter stick iron filings or not
    int howHungryPerCell[9] = {0,0,0,0,0,0,0,0,0};
    TimeManager timeManager;
    
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
        peopleTestImage.load("gradient.png");
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
        isTrimmingMode = true;
        trimmedPosition.clear();
        
        //plotter
        plotterPosition = ofVec2f(CELL/2,CELL/2);
        moveToFirst = plotterPosition;
        moveToSecond = plotterPosition;
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
        if(!isTrimmingMode)calculateImageColor(goalImage.getTexture(), realIronFilingsImage.getTexture());
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
        drawPlotterInformation();
        drawText("grayscale image of people from camera");
        
        ofPopMatrix();
        
        
    }
    
    //--------------------------------------------------------------
    void drawIronFilings(){
        
        
        /*******************
         real iron filings condition
         *******************/
        
#ifdef REALTIME_CAPTURE_IRONFILINGS
        colorIronFilingsImage.setFromPixels(ironFilingsCamera.getPixels());
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
        grayIronFilingsImage.contrastStretch(); //increase the contrast
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
        
        if(timeManager.getLeftTime() == 0){
            
            if( moveToFirst == moveToSecond ){ //caluculate next position
                const ofVec2f nowPosition = moveToSecond;
                
                ofPixels goalPixels, realPixels;
                goal.readToPixels(goalPixels);
                real.readToPixels(realPixels);
                
                //int nx = ( floor(ofRandom(WIDTH_PROCESS/CELL-2)) +1)*CELL;
                //int ny = ( floor(ofRandom(HEIGHT_PROCESS/CELL-2)) +1)*CELL;

                int nx = floor(ofRandom(WIDTH_PROCESS/CELL))*CELL;
                int ny = floor(ofRandom(HEIGHT_PROCESS/CELL))*CELL;

                const ofVec2f nextPosition = ofVec2f(nx,ny);
                
                
                
                bool isExpand[9] = {true, true, true, true, true, true, true, true, true};
                
                int loop = 0;
                while(loop < RANGE_SEARCH_CELL){
                    bool isBreakWhile = false; //break while or not
                    
                    ofVec2f aroundCells[9] = {
                        ofVec2f(0,0)*(loop+1), //center
                        ofVec2f(0,-1)*(loop+1), //up
                        ofVec2f(1,0)*(loop+1), //right
                        ofVec2f(0,1)*(loop+1), //down
                        ofVec2f(-1,0)*(loop+1), //left
                        ofVec2f(1,-1)*(loop+1), //up right
                        ofVec2f(1,1)*(loop+1), //down right
                        ofVec2f(-1,1)*(loop+1), //down left
                        ofVec2f(-1,-1)*(loop+1) //left up
                    };
                    
                    int goalGrayScalePerCell[9] = {0, 0, 0, 0, 0, 0, 0, 0, 0};
                    int realGrayScalePerCell[9] = {0, 0, 0, 0, 0, 0, 0, 0, 0};

                    const int num = NUM_CELLS_AROUND_TARGET;
                    
                    for(int i=0; i<num; i++){ //check all cell
                        if(isExpand[i]){
                            for(int y=0; y<CELL; y++){
                                for(int x=0; x<CELL; x++){
                                    int px = nextPosition.x + aroundCells[i].x*CELL + x;
                                    int py = nextPosition.y + aroundCells[i].y*CELL + y;
                                    if(px < 0 || px >= WIDTH_PROCESS || py < 0 || py >= HEIGHT_PROCESS) isExpand[i] = false;
                                    goalGrayScalePerCell[i] += (255 - MIN(goalPixels.getColor(px, py).r, 230) );
                                    realGrayScalePerCell[i] += (255 - MIN(realPixels.getColor(px, py).r, 230) );
                                    
                                }
                            }
                            howHungryPerCell[i] = (goalGrayScalePerCell[i] - realGrayScalePerCell[i])/(CELL*CELL);
                        }
                    }
                    
                    
                    bool isHungry, isChangeNextPoint;
                    
                    //if the center cell is not need to be moved, go to search next point
                    howHungryPerCell[0] == 0 ? isChangeNextPoint = true : isChangeNextPoint = false;
                    
                    if(!isChangeNextPoint){
                        howHungryPerCell[0] > 0 ? isHungry = true : isHungry = false;

                        int movePositionIndex = -1;
                        for(int i=1; i<num; i++){
                            if(isExpand[i]){
                                if(movePositionIndex < 0) movePositionIndex = i; //init index
                                else{
                                    if(isHungry){ //look for the most enough cell around the center cell
                                        if(howHungryPerCell[movePositionIndex] > howHungryPerCell[i])
                                            movePositionIndex = i;
                                    }else{ //look for the most hungry cell around the center cell
                                        if(howHungryPerCell[movePositionIndex] < howHungryPerCell[i])
                                            movePositionIndex = i;
                                    }
                                }
                            }
                        }
                        
                        
                        
                        if(movePositionIndex < 0){ // no longer move magnet anywhere(there isn't the hungry point)
                            movePositionIndex = MAX(0, movePositionIndex);
                            isChangeNextPoint = true; //search the different point
                        }
                        
                        
                        
                        //check whether need to expand the serching area or not (continue while)
                        if(isHungry){
                            if(howHungryPerCell[movePositionIndex] == 0){
                                isExpand[movePositionIndex] = false;
                            }
                            else if( howHungryPerCell[movePositionIndex] > 0){
                                isBreakWhile = false;
                            }else{
                                isBreakWhile = true;
                            }
                        }else{
                            if(howHungryPerCell[movePositionIndex] == 0){
                                isExpand[movePositionIndex] = false;
                            }
                            else if( howHungryPerCell[movePositionIndex] < 0){
                                isBreakWhile = false;
                            }else{
                                isBreakWhile = true;
                            }
                        }
                        
                        if(isBreakWhile){
                            plotterPosition = nextPosition + ofVec2f(CELL/2, CELL/2);
                            
                            if(isHungry){
                                moveToFirst = nextPosition  + ofVec2f(CELL/2, CELL/2) + aroundCells[movePositionIndex]*CELL;
                                moveToSecond = nextPosition  + ofVec2f(CELL/2, CELL/2);
                                
                                
                            }else{
                                moveToFirst = nextPosition  + ofVec2f(CELL/2, CELL/2);
                                moveToSecond = nextPosition  + ofVec2f(CELL/2, CELL/2) + aroundCells[movePositionIndex]*CELL;
                            }
                            
                            //tiny
                            ofVec2f direction = moveToSecond - moveToFirst;
                            direction.x = ofSign(direction.x);
                            direction.y = ofSign(direction.y);
                            moveToSecond -= direction*ofVec2f(CELL/(float)HOW_TINY, CELL/(float)HOW_TINY);
                            moveToFirst += direction*ofVec2f(CELL/(float)HOW_TINY, CELL/(float)HOW_TINY);
                        }
                        
                    }else{ isBreakWhile = true; }
                    
                    
                    
                    if(isBreakWhile){
                        if(!isChangeNextPoint){
                            plotterUp = false;
                            osc.send(0);
                            ofSleepMillis(500);
                            osc.send(moveToFirst/ofVec2f(WIDTH_PROCESS, HEIGHT_PROCESS));
                            float dist = moveToFirst.distance(nowPosition);
                            timeManager.start(dist/(float)UNIT_DISRANCE_PER_SECOND);
                        }
                        break;
                    }
                    
                    loop++;
                }
                
            }else{ //plotter move to "moveToFirst"
                
                plotterUp = true;
                osc.send(1);
                ofSleepMillis(500);
                osc.send(moveToSecond/ofVec2f(WIDTH_PROCESS, HEIGHT_PROCESS));
                
                
                float dist = moveToSecond.distance(moveToFirst);
                timeManager.start(dist/(float)UNIT_DISRANCE_PER_SECOND);
                
                moveToFirst = moveToSecond;
            }
        }
    }
    //--------------------------------------------------------------
    void drawPlotterInformation(){
        ofPushStyle();
        ofSetColor(150, 0, 0);
        plotterUp == true ? ofFill() : ofNoFill();
        ofDrawCircle(moveToSecond.x, moveToSecond.y, 10);
        ofDrawCircle(moveToFirst.x, moveToFirst.y, 10);
        
        ofVec2f aroundCells[9] = {
            ofVec2f(0,0), //center
            ofVec2f(0,-1), //up
            ofVec2f(1,0), //right
            ofVec2f(0,1), //down
            ofVec2f(-1,0), //left
            ofVec2f(1,-1), //up right
            ofVec2f(1,1), //down right
            ofVec2f(-1,1), //down left
            ofVec2f(-1,-1) //left up
        };
        
        for(int i=0; i<NUM_CELLS_AROUND_TARGET; i++){
            ofDrawBitmapStringHighlight(ofToString(howHungryPerCell[i]),
                                        plotterPosition + aroundCells[i]*40 + ofVec2f(-15, 6),
                                        ofColor(0,100), ofColor(255));
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
        if(key == 'r') osc.reset();
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





