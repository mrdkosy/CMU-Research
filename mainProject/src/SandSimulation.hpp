//
//  SandSimulation.hpp
//  mainProject
//
//  Created by STUDIO on 9/30/18.
//
//

#ifndef SandSimulation_hpp
#define SandSimulation_hpp

#include <stdio.h>
#include "ofMain.h"

#endif /* SandSimulation_hpp */

class SandSimulation{
private:
    ofVec2f position;
    
    
public:
    SandSimulation(ofVec2f range){
        position = ofVec2f( ofRandom(0, range.x), ofRandom(0, range.y));
    }
    
    void update(){
        
    }
    
    void draw(){
        
        update();
        
        ofSetColor(0);
        ofDrawCircle(position.x, position.y, 1);
    }
};