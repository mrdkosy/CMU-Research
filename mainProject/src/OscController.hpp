//
//  OscController.hpp
//  mainProject
//
//  Created by Maya Atsuki on 10/12/18.
//
//

#ifndef OscController_hpp
#define OscController_hpp

#include <stdio.h>
#include "ofMain.h"
#include "ofxOscSender.h"

#define IP "128.237.198.71"
#define PORT 12345

class OscController
{
    
private:
    ofxOscSender osc;
    bool isConnected = false;
    
public:
    OscController(){
        isConnected = osc.setup(IP, PORT);
    }
    
    void OscSend(ofVec2f position){
        if(isConnected){
            ofxOscMessage m;
            m.setAddress("/plotter/position/");
            m.addFloatArg(position.x);
            m.addFloatArg(position.y);
            osc.sendMessage(m);
        }
        
    }
    void OscSend(float value){
        if(isConnected){
            ofxOscMessage m;
            m.setAddress("/plotter/plotvalue/");
            m.addFloatArg(value);
            osc.sendMessage(m);
        }
    }
    
    
};
#endif /* OscController_hpp */
