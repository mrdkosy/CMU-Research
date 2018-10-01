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
#include "SandSimulation.hpp"
#include "ofxOpenCv.h"

#define DEBUG
//#define REALTIME_CAPTURE_PEOPLE
#define REALTIME_CAPTURE_SAND
#define WIDTH 640
#define HEIGHT 480
#define CELL_SIZE 30
#define UNIT_DISTANCE 82



#endif /* PlotterController_hpp */

class PlotterController{
private:
    void imageFilterShader(ofTexture& tex, bool mono, int posterization, bool less_resolution);
    
    void plotterPositionCalcurator();
    ofVec2f position, simulatePosition; // control left right front back
    float plot, movingTime; //control up or down, the time while plotter moving to other place
    float triggerTime; //the time that plotter start to move other place
    int direction;
    bool isUpdatePlotterX;
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
    
public:
    void init();
    void update();
    void draw();
    void keyPressed(int key);
    void mousePressed(int x, int y);
    ofVec2f getPosition();
    
};
