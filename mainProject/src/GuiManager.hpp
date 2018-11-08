//
//  GuiManager.hpp
//  mainProject
//
//  Created by Maya Atsuki on 11/1/18.
//
//

#ifndef GuiManager_hpp
#define GuiManager_hpp

#include <stdio.h>
#include "ofMain.h"
#include "ofxGui.h"

class GuiManager{
private:
    bool isDraw;
    ofxPanel gui;
    ofxLabel label0, label1, label2, label3;
    ofColor labelColor;
    int isStorageChanged;
    
    void storageChanged(int &STORAGE_OF_FILINGS){
        isStorageChanged = true;
    }


public:
    ofxIntField CELL, STORAGE_OF_FILINGS, COUNTER_LIMIT, howRandomPoint, monoThreshold, BlackOrWhiteThreshold, loopStorage;
    ofxFloatField UNIT_DISRANCE_PER_SECOND, test;
    ofxButton point_00, point_W0, point_WH, point_0H, oscReset, screenShot, clearPosition, setRange,plotterUp, plotterDown;
    ofxToggle trimmingMode, mouseDebugMode, searchColorMode, viewGrid;
    ofxIntField convertMin, convertMax;
    
    GuiManager(){
        isDraw = true;
        labelColor = ofColor(0,0,110);
        isStorageChanged = false;
        
        STORAGE_OF_FILINGS.addListener(this, &GuiManager::storageChanged);
        
        
        gui.setup();
        gui.setSize(300, ofGetHeight());
        //gui.setWidthElements(300);
        gui.setDefaultWidth(gui.getWidth());
        
        label0.setBackgroundColor(labelColor);
        gui.add(label0.setup("","parameter settings"));
        gui.add(CELL.setup("CELL", 30, 2, 100));
        gui.add(STORAGE_OF_FILINGS.setup("STORAGE_OF_FILINGS", 40, 0, 100));
        gui.add(UNIT_DISRANCE_PER_SECOND.setup("UNIT_DISRANCE_PER_SECOND", 8.5, 1, 20));
        gui.add(COUNTER_LIMIT.setup("COUNTER_LIMIT", 80, 1, 500));
        gui.add(howRandomPoint.setup("how random points", 80, 10, 300));
        gui.add(monoThreshold.setup("cv mono threshold", 125, 0, 255));
        gui.add(BlackOrWhiteThreshold.setup("black or white threshold", 120, 0, 255));
        gui.add(loopStorage.setup("loop storage", 5, 1, 30));
        
        label1.setBackgroundColor(labelColor);
        gui.add(label1.setup("", "plotter calibration"));
        gui.add(oscReset.setup("reset plotter"));
        gui.add(point_00.setup("Move to (0, 0)"));
        gui.add(point_W0.setup("Move to (MAX_WIDTH, 0)"));
        gui.add(point_0H.setup("Move to (0, MAX_HEIGHT)"));
        gui.add(point_WH.setup("Move to (MAX_WIDTH, MAX_HEIGHT)"));
        gui.add(plotterUp.setup("magnet up"));
        gui.add(plotterDown.setup("magnet down"));
        gui.add(clearPosition.setup("clear positions"));
        gui.add(setRange.setup("set range"));
        
        label2.setBackgroundColor(labelColor);
        gui.add(label2.setup("", "mode change"));
        gui.add(trimmingMode.setup("trimming mode", true));
        gui.add(mouseDebugMode.setup("mouse debug mode", false));
        gui.add(searchColorMode.setup("search color mode", false));
        
        label3.setBackgroundColor(labelColor);
        gui.add(label3.setup("", ""));
        gui.add(screenShot.setup("screen shot"));
        gui.add(test.setup("test float parameter", 1, 0, 1));
        gui.add(convertMin.setup("color convert min", 40, 0, 255));
        gui.add(convertMax.setup("color convert max", 255, 0, 255));
        gui.add(viewGrid.setup("show grid", false));
        
    }
    void setIsDraw(){
        isDraw = !isDraw;
    }
    void draw(){
        if(isDraw) gui.draw();
        isStorageChanged = false;
    }
    bool getIsStorageChanged(){
        return isStorageChanged;
    }
    int getStorageValue(){
        return STORAGE_OF_FILINGS;
    }
    
};

#endif /* GuiManager_hpp */
