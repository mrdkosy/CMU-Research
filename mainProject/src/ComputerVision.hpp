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
#include "GuiManager.hpp"
#include "ofxOpenCv.h"
#include "ofxCv.h"

//#define DEBUG
//#define REALTIME_CAPTURE_PEOPLE
#define REALTIME_CAPTURE_IRONFILINGS
#define WIDTH_PROCESS 640
#define HEIGHT_PROCESS 480
#define WIDTH_VIEW 640
#define HEIGHT_VIEW (((float)HEIGHT_PROCESS/WIDTH_PROCESS)*WIDTH_VIEW)
//#define UNIT_DISRANCE_PER_SECOND (WIDTH_PROCESS/8.5)
//#define CELL 30 //2,4,8,10,16,20,32,40,50
//#define STORAGE_OF_FILINGS 40
#define RANGE_SEARCH_CELL 200
#define NUM_CELLS_AROUND_TARGET 5 //5 or 9
#define HOW_TINY 0 //min:2 max:cell 0:just center


class TimeManager{
    float startTime = 0;
    float timeLimit = 0;
public:
    void start(float dist){
        startTime = ofGetElapsedTimef();
        timeLimit = dist;
#ifdef DEBUG
        cout << "set time : left -> " << timeLimit << endl;
#endif
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
    ofVec2f plotterPosition, moveToFirst, moveToSecond, moveInStorage; //plotter position
    bool plotterUp; //plotter stick iron filings or not
    int howHungryPerCell[9] = {0,0,0,0,0,0,0,0,0};
    TimeManager timeManager;
    bool isColorDebugMode = false; //you can search the color by your mouse
    bool isCalibrationMode; //calibration
    int CELL_SIZE;
    ofRectangle storageOfFilings;
    bool isManageStorageMode, isMoveFilingsToWhiteArea;
    int STEP;
    vector<ofVec2f> stockPosition;
    int stockPositionIndex;
    ofVec2f cornerPoints[5] = {ofVec2f(0,0), ofVec2f(WIDTH_PROCESS,0), ofVec2f(WIDTH_PROCESS,HEIGHT_PROCESS), ofVec2f(0,HEIGHT_PROCESS),ofVec2f(0,0)};
    int COUNTER;
    ofImage horizonalSobel, verticalSobel, thresholdImg;
    
    int CELL, STORAGE_OF_FILINGS;
    float UNIT_DISRANCE_PER_SECOND;
    
    GuiManager gui;
    
    //--------------------------------------------------------------
    void init(){
        
        realIronFilingsImage.allocate(WIDTH_PROCESS, HEIGHT_PROCESS);
        goalImage.allocate(WIDTH_PROCESS, HEIGHT_PROCESS);
        grayIronFilingsImage.allocate(WIDTH_PROCESS, HEIGHT_PROCESS);
        grayPeopleImage.allocate(WIDTH_PROCESS, HEIGHT_PROCESS);
        colorIronFilingsImage.allocate(WIDTH_PROCESS, HEIGHT_PROCESS);
        colorPeopleImage.allocate(WIDTH_PROCESS, HEIGHT_PROCESS);
        trimmedIronFilingsImage.allocate(WIDTH_PROCESS, HEIGHT_PROCESS);
        
        horizonalSobel.allocate(WIDTH_PROCESS, HEIGHT_PROCESS, OF_IMAGE_COLOR);
        verticalSobel.allocate(WIDTH_PROCESS, HEIGHT_PROCESS, OF_IMAGE_COLOR);
        thresholdImg.allocate(WIDTH_PROCESS, HEIGHT_PROCESS, OF_IMAGE_COLOR);
        
#ifdef REALTIME_CAPTURE_PEOPLE
        peopleCamera.setVerbose(true);
        peopleCamera.setDeviceID(0);
        peopleCamera.initGrabber(WIDTH_PROCESS, HEIGHT_PROCESS);
#else
        peopleTestImage.load("mountain.png");
        peopleTestImage.resize(WIDTH_PROCESS, HEIGHT_PROCESS);
        peopleTestImage.setImageType(OF_IMAGE_COLOR);
        colorPeopleImage = peopleTestImage;
#endif
        
        
        
#ifdef REALTIME_CAPTURE_IRONFILINGS
        ironFilingsCamera.setVerbose(true);
        ironFilingsCamera.setDeviceID(1);
        ironFilingsCamera.initGrabber(WIDTH_PROCESS, HEIGHT_PROCESS);
#else
        ironfilingsTestImage.load("1087.png");
        ironfilingsTestImage.resize(WIDTH_PROCESS, HEIGHT_PROCESS);
        ironfilingsTestImage.setImageType(OF_IMAGE_COLOR);
        colorIronFilingsImage = ironfilingsTestImage;
#endif
    }
    
    //--------------------------------------------------------------
    void setup(){
        isTrimmingMode = true;
        trimmedPosition.clear();
        isColorDebugMode = false;
        isCalibrationMode = false;
        
        //plotter
        plotterPosition = ofVec2f(0,0);
        moveToFirst = plotterPosition;
        moveToSecond = plotterPosition;
        moveInStorage = plotterPosition;
        plotterUp = false;
        osc.reset();
        
        
        
        //set strage
        STORAGE_OF_FILINGS = gui.getStorageValue();
        int sh = STORAGE_OF_FILINGS*2;
        int sw = ((WIDTH_PROCESS/(float)HEIGHT_PROCESS)*STORAGE_OF_FILINGS)*2;
        float h = HEIGHT_PROCESS - sh;
        float w = WIDTH_PROCESS - sw;
        storageOfFilings.setPosition(sw/2, sh/2);
        storageOfFilings.setSize(w, h);
        
        CELL = gui.CELL;
        UNIT_DISRANCE_PER_SECOND = (WIDTH_PROCESS/gui.UNIT_DISRANCE_PER_SECOND);
        
        isManageStorageMode = false;
        STEP = 0;
        COUNTER = 0;
        
    }
    //--------------------------------------------------------------
    void update(){
#ifdef REALTIME_CAPTURE_PEOPLE
        peopleCamera.update();
#endif
        
#ifdef REALTIME_CAPTURE_IRONFILINGS
        ironFilingsCamera.update();
#endif
        
        if( (!isTrimmingMode)&&(!isColorDebugMode) && (!isCalibrationMode)){
            if(timeManager.getLeftTime() == 0){
                const int minX = storageOfFilings.getX();
                const int maxX = minX + storageOfFilings.getWidth();
                const int minY = storageOfFilings.getY();
                const int maxY = minY + storageOfFilings.getHeight();
                
                if(!isManageStorageMode){
                    
                    int range = 50;
                    
                    if(plotterPosition.x < minX || plotterPosition.x > maxX || plotterPosition.y < minY || plotterPosition.y > maxY){
                        plotterPosition = ofVec2f(ofRandom(minX, maxX), ofRandom(minY, maxY));
                    }
                    
                        ofVec2f rand;
                    if(plotterPosition.x <= minX+range) rand.x = ofRandom(plotterPosition.x-minX, range*2);
                    else if(plotterPosition.x >= maxX-range) rand.x = ofRandom(-range*2, maxX-plotterPosition.x);
                    else rand.x = ofRandom(-range, range);
                    
                    if(plotterPosition.y <= minY+range) rand.y = ofRandom(plotterPosition.y-minY, range*2);
                    else if(plotterPosition.y >= maxY-range) rand.y = ofRandom(-range*2, maxY-plotterPosition.y);
                    else rand.y = ofRandom(-range, range);
                    
                    int nx = plotterPosition.x + rand.x;
                    int ny = plotterPosition.y + rand.y;
                    
                    callCalculateImageColor(ofVec2f(nx, ny));
                    
                }else{ //manage the storage of filings
                    calculateFilingsStorage();
                }
                
            }
        }
        
        updateGUI();
    }
    //--------------------------------------------------------------
    void updateGUI(){
        
        if(gui.trimmingMode) isTrimmingMode = true;
        else isTrimmingMode = false;
        
        if(isTrimmingMode){
            if(gui.clearPosition){
                trimmedPosition.clear();
                trimmedArea.setSize(0,0);
            }
            ofVec2f p = plotterPosition/ofVec2f(WIDTH_PROCESS, HEIGHT_PROCESS); //0-1
            float plus = 0.001;
            
            if(gui.setRange) osc.setRange(0, p.x, 0, p.y);
            if(gui.point_WH){
                osc.moveToMax();
                plotterPosition = osc.getRangeMax() * ofVec2f(WIDTH_PROCESS, HEIGHT_PROCESS);
            }
            if(gui.point_00){
                osc.moveToMax();
                plotterPosition = osc.getRangeMax() * ofVec2f(WIDTH_PROCESS, HEIGHT_PROCESS);
            }
        }
        if(gui.mouseDebugMode) isColorDebugMode = true;
        else isColorDebugMode = false;
        
        if(gui.screenShot){
            ofPixels pixels;
            pixels = colorIronFilingsImage.getPixels();
            string name = "screenshot/";
            name += ofToString(ofGetFrameNum());
            name += ".png";
            ofSaveImage(pixels, name);
        }
        
        CELL = gui.CELL;
        UNIT_DISRANCE_PER_SECOND = (WIDTH_PROCESS/gui.UNIT_DISRANCE_PER_SECOND);
        
        if(gui.getIsStorageChanged()){
            STORAGE_OF_FILINGS = gui.getStorageValue();
            int sh = STORAGE_OF_FILINGS*2;
            int sw = ((WIDTH_PROCESS/(float)HEIGHT_PROCESS)*STORAGE_OF_FILINGS)*2;
            float h = HEIGHT_PROCESS - sh;
            float w = WIDTH_PROCESS - sw;
            storageOfFilings.setPosition(sw/2, sh/2);
            storageOfFilings.setSize(w, h);
        }
        
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
        ofClear(255, 255);
        grayPeopleImage = colorPeopleImage;
        grayPeopleImage.contrastStretch();
        grayPeopleImage.threshold(230);
        int x = storageOfFilings.getX();
        int y = storageOfFilings.getY();
        int w = storageOfFilings.getWidth();
        int h = storageOfFilings.getHeight();
        grayPeopleImage.draw(x, y, w, h);
        goalImage.end();
        
        ofPushMatrix();
        ofTranslate(WIDTH_VIEW, 0);
        goalImage.draw(0, 0, WIDTH_VIEW, HEIGHT_VIEW);
        drawPlotterInformation();
        drawStorage();
        drawText("grayscale image of people from camera");
        ofPopMatrix();
        
        
        
        /*******************
         sobel filter
         *******************/
        ofPixels pixels = grayPeopleImage.getPixels();
        
        ofPushMatrix();
        ofTranslate(WIDTH_VIEW*2, 0);
        cv::Mat src = ofxCv::toCv(ofImage(pixels));
        cv::Mat blurimg;
        cv::blur(src, blurimg, cv::Size(5,5));
        cv::Mat kernel = (cv::Mat1f(3,3)<<-1,0,1,-2,0,2,-1,0,1);
        cv::Mat sobelimg;
        cv::filter2D(blurimg, sobelimg, src.depth(), kernel);
        ofxCv::toOf(sobelimg, horizonalSobel);
        horizonalSobel.update();
        horizonalSobel.draw(0, 0, 400, 300);
        drawText("horizonal sobel");
        
        ofTranslate(0, 300);
        src = ofxCv::toCv(ofImage(pixels));
        cv::blur(src, blurimg, cv::Size(5,5));
        kernel = (cv::Mat1f(3,3)<<1,2,1,0,0,0,-1,-2,-1);
        cv::filter2D(blurimg, sobelimg, src.depth(), kernel);
        ofxCv::toOf(sobelimg, verticalSobel);
        verticalSobel.update();
        verticalSobel.draw(0, 0, 400, 300);
        drawText("vertical sobel");
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
        ofDrawBitmapStringHighlight(ofToString(plotterPosition), ofVec2f(10, 50),ofColor(170, 0, 0), ofColor(255));
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
        //drawGrid();
        drawPlotterInformation();
        if(isColorDebugMode || isCalibrationMode) searchColorByMouse();
        drawStorage();
        drawText("cv image of iron filings");
        ofPopMatrix();
        
        
    }
    //--------------------------------------------------------------
    void searchColorByMouse(){
        ofPushStyle();
        
        ofSetColor(0, 100);
        ofDrawRectangle(0,0,WIDTH_VIEW, HEIGHT_VIEW);
        ofSetColor(255, 255);
        
        if(isColorDebugMode)
            ofDrawBitmapString("you can search the cell color by your mouse", 180, HEIGHT_VIEW/2);
        if(isCalibrationMode)
            ofDrawBitmapString("calibraion mode", 180, HEIGHT_VIEW/2);
        ofPopStyle();
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
                ofDrawCircle(trimmedPosition[i].x, trimmedPosition[i].y, 2);
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
    void callCalculateImageColor(ofVec2f np){
        
        calculateImageColor(goalImage.getTexture(), realIronFilingsImage.getTexture(), np);
    }
    //--------------------------------------------------------------
    void calculateImageColor(ofTexture& goal, ofTexture& real, ofVec2f np){
        
        
        //if( moveToFirst == moveToSecond ){ //caluculate next position
        if( STEP == 0 ){ //caluculate next position
            const ofVec2f nowPosition = moveToSecond;
            
            ofPixels goalPixels, realPixels;
            goal.readToPixels(goalPixels);
            real.readToPixels(realPixels);
            
            const ofVec2f nextPosition = np; //ofVec2f(nx,ny);
            
            
            bool isExpand[9] = {true, true, true, true, true, true, true, true, true};
            int firstWallIndex = -1; //the place of iron filings storage
            
            cout << "---------" << endl;
            int loop = 0;
            while(loop < RANGE_SEARCH_CELL){
                bool isBreakWhile = false; //break while or not
                
                
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
                
                int goalGrayScalePerCell[9] = {0, 0, 0, 0, 0, 0, 0, 0, 0};
                int realGrayScalePerCell[9] = {0, 0, 0, 0, 0, 0, 0, 0, 0};
                
                const int _CELL_SIZE = CELL + 2*loop;
                const int HALF_CELL = _CELL_SIZE/2;
                
                const int minX = storageOfFilings.getX();
                const int maxX = minX + storageOfFilings.getWidth();
                const int minY = storageOfFilings.getY();
                const int maxY = minY + storageOfFilings.getHeight();
                
                //calcurate the colors of all cells
                int counter = 0;
                for(int i=0; i<NUM_CELLS_AROUND_TARGET; i++){ //check all cell
                    if(isExpand[i]){
                        counter = 0;
                        for(int y=-HALF_CELL; y<HALF_CELL; y++){
                            for(int x=-HALF_CELL; x<HALF_CELL; x++){
                                int px = nextPosition.x + aroundCells[i].x*_CELL_SIZE + x;
                                int py = nextPosition.y + aroundCells[i].y*_CELL_SIZE + y;
                                //if(px < 0 || px >= WIDTH_PROCESS || py < 0 || py >= HEIGHT_PROCESS){ //attack wall
                                if(px < minX || px >= maxX || py < minY || py >= maxY){ //attack wall
                                    isExpand[i] = false;
                                    if(firstWallIndex < 0) firstWallIndex = i;
                                }
                                goalGrayScalePerCell[i] += (255 - MIN(goalPixels.getColor(px, py).r, 230) );
                                realGrayScalePerCell[i] += (255 - MIN(realPixels.getColor(px, py).r, 230) );
                                counter++;
                                
                            }
                        }
                        howHungryPerCell[i] = (goalGrayScalePerCell[i] - realGrayScalePerCell[i])/MAX(counter,1);
                    }
                }
                
                
                bool isHungry, isChangeNextPoint;
                
                //if the center cell is not need to be moved, go to search next point
                howHungryPerCell[0] == 0 ? isChangeNextPoint = true : isChangeNextPoint = false;
                
                
                
                if(!isExpand[0]){
                    isBreakWhile = true;
                    //isChangeNextPoint = true;
                    cout << "firstWallIndex : " << firstWallIndex << endl;
                }
                
                
                if(!isChangeNextPoint){
                    howHungryPerCell[0] > 0 ? isHungry = true : isHungry = false;
                    
                    int movePositionIndex = -1;
                    for(int i=1; i<NUM_CELLS_AROUND_TARGET; i++){
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
                        
                        cout << "movePositionIndex < 0" << endl;
                        if(firstWallIndex > 0){//there is the wall near the target cell
                            isChangeNextPoint = false;
                            cout << "first wall index0 :" << firstWallIndex << endl;
                            
                        }else{ //search the different point
                            isChangeNextPoint = true;
                            cout << "first wall index0 :" << firstWallIndex << endl;
                        }
                        
                        movePositionIndex = MAX(0, movePositionIndex);
                        isBreakWhile = true;
                        
                        
                    }else{ //check whether need to expand the serching area or not (continue while)
                        if(isExpand[0]){
                            if(isHungry){
                                if(howHungryPerCell[movePositionIndex] == 0){
                                    //isExpand[movePositionIndex] = false;
                                    isBreakWhile = false;
                                }
                                else if( howHungryPerCell[movePositionIndex] > 0){
                                    isBreakWhile = false;
                                }else{
                                    isBreakWhile = true;
                                }
                            }else{
                                if(howHungryPerCell[movePositionIndex] == 0){
                                    //isExpand[movePositionIndex] = false;
                                    isBreakWhile = false;
                                }
                                else if( howHungryPerCell[movePositionIndex] < 0){
                                    isBreakWhile = false;
                                }else{
                                    isBreakWhile = true;
                                }
                            }
                        }
                    }
                    
                    
                    
                    
                    if(isBreakWhile){
                        
                        cout << "this is 'break while'" << endl;
                        
                        plotterPosition = nextPosition;
                        
                        if(firstWallIndex > 0){ //wether manage the storage of filings or not
                            
                            cout << "first wall index1 : " << firstWallIndex << endl;
                            
                            
                            float x, y;
                            aroundCells[firstWallIndex].x == 0 ? x = nextPosition.x : x = MAX(aroundCells[firstWallIndex].x,0)*WIDTH_PROCESS;
                            aroundCells[firstWallIndex].y == 0 ? y = nextPosition.y : y = MAX(aroundCells[firstWallIndex].y,0)*HEIGHT_PROCESS;
                            
                            if(isHungry){
                                moveToFirst = ofVec2f(x,y);
                                moveToSecond = nextPosition;
                                //isMoveFilingsToWhiteArea = false;
                                //STEP = 2;
                            }else{
                                moveToFirst = nextPosition;
                                moveToSecond = ofVec2f(x,y);
                                //isMoveFilingsToWhiteArea = true;
                                //STEP = 2;
                            }
                            //isManageStorageMode = true;
                            
                            
                            
                        }else if(isHungry){
                            moveToFirst = nextPosition + aroundCells[movePositionIndex]*_CELL_SIZE;
                            moveToSecond = nextPosition;
                            
                            
                        }else{
                            moveToFirst = nextPosition;
                            moveToSecond = nextPosition + aroundCells[movePositionIndex]*_CELL_SIZE;
                        }
                        
                        //tiny
                        ofVec2f direction = moveToSecond - moveToFirst;
                        direction.x = ofSign(direction.x);
                        direction.y = ofSign(direction.y);
                        if( HOW_TINY != 0 ){
                            moveToSecond -= direction*ofVec2f( _CELL_SIZE/(float)HOW_TINY, _CELL_SIZE/(float)HOW_TINY );
                            moveToFirst += direction*ofVec2f( _CELL_SIZE/(float)HOW_TINY, _CELL_SIZE/(float)HOW_TINY );
                        }
                    }
                    
                }else{
                    isBreakWhile = true;
                }
                
                CELL_SIZE = _CELL_SIZE;
                
                if(isBreakWhile){
                    if(!isChangeNextPoint){
                        plotterUp = false;
                        osc.plotterDown();
                        osc.send(moveToFirst/ofVec2f(WIDTH_PROCESS, HEIGHT_PROCESS));
                        float dist = moveToFirst.distance(nowPosition);
                        timeManager.start(dist/UNIT_DISRANCE_PER_SECOND);
                        if(!isManageStorageMode) STEP = 1;
                        
                    }
                    break;
                }
                
                loop++;
            }
            
        }else if(STEP == 1){ //plotter move to "moveToFirst"
            
            
            plotterUp = true;
            osc.plotterUp();
            osc.send(moveToSecond/ofVec2f(WIDTH_PROCESS, HEIGHT_PROCESS));
            
            
            float dist = moveToSecond.distance(moveToFirst);
            timeManager.start(dist/UNIT_DISRANCE_PER_SECOND);
            
            
            COUNTER++;
            if(COUNTER > gui.COUNTER_LIMIT){
                isManageStorageMode = true;
                STEP = 5;
                COUNTER = 0;
            }else{
                STEP = 0;
            }
            
            
        }
    }
    //--------------------------------------------------------------
    void calculateFilingsStorage(){
        
        const ofVec2f firstPosition = moveToFirst;
        const ofVec2f secondPosition = moveToSecond;
        
        const int minX = storageOfFilings.getX();
        const int maxX = minX + storageOfFilings.getWidth();
        const int minY = storageOfFilings.getY();
        const int maxY = minY + storageOfFilings.getHeight();
        const int storageWidth = minX;
        const int storageHeight = minY;
        
        
        
        if(STEP == 2){
            
            stockPosition.clear();
            
            ofVec2f p;
            if(isMoveFilingsToWhiteArea){
                p = secondPosition;
            }else{
                stockPosition.push_back(secondPosition);
                p = firstPosition;
            }
            
            if(p.x <= minX) p.x  = storageWidth/2;
            else if(p.x >= maxX) p.x = maxX + storageWidth/2;
            if(p.y <= minY) p.y = storageHeight/2;
            else if(p.y >= maxY) p.y = maxY + storageHeight/2;
            
            
            stockPosition.push_back(p);
            moveInStorage = p;
            STEP = 3;
            
        }
        if(STEP == 3){
            
            
            int d = 1;
            if(int(moveToFirst.x)%2 == 0) d = -1; //just random
            
            
            ofVec2f _p = stockPosition.at(stockPosition.size() - 1);
            
            ofVec2f direction;
            if(_p.x < minX || maxX <= _p.x){
                direction.x = 0;
                direction.y = d;
            }else if(_p.y < minY || maxY <= _p.y){
                direction.x = d;
                direction.y = 0;
            }
            
            ofPixels realPixels;
            realIronFilingsImage.readToPixels(realPixels);
            
            int i=0;
            while(true){
                bool isBreakWhile = false;
                
                int _minX, _maxX, _minY, _maxY;
                
                int last_index = stockPosition.size() - 1;
                ofVec2f pos = stockPosition.at(last_index);
                
                ofVec2f norm = ofVec2f(1-abs(direction.x), 1-abs(direction.y));
                ofVec2f mul = norm*ofVec2f(storageWidth, storageHeight);
                int len = mul.length();
                ofVec2f pul = direction * i;
                
                
                _minX = pos.x - len/2 + pul.x;
                _maxX = pos.x + len/2 + pul.x;
                _minY = pos.y - len/2 + pul.y;
                _maxY = pos.y + len/2 + pul.y;
                
                
                
                if(_minX < 0 || _maxX > WIDTH_PROCESS || _minY < 0 || _maxY > HEIGHT_PROCESS){
                    
                    ofVec2f p = pos+pul;
                    ofVec2f d = ofVec2f(abs(direction.y), abs(direction.x));
                    
                    int cell;
                    if(d.x == 1) cell = storageHeight/2;
                    else if(d.y == 1) cell = storageWidth/2;
                    
                    if(p.x < minX){
                        if(p.y < minY){
                            p.x = cell;
                            p.y = cell;
                        }else if(p.y > maxY){
                            p.x = cell;
                            p.y = HEIGHT_PROCESS - cell;
                        }
                    }else if(p.x > maxX){
                        if(p.y < minY){
                            p.x = WIDTH_PROCESS - cell;
                            p.y = cell;
                        }else if(p.y > maxY){
                            p.x = WIDTH_PROCESS - cell;
                            p.y = HEIGHT_PROCESS - cell;
                        }
                    }
                    
                    
                    stockPosition.push_back(p);
                    
                    p.x < storageWidth ? direction.x = d.x : direction.x = -1*d.x;
                    p.y < storageHeight ? direction.y = d.y : direction.y = -1*d.y;
                    
                    i = 0;
                    
                    
                }else{
                    
                    int blackScale = 0;
                    int counter = 0;
                    for(int y=_minY; y<_maxY; y++){
                        for(int x=_minX; x<_maxX; x++){
                            ofColor c = realPixels.getColor(x,y);
                            blackScale += c.r;
                            counter++;
                        }
                    }
                    blackScale /= MAX(counter,1);
                    blackScale = 255-blackScale;
                    
                    
                    if(isMoveFilingsToWhiteArea){
                        if(blackScale < 150){ //this is white area
                            isBreakWhile = true;
                            stockPosition.push_back(pos+pul);
                        }
                    }else{
                        if(blackScale > 150){ //this is black area
                            isBreakWhile = true;
                            stockPosition.push_back(pos+pul);
                        }
                    }
                    
                }
                
                
                if(stockPosition.size() > 5){
                    isBreakWhile = true;
                }
                
                if(isBreakWhile) {
                    if(isMoveFilingsToWhiteArea) stockPositionIndex = 0;
                    else stockPositionIndex = stockPosition.size() -1;
                    
                    STEP = 4;
                    break;
                }
                i++;
                
            }
        }
        
        
        if(STEP == 4){
            
            bool isNext = false;
            if(isMoveFilingsToWhiteArea){
                
                if(stockPositionIndex < stockPosition.size()){
                    float dist;
                    ofVec2f rand = ofVec2f(ofRandom(-storageWidth/2, storageWidth/2),
                                           ofRandom(-storageHeight/2, storageHeight/2));
                    ofVec2f p = stockPosition.at(stockPositionIndex);
                    if(stockPositionIndex == 0){
                        plotterUp = true;
                        osc.plotterUp();
                        dist = firstPosition.distance(p);
                        rand = ofVec2f(0,0);
                    }else{
                        dist = stockPosition.at(stockPositionIndex-1).distance(p);
                    }
                    p = p+rand;
                    osc.send(p/ofVec2f(WIDTH_PROCESS, HEIGHT_PROCESS));
                    timeManager.start(dist/UNIT_DISRANCE_PER_SECOND);
                    moveInStorage = p;
                    stockPositionIndex++;
                }else{
                    moveToSecond = stockPosition.at(stockPosition.size()-1);
                    isNext = true;
                }
                
            }
            else{
                if(stockPositionIndex >= 0){
                    float dist;
                    ofVec2f rand = ofVec2f(ofRandom(-storageWidth/2, storageWidth/2),
                                           ofRandom(-storageHeight/2, storageHeight/2));
                    
                    ofVec2f p = stockPosition.at(stockPositionIndex);
                    if(stockPositionIndex == (stockPosition.size()-1)){
                        plotterUp = false;
                        osc.plotterDown();
                        dist = firstPosition.distance(p);
                        rand = ofVec2f(0,0);
                    }else if(stockPositionIndex == (stockPosition.size()-2)){
                        plotterUp = true;
                        osc.plotterUp();
                        dist = stockPosition.at(stockPositionIndex+1).distance(p);
                    }else{
                        dist = stockPosition.at(stockPositionIndex+1).distance(p);
                    }
                    p = p+rand;
                    osc.send(p/ofVec2f(WIDTH_PROCESS, HEIGHT_PROCESS));
                    timeManager.start(dist/UNIT_DISRANCE_PER_SECOND);
                    moveInStorage = p;
                    stockPositionIndex--;
                }else{
                    moveToSecond = stockPosition.at(0);
                    isNext = true;
                }
            }
            
            if(isNext){
                isManageStorageMode = false;
                STEP = 0;
                plotterUp = false;
                osc.plotterDown();
            }
            cout << "stockPositionIndex : " << stockPositionIndex << endl;
        }
        
        if(STEP == 5){
            float dist;
            ofVec2f p = cornerPoints[COUNTER];
            if(COUNTER == 0){
                plotterUp = false;
                osc.plotterDown();
                dist = moveToSecond.distance(p);
            }else if(COUNTER == 1){
                plotterUp = true;
                osc.plotterUp();
                dist = p.distance(cornerPoints[COUNTER-1]);
            }else{
                dist = p.distance(cornerPoints[COUNTER-1]);
            }
            int dx, dy;
            p.x == 0 ? dx = 1 : dx = -1;
            p.y == 0 ? dy = 1 : dy = -1;
            
            ofVec2f rand = ofVec2f(ofRandom(storageWidth), ofRandom(storageHeight)) * ofVec2f(dx, dy);
            p = p + rand;
            osc.send(p/ofVec2f(WIDTH_PROCESS, HEIGHT_PROCESS));
            timeManager.start(dist/UNIT_DISRANCE_PER_SECOND);
            plotterPosition = p;
            
            if(COUNTER == 4){
                isManageStorageMode = false;
                COUNTER = 0;
                STEP = 0;
                moveToSecond = p;
                //plotterPosition = ofVec2f(ofRandom(minX, maxX), ofRandom(minY, maxY)); //!!!!!!!!!!!!
            }
            COUNTER++;
        }
        
        
    }
    
    //--------------------------------------------------------------
    void drawStorage(){
        ofPushStyle();
        ofSetColor(0, 0, 150);
        ofNoFill();
        ofDrawRectangle(storageOfFilings);
        ofPopStyle();
    }
    //--------------------------------------------------------------
    void drawPlotterInformation(){
        ofPushStyle();
        ofSetColor(150, 0, 0);
        //plotterUp == true ? ofFill() : ofNoFill();
        ofNoFill();
        ofSetRectMode(OF_RECTMODE_CENTER);
        ofSetLineWidth(3);
        ofDrawRectangle(moveToSecond.x, moveToSecond.y, CELL_SIZE, CELL_SIZE);
        ofDrawRectangle(moveToFirst.x, moveToFirst.y, CELL_SIZE, CELL_SIZE);
        
        
        if(isManageStorageMode){
            ofSetColor(0,150,0);
            for(int i=0; i<stockPosition.size(); i++){
                ofDrawCircle(stockPosition[i].x, stockPosition[i].y, STORAGE_OF_FILINGS/2);
            }
            ofSetColor(0,50,0);
            ofDrawCircle(moveInStorage.x, moveInStorage.y, STORAGE_OF_FILINGS/2);
        }
        
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
        
        ofSetRectMode(OF_RECTMODE_CORNER);
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
        gui.draw();
    }
    
    //--------------------------------------------------------------
    void keyPressed(int key){
        
        if(isTrimmingMode){
            ofVec2f p = plotterPosition/ofVec2f(WIDTH_PROCESS, HEIGHT_PROCESS); //0-1
            float plus = 0.001;
            
            if(key == OF_KEY_UP){
                p.y = MAX(p.y - plus, 0);
                osc.send(p);
            }
            if(key == OF_KEY_DOWN){
                p.y = MIN(p.y + plus, 1);
                osc.send(p);
            }
            if(key == OF_KEY_RIGHT){
                p.x = MIN(p.x + plus, 1);
                osc.send(p);
            }
            if(key == OF_KEY_LEFT){
                p.x = MAX(p.x - plus, 0);
                osc.send(p);
            }
            plotterPosition = p*ofVec2f(WIDTH_PROCESS, HEIGHT_PROCESS);
        }
        if(key == 'a') isCalibrationMode = !isCalibrationMode;
        if(key == 'g') gui.setIsDraw();
        /*
        if(key == 't') isTrimmingMode = !isTrimmingMode;
        if(isTrimmingMode){
            if(key == 'c'){
                trimmedPosition.clear();
                trimmedArea.setSize(0,0);
            }
            ofVec2f p = plotterPosition/ofVec2f(WIDTH_PROCESS, HEIGHT_PROCESS); //0-1
            float plus = 0.001;
            
            if(key == OF_KEY_UP){
                p.y = MAX(p.y - plus, 0);
                osc.send(p);
            }
            if(key == OF_KEY_DOWN){
                p.y = MIN(p.y + plus, 1);
                osc.send(p);
            }
            if(key == OF_KEY_RIGHT){
                p.x = MIN(p.x + plus, 1);
                osc.send(p);
            }
            if(key == OF_KEY_LEFT){
                p.x = MAX(p.x - plus, 0);
                osc.send(p);
            }
            plotterPosition = p*ofVec2f(WIDTH_PROCESS, HEIGHT_PROCESS);
            
            if(key == 'o') osc.setRange(0, p.x, 0, p.y);
            if(key == 'm'){
                osc.moveToMax();
                plotterPosition = osc.getRangeMax() * ofVec2f(WIDTH_PROCESS, HEIGHT_PROCESS);
            }
            if(key == 'u') osc.plotterUp();
            if(key == 'd') osc.plotterDown();
        }
        if(key == 'r'){
            osc.reset();
            plotterPosition = ofVec2f(0,0);
        }
        if(key == 'd') isColorDebugMode = !isColorDebugMode;
        if(key == 'a') isCalibrationMode = !isCalibrationMode;
        
        if(key == 's'){
            ofPixels pixels;
            pixels = colorIronFilingsImage.getPixels();
            string name = ofToString(ofGetFrameNum());
            name += ".png";
            ofSaveImage(pixels, name);
        }
        
        */
    }
    //--------------------------------------------------------------
    void mousePressed(int x, int y){
        
        if(isTrimmingMode){
            int mx = x;
            int my = y - HEIGHT_VIEW;
            if(0 <= mx && mx < WIDTH_VIEW && 0 <= my && my < HEIGHT_VIEW){
                if(trimmedPosition.size() < 2){
                    int _x = x;
                    int _y = y-HEIGHT_VIEW;
                    
                    if(trimmedPosition.size() == 1){
                        _x = ((float)WIDTH_PROCESS/HEIGHT_PROCESS)*(_y-trimmedPosition[0].y) + trimmedPosition[0].x;
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
        
        if(isColorDebugMode){
            int mx = x - WIDTH_VIEW;
            int my = y - HEIGHT_VIEW;
            if(0 <= mx && mx < WIDTH_VIEW && 0 <= my && my < HEIGHT_VIEW){
                isManageStorageMode = false;
                STEP = 0;
                callCalculateImageColor(ofVec2f(mx, my));
            }
        }
        if(isCalibrationMode){
            int mx = x - WIDTH_VIEW;
            int my = y - HEIGHT_VIEW;
            if(0 <= mx && mx < WIDTH_VIEW && 0 <= my && my < HEIGHT_VIEW){
                osc.send(1);
                osc.send(ofVec2f(mx, my));
                
            }
        }
    }
    
};
#endif /* ComputerVision_hpp */





