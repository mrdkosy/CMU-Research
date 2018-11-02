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
    ofxIntField CELL, STORAGE_OF_FILINGS, COUNTER_LIMIT;
    ofxFloatField UNIT_DISRANCE_PER_SECOND;
    ofxButton point_00, point_W0, point_WH, point_0H, screenShot, clearPosition, setRange;
    ofxToggle trimmingMode, mouseDebugMode, plotterUp;
    
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
        gui.add(COUNTER_LIMIT.setup("COUNTER_LIMIT", 40, 1, 500));
        
        label1.setBackgroundColor(labelColor);
        gui.add(label1.setup("", "plotter calibration"));
        gui.add(point_00.setup("Move to (0, 0)"));
        gui.add(point_W0.setup("Move to (MAX_WIDTH, 0)"));
        gui.add(point_0H.setup("Move to (0, MAX_HEIGHT)"));
        gui.add(point_WH.setup("Move to (MAX_WIDTH, MAX_HEIGHT)"));
        gui.add(plotterUp.setup("magnet up", false));
        gui.add(clearPosition.setup("clear positions"));
        gui.add(setRange.setup("set range"));
        
        label2.setBackgroundColor(labelColor);
        gui.add(label2.setup("", "mode change"));
        gui.add(trimmingMode.setup("trimming mode", true));
        gui.add(mouseDebugMode.setup("mouse debug mode", false));
        
        label3.setBackgroundColor(labelColor);
        gui.add(label3.setup("", ""));
        gui.add(screenShot.setup("screen shot"));
        
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
