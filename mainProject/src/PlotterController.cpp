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
    simulatePosition = ofVec2f(0.11,0.11);
    direction = 1;
    plot = 0;
    movingTime = 0;
    triggerTime = 0;
    isUpdatePlotterX = true;
    isResizeMode = true;
    mouse = ofVec2f(-1000, -1000);
    isMouseClicked = false;
    
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
    imageFilterShader(peopleFbo.getTexture(), true, 3, true);
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
    //trimming process from sand video
    if(resizePositions.size() == 2 && !isResizeMode){
        trimmedImage.setFromPixels(captureSand.getPixels());
        trimmedImage.setROI(trimArea);
        trimmedImage.drawROI(0, 0, WIDTH, HEIGHT);
    }else{
      captureSand.draw(0, 0);
    }
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
    
    
    /*****************
     cv
     *****************/

    if(!isResizeMode)plotterPositionCalcurator();
    
    
    
    /*****************
     resize sand camera
     *****************/

    //resize sand camera
    if (isResizeMode) ResizeSandCamera();
    
    
    
    isMouseClicked = false;
}
//--------------------------------------------------------------
void PlotterController::keyPressed(int key){
    
    
    if(key == 'r'){
        isResizeMode = !isResizeMode;
        if(resizePositions.size() == 2){ // trim video
            float w = abs(resizePositions[0].x - resizePositions[1].x);
            float h = abs(resizePositions[0].y - resizePositions[1].y);
            trimmedImage.allocate(w, h);

#ifdef DEBUG
            cout << "trimmed image size : ";
            cout << w << " , " << h << endl;
#endif
        }
    }
    if (key == 'c') resizePositions.clear();

}
void PlotterController::mousePressed(int x, int y){
    mouse = ofVec2f(x,y);
    isMouseClicked = true;
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

    /*
    if(position.x > WIDTH || position.x < 0){
        direction *= -1;
        position.y += CELL_SIZE;
    }
    if(position.y > HEIGHT) position.y = 0;
    position.x += CELL_SIZE*direction;
    */
    
    float t = (ofGetElapsedTimef() - triggerTime);
    if( (movingTime - t) <= 0){
        

        float _t = 0;
        if(isUpdatePlotterX){
            if(direction > 0){
                position.x = 0;
            }else{
                position.x = WIDTH;
            }
            direction *= -1;
            _t = WIDTH;
        }else{
            position.y += CELL_SIZE;
            _t = CELL_SIZE;
            if(position.y > HEIGHT){
                position.y = 0;
                _t = HEIGHT;
            }
            
        }
        
        
        isUpdatePlotterX = !isUpdatePlotterX;
        triggerTime = ofGetElapsedTimef();
        movingTime = float(_t/UNIT_DISTANCE);
        
        
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
    
    ofFill();
    for (int x=0; x<(WIDTH/CELL_SIZE); x++) {
        ofDrawLine(x*CELL_SIZE, 0, x*CELL_SIZE, HEIGHT);
        
    }
    
    for (int y=0; y<(HEIGHT/CELL_SIZE); y++) {
        ofDrawLine(0, y*CELL_SIZE, WIDTH, y*CELL_SIZE);
    }

    ofPopMatrix();
    ofPopStyle();
    
#endif
    
}
//--------------------------------------------------------------
void PlotterController::ResizeSandCamera(){
    
    ofPushMatrix();
    ofPushStyle();
    ofTranslate(0, HEIGHT);
    
    
    ofSetColor(0, 0, 0, 100);
    ofDrawRectangle(0, 0, WIDTH, HEIGHT);
    
    ofSetColor(255, 255);
    ofDrawBitmapString("mosue click to resize sand camera", 180, HEIGHT/2);
    ofDrawBitmapString("push 'c' key to clear resize points", 180, HEIGHT/2+20);
    ofDrawBitmapString("push 'r' key to quit this mode", 180, HEIGHT/2+40);
    
    
    
    if(isMouseClicked){
        
        //resize aspect 4:3
        ofVec2f p = ofVec2f(mouse.x, mouse.y-HEIGHT);
        if (resizePositions.size() == 1){
            ofVec2f pp = resizePositions[0];
            float x = abs(p.x - pp.x);
            p.y = (x*3/4) + pp.y;
        }
        if(resizePositions.size() < 2) resizePositions.push_back(p);
        if(resizePositions.size() == 2){
            trimArea.set(resizePositions[0].x, resizePositions[0].y,
                         abs(resizePositions[0].x - resizePositions[1].x),
                         abs(resizePositions[0].y - resizePositions[1].y));
        }
    }
    

    ofSetColor(0, 0, 255, 255);
    for (int i=0; i<resizePositions.size(); i++) {
        ofDrawCircle(resizePositions[i].x, resizePositions[i].y, 5);
    }
    if(resizePositions.size() == 2){
        ofNoFill();
        ofDrawRectangle(trimArea);
        ofFill();
    }

    
    ofPopStyle();
    ofPopMatrix();
}
//--------------------------------------------------------------
ofVec2f PlotterController::getPosition(){
//    if(resizePositions.size() == 2 && !isResizeMode) return (position/ofVec2f(WIDTH,HEIGHT));
//    else return ofVec2f(0,0);
    if(resizePositions.size() == 2 && !isResizeMode){
        return (position/ofVec2f(WIDTH,HEIGHT));
    }else{
        return ofVec2f(-1,-1);
    }
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