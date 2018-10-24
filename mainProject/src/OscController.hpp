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

#define IP "128.237.121.129"
#define PORT 12345

class OscController
{
    
private:
    ofxOscSender osc;
    bool isConnected = false;
    ofVec2f min = ofVec2f(0, 0);
    ofVec2f max = ofVec2f(1, 1);
    
public:
    OscController(){
        isConnected = osc.setup(IP, PORT);
    }
    
    void send(ofVec2f position){
        if(isConnected){
            ofVec2f p;
            p.x = position.x*(max.x - min.x) + min.x;
            p.y = position.y*(max.y - min.y) + min.y;
            
            ofxOscMessage m;
            m.setAddress("/plotter/position/");
            m.addFloatArg(p.x);
            m.addFloatArg(p.y);
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
    
    void setRange(float minX, float maxX, float minY, float maxY){
        min = ofVec2f(minX, minY);
        max = ofVec2f(maxX, maxY);
        cout << "set the range of moving plotter : x[ ";
        cout << min.x << ", " << max.x << " ], y[ ";
        cout << min.y << ", " << max.y << " ]" << endl;;
    }
    
    void moveToMax(){
        plotterDown();
        send(max);
    }
    
    ofVec2f getRangeMin(){
        return min;
    }
    
    ofVec2f getRangeMax(){
        return max;
    }
    
    void plotterUp(){
        send(1);
    }
    
    void plotterDown(){
        send(0);
    }
    
    
};
#endif /* OscController_hpp */
