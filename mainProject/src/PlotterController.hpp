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

#endif /* PlotterController_hpp */

class PlotterController{
private:
    void imageFilterShader();
    ofVec2f position;
    ofVideoGrabber capturePeople, captureSand;
    ofImage imagePeople;
    ofShader shader;
    
public:
    void init();
    void update();
    void draw();
    ofVec2f getPosition();
    
};
