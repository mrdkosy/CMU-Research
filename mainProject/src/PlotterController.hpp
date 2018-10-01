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


#define DEBUG
//#define REALTIME_CAPTURE_PEOPLE
//#define REALTIME_CAPTURE_SAND
#define WIDTH 640
#define HEIGHT 480
#define CELL_SIZE 10
#define DISTANCE_PER_SECOND 100


#endif /* PlotterController_hpp */

class PlotterController{
private:
    void imageFilterShader(ofTexture& tex, bool mono, int posterization, bool less_resolution);
    
    void plotterPositionCalcurator();
    ofVec2f position, simulatePosition; // control left right front back
    float plot, movingTime; //control up or down, the time while plotter moving to other place
    float triggerTime; //the time that plotter start to move other place
    int direction;
    
    ofVideoGrabber capturePeople, captureSand;
    ofImage imagePeople;
    ofShader shader;
    ofFbo peopleFbo, sandFbo, goalImageFbo, cvSandImageFbo;
    
    void sandSimulation();
    void sandSimulationInit();
    const int sandNum = 2000;
    vector<SandSimulation> sand;
    
public:
    void init();
    void update();
    void draw();
    ofVec2f getPosition();
    
};
