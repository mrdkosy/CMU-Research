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

//#define DEBUG

#define IP "128.237.170.91"
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
    
    void send(ofVec2f position){
        if(isConnected){
            ofxOscMessage m;
            m.setAddress("/plotter/position/");
            m.addFloatArg(position.x);
            m.addFloatArg(position.y);
            osc.sendMessage(m);
#ifdef DEBUG
            cout << "osc send : " << position << endl;
#endif
        }
        
    }
    void send(float value){
        if(isConnected){
            ofxOscMessage m;
            m.setAddress("/plotter/plotvalue/");
            m.addFloatArg(1-value);
            osc.sendMessage(m);
            ofSleepMillis(500);
#ifdef DEBUG
            cout << "osc send : " << value << endl;
#endif
        }
    }
    
    void reset(){
        if(isConnected){
            send(0);
            send(ofVec2f(0,0));
        }
    }
    
    
};
#endif /* OscController_hpp */
