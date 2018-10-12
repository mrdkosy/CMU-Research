// Real-time AxiDraw Mouse-Following. 
// by Golan Levin, September 2018. 
//
// Known to work with Processing v3.4 on OSX 10.13.5, 
// Using Node.js v10.10.0, npm v6.4.1.
// Based on AxiDraw_Simple by Aaron Koblin
// https://github.com/koblin/AxiDrawProcessing
// Uses CNCServer by @techninja
// https://github.com/techninja/cncserver
//
// Instructions: in a terminal, type 
// node cncserver --botType=axidraw
// Then run this program. 

import oscP5.*;
import netP5.*;
OscP5 oscP5;


CNCServer cnc;
boolean bPlotterIsZeroed;
boolean bFollowingMouse;

float plotterX = 0, plotterY = 0;
float plotterValue = 0;
float mousePlotterValue = 0;

boolean isOscControl = false;

void setup() {
  size(1000, 1000); 
  background(0, 0, 0); 
  fill(255, 255, 255); 
  text("Waiting for plotter to connect.", 20, 20); 

  bPlotterIsZeroed = false;
  bFollowingMouse = false;
  cnc = new CNCServer("http://localhost:4242");
  cnc.unlock();
  cnc.penUp(0);
  println("Plotter is at home? Press 'u' to unlock, 'z' to zero, 'd' to draw");
  
  //osc
  oscP5 = new OscP5(this,12345);
}

//=======================================
void draw() {

  if (bPlotterIsZeroed) {
    background(255, 255, 255); 
    if (bFollowingMouse) {
      
      
      
      float mx, my;
      if(isOscControl){
        float val = constrain(plotterValue, 0, 1);
        cnc.penUp(val);
        mx = constrain(plotterX*100, 0, 100);
        my = constrain(plotterY*100, 0, 100);
        text("Plotter is controlled by OSC", 20, 50);
      }else{
        if(mousePressed)cnc.penUp(0);//cnc.penUp(mousePlotterValue);
        else cnc.penUp(1);
        
        mx = constrain(mouseX/10.0, 0, 100);
        my = constrain(mouseY/10.0, 0, 100);
        text("Plotter is controlled by YOUR MOUSE", 20, 50);
      }
      
      cnc.moveTo(mx, my);
      
    } else {
      fill(0, 0, 0);
      text ("Enable drawing to move plotter.", 20, 20);
      text ("Toggle 'd' to enable drawing.", 20, 35);
    }
  } else {
    background(255, 0, 0); 
    fill(255, 255, 255); 
    text("Must zero plotter before use!", 20, 20);
    text("Move plotter to home position, press 'z'.", 20, 35);
  }
}


//=======================================
void keyPressed() {

  if (key == 'u') {
    cnc.unlock();
    println("Pen unlocked ..... remember to zero!");
  }

  if (key == 'z') {
    cnc.zero();
    bPlotterIsZeroed = true; 
    println("Pen zero'd");
  }

  if (key == 'd') {
    bFollowingMouse = !bFollowingMouse;
    println("bFollowingMouse = " + bFollowingMouse);
  }
  
  if(key == 'o'){
     isOscControl = !isOscControl; 
  }
  
  if(!isOscControl){
    if(key == 'q'){ //down
      mousePlotterValue += 0.01;
      mousePlotterValue = constrain(mousePlotterValue, 0, 1);
      println(mousePlotterValue);
    }
    if(key == 'w'){ //up
      mousePlotterValue -= 0.01;
      mousePlotterValue = constrain(mousePlotterValue, 0, 1);
      println(mousePlotterValue);
      
    }
  }
  
}

//=======================================
void exit() {
  cnc.penUp(0);
  cnc.unlock();
  cnc.moveTo(0, 0);
  println("Goodbye!");
  super.exit();
}

void stop() {
  super.exit();
}
//=======================================
void oscEvent(OscMessage msg) {
  if(isOscControl){
    if(msg.checkAddrPattern("/plotter/position/")==true) {
      plotterX = msg.get(0).floatValue();
      plotterY = msg.get(1).floatValue();
      println("get OSC message : " + plotterX + ", " + plotterY); 
    }
    
    if(msg.checkAddrPattern("/plotter/plotvalue/")==true) {
      plotterValue = msg.get(0).floatValue();
      println("get OSC message : " + plotterValue);
    }    
  }
}
