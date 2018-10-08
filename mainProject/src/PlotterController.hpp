//
//  PlotterController.hpp
//  main
//
//  Created by Maya Atsuki on 2018/09/28.
//

#ifndef PlotterController_hpp
#define PlotterController_hpp

#include <stdio.h>
#include "ofMain.h"
#include "ofxOscSender.h"
#include "SandSimulation.hpp"
#include "ofxOpenCv.h"

#define DEBUG
//#define REALTIME_CAPTURE_PEOPLE
#define REALTIME_CAPTURE_SAND
#define WIDTH 640
#define HEIGHT 480
#define CELL_SIZE 20 //2,4,8,10,16,20,32,40,50
#define UNIT_DISTANCE 82 //when the size of cell is, how long the plotter move for a second
#define UNIT_DISTANCE_PER_SECOND (WIDTH/10)
#define SIMULATION_VIEWER


#endif /* PlotterController_hpp */

class PlotterController{
private:
    void imageFilterShader(ofTexture& tex, bool mono, int posterization, bool less_resolution, bool laplacian);
    
    void plotterPositionCalcurator();
    void plotterValueCalcurator();
    void DrawingAlgorithm1();
    void DrawingAlgorithm2();
    ofVec2f position, prePosition, direction, moveToPosition; // control left right front back
    float plotValue, movingTime; //control up or down, the time while plotter moving to other place
    // plotValue: 1->down, 0->up
    float triggerTime; //the time that plotter start to move other place
    int drawingMode;
    bool isUpdatePlotterX, isGoNextStep, isBlackToWhite;
    ofVec2f mouse;
    bool isMouseClicked;
    
    ofVideoGrabber capturePeople, captureSand;
    ofImage imagePeople;
    ofShader shader;
    ofFbo peopleFbo, sandFbo, goalImageFbo, cvSandImageFbo;

    void ResizeSandCamera();
    bool isResizeMode;
    vector<ofVec2f> resizePositions;
    ofRectangle trimArea;
    ofxCvColorImage trimmedImage;
    
    void sandSimulation();
    void sandSimulationInit();
    const int sandNum = 2000;
    vector<SandSimulation> sand;
    
    
    
    void sendOscMessage(ofVec2f p);
    void sendOscMessage(float value);
    
    ofxOscSender osc;
    bool isConnectedOSC;

    
public:
    void init();
    void update();
    void draw();
    void keyPressed(int key);
    void mousePressed(int x, int y);
    ofVec2f getPosition();
    float getPlotValue();
    
};
