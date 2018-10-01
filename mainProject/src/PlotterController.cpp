//
//  PlotterController.cpp
//  main
//
//  Created by Maya Atsuki on 2018/09/28.
//
    
#include "PlotterController.hpp"

//--------------------------------------------------------------
void PlotterController::init(){
    
    shader.load("", "shader.frag");
    position = ofVec2f(0,0);
    simulatePosition = ofVec2f(0,0);
    direction = 1;
    plot = 0;
    movingTime = 0;
    triggerTime = 0;
    
    /*****************
     people
     *****************/
    
#ifdef REALTIME_CAPTURE_PEOPLE
    //default aspect 4:3
    capturePeople.setVerbose(true);
    capturePeople.setDeviceID(1);
    capturePeople.initGrabber(WIDTH, HEIGHT);
#else
    imagePeople.load("circle.png");
#endif
    peopleFbo.allocate(WIDTH, HEIGHT);
    goalImageFbo.allocate(WIDTH, HEIGHT);
    
    
#ifdef DEBUG
    capturePeople.listDevices();
#endif

    
    /*****************
     sand
     *****************/

#ifdef REALTIME_CAPTURE_SAND
    //default aspect 4:3
    captureSand.setVerbose(true);
    captureSand.setDeviceID(0);
    captureSand.initGrabber(WIDTH, HEIGHT);
#else
    sandSimulationInit();
#endif
    sandFbo.allocate(WIDTH, HEIGHT);
    cvSandImageFbo.allocate(WIDTH, HEIGHT);
    
    

}
//--------------------------------------------------------------
void PlotterController::update(){
    
#ifdef REALTIME_CAPTURE_PEOPLE
    capturePeople.update();
#endif
    
    captureSand.update();
    
}

//--------------------------------------------------------------
void PlotterController::draw(){
    
    ofSetColor(255);
    
    /*****************
     people
     *****************/
    peopleFbo.begin();
    ofClear(255);
#ifdef REALTIME_CAPTURE_PEOPLE
    capturePeople.draw(0,0);
#else
    imagePeople.draw(0,0, WIDTH, HEIGHT);
#endif
    peopleFbo.end();
    peopleFbo.draw(0,0);
    ofDrawBitmapStringHighlight("camera for people", ofVec2f(10, 20), ofColor(255), ofColor(0));
    
    //goal image
    ofPushMatrix();
    ofTranslate(WIDTH, 0);
    
    goalImageFbo.begin();
    ofClear(255);
    imageFilterShader(peopleFbo.getTexture(), true, 4, true);
    goalImageFbo.end();
    goalImageFbo.draw(0,0);
    
    ofDrawBitmapStringHighlight("mono image of people", ofVec2f(10, 20), ofColor(255), ofColor(0));
    ofPopMatrix();
    
    
    
    /*****************
     sand
     *****************/
    sandFbo.begin();
    ofClear(255);
#ifdef REALTIME_CAPTURE_SAND
    captureSand.draw(0, 0);
#else
    sandSimulation();
#endif
    sandFbo.end();
    
    ofPushMatrix();
    ofTranslate(0, HEIGHT);
    sandFbo.draw(0,0);
    ofDrawBitmapStringHighlight("camera for sand", ofVec2f(10, 20), ofColor(255), ofColor(0));
    ofPopMatrix();
    
    //cv sand image
    ofPushMatrix();
    ofTranslate(WIDTH, HEIGHT);
    
    cvSandImageFbo.begin();
    ofClear(255);
    imageFilterShader(sandFbo.getTexture(), true, 3, true);
    cvSandImageFbo.end();
    cvSandImageFbo.draw(0,0);
    
    ofDrawBitmapStringHighlight("cv image of sand", ofVec2f(10, 20), ofColor(255), ofColor(0));
    ofPopMatrix();
    
    
    plotterPositionCalcurator();
    
}
//--------------------------------------------------------------
void PlotterController::imageFilterShader(ofTexture& tex, bool mono, int posterization, bool less_resolution){
    
    shader.begin();
    float coef[] = {
        1.0,  1.0,  1.0,
        1.0, -8.0,  1.0,
        1.0,  1.0,  1.0
    };
    
    shader.setUniform2f("resolution", WIDTH, HEIGHT);
    shader.setUniformTexture("tex'", tex, 0);
    shader.setUniform1i("mono", int(mono));
    shader.setUniform1i("posterization", posterization);
    shader.setUniform1i("less_resolution", int(less_resolution));

    ofSetColor(255,255);
    glBegin(GL_TRIANGLE_STRIP);
    glTexCoord2d(0, 0);
    glVertex2d(0, 0);
    
    glTexCoord2d(WIDTH, 0);
    glVertex2d(WIDTH, 0);
    
    glTexCoord2d(0, HEIGHT);
    glVertex2d(0, HEIGHT);
    
    glTexCoord2d(WIDTH, HEIGHT);
    glVertex2d(WIDTH, HEIGHT);
    glEnd();
    
    shader.end();
}
//--------------------------------------------------------------
void PlotterController::plotterPositionCalcurator(){

    
    float t = (ofGetElapsedTimef() - triggerTime);
    if( (movingTime - t) <= 0){
        
        if(direction > 0){
            position.x = 0;
        }else{
            position.x = WIDTH;
        }
        direction *= -1;
        position.y += CELL_SIZE;
        if(position.y > HEIGHT) position.y = 0;
        
        triggerTime = ofGetElapsedTimef();
        movingTime = float(WIDTH/DISTANCE_PER_SECOND);
        
        
#ifdef DEBUG
        cout << "change goal point to -> ";
        cout << position << endl;
        cout << "the time it takes to move to goal -> ";
        cout << movingTime;
        cout << " seconds" << endl;
#endif
    }


    
#ifdef DEBUG
    ofPushStyle();
    ofPushMatrix();
    ofTranslate(WIDTH, HEIGHT);
    ofNoFill();
    ofSetColor(255, 0, 0);
    ofDrawCircle(position, 10);
    ofPopMatrix();
    ofPopStyle();
#endif
    
}
//--------------------------------------------------------------
ofVec2f PlotterController::getPosition(){
    return (position/ofVec2f(WIDTH,HEIGHT));
}
//--------------------------------------------------------------
void PlotterController::sandSimulationInit(){
    sand.clear();
    for (int i=0; i<sandNum; i++) {
        SandSimulation s = SandSimulation(ofVec2f(WIDTH, HEIGHT));
        sand.push_back(s);
    }
}

void PlotterController::sandSimulation(){

    //draw
    for(int i=0; i<sand.size(); i++){
        sand[i].draw();
    }
}