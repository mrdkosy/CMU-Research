//
//  PlotterController.cpp
//  main
//
//  Created by Maya Atsuki on 2018/09/28.
//

#include "PlotterController.hpp"

//--------------------------------------------------------------
void PlotterController::init(){
    
    //osc
    isConnectedOSC = osc.setup("128.237.200.67", 12345);
    
    
    shader.load("", "shader.frag");
    position = ofVec2f(0,0);
    prePosition = ofVec2f(0,0);
    direction = ofVec2f(1,1);
    moveToPosition = ofVec2f(0,0);
    drawingMode = 0;
    plotValue = 1;
    movingTime = 0;
    triggerTime = 0;
    isUpdatePlotterX = true;
    isResizeMode = true;
    mouse = ofVec2f(-1000, -1000);
    isMouseClicked = false;
    isGoNextStep = false;
    isBlackToWhite = true;
    
    /*****************
     people
     *****************/
    
#ifdef REALTIME_CAPTURE_PEOPLE
    //default aspect 4:3
    capturePeople.setVerbose(true);
    capturePeople.setDeviceID(0);
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
    captureSand.setDeviceID(1);
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
    imageFilterShader(peopleFbo.getTexture(), true, 0, false, true);
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
    imageFilterShader(sandFbo.getTexture(), true, 2, true, false);
    //imageFilterShader(sandFbo.getTexture(), false, 0, false, true);
    cvSandImageFbo.end();
    cvSandImageFbo.draw(0,0);
    
    ofDrawBitmapStringHighlight("cv image of sand", ofVec2f(10, 20), ofColor(255), ofColor(0));
    ofPopMatrix();
    
    
    /*****************
     cv
     *****************/
    
    if(!isResizeMode){
        plotterPositionCalcurator();
    }
    
    
    
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
    
    if (key == 'o') sendOscMessage(ofVec2f(0,0));
    
}
void PlotterController::mousePressed(int x, int y){
    mouse = ofVec2f(x,y);
    isMouseClicked = true;
}
//--------------------------------------------------------------
void PlotterController::imageFilterShader(ofTexture& tex, bool mono, int posterization, bool less_resolution, bool laplacian){
    
    shader.begin();
    
    shader.setUniform2f("resolution", WIDTH, HEIGHT);
    shader.setUniformTexture("tex'", tex, 0);
    shader.setUniform1i("mono", int(mono));
    shader.setUniform1i("posterization", posterization);
    shader.setUniform1i("less_resolution", int(less_resolution));
    shader.setUniform1i("laplacian", int(laplacian));
    
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
#ifdef SIMULATION_VIEWER
    ofPushStyle();
    ofSetColor(255, 0, 0);
    ofPushMatrix();
    ofTranslate(WIDTH, HEIGHT);
    
    for (int x=0; x<(WIDTH/CELL_SIZE)+1; x++) {
        ofDrawLine(x*CELL_SIZE, 0, x*CELL_SIZE, HEIGHT);
    }
    
    for (int y=0; y<(HEIGHT/CELL_SIZE); y++) {
        ofDrawLine(0, y*CELL_SIZE, WIDTH, y*CELL_SIZE);
    }
    
    ofPopMatrix();
    ofPushMatrix();
    ofTranslate(WIDTH, 0);
    
    for (int x=0; x<(WIDTH/CELL_SIZE)+1; x++) {
        ofDrawLine(x*CELL_SIZE, 0, x*CELL_SIZE, HEIGHT);
    }
    
    for (int y=0; y<(HEIGHT/CELL_SIZE); y++) {
        ofDrawLine(0, y*CELL_SIZE, WIDTH, y*CELL_SIZE);
    }
    
    ofPopMatrix();
    
    ofPopStyle();
#endif
    
    
    //DrawingAlgorithm1();
    DrawingAlgorithm2();
    
    
}
//--------------------------------------------------------------
void PlotterController::plotterValueCalcurator(){
    
    if(drawingMode == 0 || drawingMode == 1){
        ofPixels pixels;
        goalImageFbo.getTexture().readToPixels(pixels);
        
        int blackPoint = 0;
        int threshold = 10;
        int c = CELL_SIZE/2;
        int blackThreshold = 230	;
        
        bool isEnd = false;
        if(drawingMode == 0){
            int skipX = 1;
            int skipY = 1;
            for(int x = (position.x-c); x <= (position.x+c); x+=skipX){
                for(int y = 0; y < HEIGHT; y+= skipY){
                    
                    if(0 <= x && x < WIDTH && 0 <= y && y < HEIGHT){
                        ofColor color = pixels.getColor(x, y);
                        if(color.r < blackThreshold){
                            blackPoint++;
                            if(blackPoint == threshold){
                                isEnd = true;
                                break;
                            }
                        }
                    }
                }
                if(isEnd) break;
            }
        }else if (drawingMode == 1){
            int skipX = 1;
            int skipY = 1;
            for(int x = (position.x-c); x <= (position.x+c); x+=skipX){
                for(int y = (position.y-c); y <= (position.y+c); y+=skipY){
                    
                    if(0 <= x && x < WIDTH && 0 <= y && y < HEIGHT){
                        ofColor color = pixels.getColor(x, y);
                        if(color.r < blackThreshold) blackPoint++;
                        if(blackPoint == threshold){
                            isEnd = true;
                            break;
                        }
                    }
                }
                if (isEnd) break;
            }
        }
        
        if(blackPoint >= threshold) plotValue = 1;
        else plotValue = 0;
        
        
    }
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
    if(resizePositions.size() == 2 && !isResizeMode){
        return (position/ofVec2f(WIDTH,HEIGHT));
    }else{
        return ofVec2f(-1,-1);
    }
}
float PlotterController::getPlotValue(){
    return plotValue;
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

//--------------------------------------------------------------
void PlotterController::DrawingAlgorithm1(){
    
    //calculate next position
    float t = (ofGetElapsedTimef() - triggerTime);
    
    
    
#ifdef SIMULATION_VIEWER
    ofPushStyle();
    ofPushMatrix();
    ofTranslate(WIDTH, HEIGHT);
    if(plotValue == 1) ofNoFill();
    else ofFill();
    ofSetColor(255, 0, 0);
    float x = ofMap(t, 0, movingTime, prePosition.x, position.x);
    float y = ofMap(t, 0, movingTime, prePosition.y, position.y);
    ofDrawCircle(x, y, 10);
    
    for (int x=0; x<(WIDTH/CELL_SIZE)+1; x++) {
        ofDrawLine(x*CELL_SIZE, 0, x*CELL_SIZE, HEIGHT);
    }
    
    for (int y=0; y<(HEIGHT/CELL_SIZE); y++) {
        ofDrawLine(0, y*CELL_SIZE, WIDTH, y*CELL_SIZE);
    }
    
    ofPopMatrix();
    
    ofPushMatrix();
    ofTranslate(WIDTH, 0);
    ofDrawCircle(x, y, 10);
    ofPopMatrix();
    
    ofPopStyle();
    
#endif
    
    
    
    
    if( (movingTime - t) <= 0){
        
        plotterValueCalcurator();
        
        prePosition = position;
        
        float _t = 0;
        if(drawingMode == 0){
            if(isUpdatePlotterX){
                position.x += CELL_SIZE*direction.x;
                if(position.x > WIDTH || position.x < 0){
                    isUpdatePlotterX = false;
                    if(position.x > WIDTH) position.x = WIDTH;
                    else position.x = 0;
                }
                
            }
            else{
                position.y += CELL_SIZE*direction.y;
                if(position.x >= WIDTH) direction.x = -1;
                else direction.x = 1;
                isUpdatePlotterX = true;
                //outside screen
                if(position.y > HEIGHT || position.y < 0){
                    drawingMode = 1;
                    isUpdatePlotterX = false;
                    if(position.y >= HEIGHT)direction.y = -1;
                    else direction.y = 1;
                }
            }
            
        }else if(drawingMode == 1) {
            
            if(isUpdatePlotterX){
                position.x += CELL_SIZE*direction.x;
                if(position.y >= HEIGHT) direction.y = -1;
                else direction.y = 1;
                isUpdatePlotterX = false;
                //outside screen
                if(position.x > WIDTH || position.x < 0){
                    drawingMode = 0;
                    isUpdatePlotterX = false;
                    if(position.x >= WIDTH)direction.x = -1;
                    else direction.x = 1;
                }
            }
            else{
                position.y += CELL_SIZE*direction.y;
                if(position.y > HEIGHT || position.y < 0){
                    isUpdatePlotterX = true;
                    if(position.y > HEIGHT) position.y = HEIGHT;
                    else position.y = 0;
                }
                
            }
            
            
        }
        _t = position.distance(prePosition)/UNIT_DISTANCE_PER_SECOND;
        
        
        triggerTime = ofGetElapsedTimef();
        movingTime = _t;
        plotterValueCalcurator();
        
        
#ifdef DEBUG
        if(movingTime > 0){
            cout << "change goal point to -> ";
            cout << position << endl;
            cout << "the time it takes to move to goal -> ";
            cout << movingTime;
            cout << " seconds" << endl;
        }
#endif
    }
}
//--------------------------------------------------------------
void PlotterController::DrawingAlgorithm2(){
    float t = (ofGetElapsedTimef() - triggerTime);
    
    
    
    
#ifdef SIMULATION_VIEWER
    ofPushStyle();
    ofFill();
    ofSetColor(0, 0, 255);
    
    //isBlack_goalImage ? ofFill() : ofNoFill();
    //plotValue == 1 ? ofNoFill() : ofFill();
    float x = ofMap(t, 0, movingTime, prePosition.x, position.x);
    float y = ofMap(t, 0, movingTime, prePosition.y, position.y);
    ofPushMatrix();
    ofTranslate(WIDTH, 0);
    plotValue == 1 ? ofNoFill() : ofFill();
    ofDrawCircle(x, y, 10);
    
    isBlackToWhite ? ofFill() : ofNoFill();
    ofDrawCircle(position.x, position.y, 7);
    isBlackToWhite ? ofNoFill() : ofFill();
    ofDrawCircle(moveToPosition, 3);
    
    ofPopMatrix();
    
    //isBlack_sandImage ? ofFill() : ofNoFill();
    //plotValue == 1 ? ofNoFill() : ofFill();
    ofPushMatrix();
    ofTranslate(WIDTH, HEIGHT);
    plotValue == 1 ? ofNoFill() : ofFill();
    ofDrawCircle(x, y, 10);
    
    isBlackToWhite ? ofFill() : ofNoFill();
    ofDrawCircle(position.x, position.y, 7);
    isBlackToWhite ? ofNoFill() : ofFill();
    ofDrawCircle(moveToPosition, 3);
    
    ofPopMatrix();
    ofPopStyle();
#endif
    
    
    
    
    bool isBlack_goalImage = false, isBlack_sandImage = false;
    ofVec2f nextPosition;
    
    
    if( (movingTime - t) <= 0){
        ofPixels gPixels, sPixels;
        goalImageFbo.getTexture().readToPixels(gPixels);
        cvSandImageFbo.getTexture().readToPixels(sPixels);
        
        
        if(isGoNextStep){
            prePosition = position;
            position = moveToPosition;
            triggerTime = ofGetElapsedTimef();
            movingTime = position.distance(prePosition)/UNIT_DISTANCE_PER_SECOND;
            plotValue = 0;
            sendOscMessage(plotValue);
            sendOscMessage(position);
            
            isGoNextStep = false;
            
        }else{
            int i = 0;
            while(i < 100){ //loop until find bad cell(max is 100)
                
                int nx = int(ofRandom(WIDTH/CELL_SIZE + 1)) * CELL_SIZE;
                int ny = int(ofRandom(HEIGHT/CELL_SIZE + 1)) * CELL_SIZE;
                nextPosition = ofVec2f(nx, ny);
                
                
                int c = CELL_SIZE/2;
                int gColor = 0, sColor = 0;
                for(int y=-c; y<c; y++){
                    for(int x = -c; x<c; x++){
                        int _x = nextPosition.x + x;
                        int _y = nextPosition.y + y;
                        if(0 <= _x && _x < WIDTH && 0<= _y && _y < HEIGHT){
                            int _gColor = gPixels.getColor(_x, _y).r;
                            int _sColor = sPixels.getColor(_x, _y).r;
                            if( _gColor == 0 ) gColor++;
                            if( _sColor == 0 ) sColor++;
                        }
                    }
                }
                
                //a certain threshold
                if(gColor > 0) isBlack_goalImage = true; //!!!!!!!!!!
                if(sColor > (CELL_SIZE*CELL_SIZE/3)) isBlack_sandImage = true; //!!!!!!!!!!
                
                
                float _t = 0;
                //the cell of sand picture is not good
                if(isBlack_goalImage != isBlack_sandImage){
                    isGoNextStep = true;
                    break;
                }
                i++;
            }
            
            
            if(isGoNextStep){ //analyze the each color of up, down, right and left cell
                
                int loop=1;
                while (loop<50) {
                    
                    
                    ofVec2f aroundPixels[8] = {
                        ofVec2f(0, - CELL_SIZE*loop), //up
                        ofVec2f(CELL_SIZE*loop, - CELL_SIZE*loop), //right up
                        ofVec2f(CELL_SIZE*loop, 0), //right
                        ofVec2f(CELL_SIZE*loop, CELL_SIZE*loop), //right down
                        ofVec2f(0, CELL_SIZE*loop), //down
                        ofVec2f(-CELL_SIZE*loop, CELL_SIZE*loop), //down left
                        ofVec2f(-CELL_SIZE*loop, 0), //left
                        ofVec2f(-CELL_SIZE*loop, - CELL_SIZE*loop) //left up
                    };
                    int aroundGoalColor[8] = {0, 0, 0, 0, 0, 0, 0, 0};
                    int aroundSandColor[8] = {0, 0, 0, 0, 0, 0, 0, 0};
                    
                    //cout << "---------" << endl;
                    int c = CELL_SIZE/2;
                    for(int i=0; i<8; i++){
                        for(int y=-c; y<c; y++){
                            for(int x=-c; x<c; x++){
                                
                                int _x = nextPosition.x + aroundPixels[i].x + x;
                                int _y = nextPosition.y + aroundPixels[i].y+y;
                                if(0 <= _x && _x < WIDTH && 0 <= _y && _y < HEIGHT){
                                    int _gColor = gPixels.getColor(_x, _y).r;
                                    int _sColor = sPixels.getColor(_x, _y).r;
                                    
                                    int threshold = 0;
                                    if(_gColor <= threshold) aroundGoalColor[i]++;
                                    if(_sColor <= threshold) aroundSandColor[i]++;
                                }
                            }
                            
                        }
                        //cout << "loop :" << loop << ", goal: " << aroundGoalColor[i] << " , real: " << aroundSandColor[i] << " , diff: " << aroundGoalColor[i] - aroundSandColor[i]<< endl;
                    }
                    
                    
                    //choose the different cell from goal image
                    isBlack_goalImage ? isBlackToWhite = false : isBlackToWhite = true;
                    
                    
                    int maxDifferencesIndex = 0;
                    for(int i=1; i<8	; i++){
                        int diff = aroundGoalColor[i] - aroundSandColor[i];
                        int maxDiff = aroundGoalColor[maxDifferencesIndex] - aroundSandColor[maxDifferencesIndex];
                        
                        if( isBlackToWhite ){ //look for the white cell wants iron filings around nextPoint
                            if( maxDiff < diff ) //most hungry cell
                                maxDifferencesIndex = i;
                        }else{ // lokk for the black cell wants to remove iron filings around nextPoint
                            if(diff < maxDiff) maxDifferencesIndex = i;
                        }
                    }
                    
                    
                    int maxDiff = aroundGoalColor[maxDifferencesIndex] - aroundSandColor[maxDifferencesIndex];
                    //cout << "maxDiff : " << maxDiff << endl; //!!!!!!!!!!
                    
                    
                    if(isBlackToWhite){
                        if(maxDiff > 0){
                            prePosition = position;
                            position = nextPosition;
                            moveToPosition = nextPosition + aroundPixels[maxDifferencesIndex];
                            break;
                        }
                    }else{
                        if(maxDiff < 0){
                            prePosition = position;
                            position = nextPosition + aroundPixels[maxDifferencesIndex];
                            moveToPosition = nextPosition;
                            break;
                        }
                    }
                    
                    loop++;
                } //end while
                
                
                
                triggerTime = ofGetElapsedTimef();
                movingTime = position.distance(prePosition)/UNIT_DISTANCE_PER_SECOND;
                
                plotValue = 1;
                sendOscMessage(plotValue);
                sendOscMessage(position);
                
            }
            
        }
        
        
        
    }
    
    
}
//--------------------------------------------------------------
void PlotterController::sendOscMessage(ofVec2f p){
    if(isConnectedOSC){
        ofxOscMessage msg;
        msg.setAddress("/plotter/position/");
        msg.addFloatArg(p.x/WIDTH);
        msg.addFloatArg(p.y/HEIGHT);
        osc.sendMessage(msg);
#ifdef DEBUG
        cout << "OSC send : ";
        cout << p << endl;
#endif
    }
}
void PlotterController::sendOscMessage(float value){
    if(isConnectedOSC){
        ofxOscMessage msg;
        msg.setAddress("/plotter/plotvalue/");
        msg.addFloatArg(value);
        osc.sendMessage(msg);
#ifdef DEBUG
        cout << "OSC send : ";
        cout << value << endl;
#endif
        ofSleepMillis(100);
    }
}








