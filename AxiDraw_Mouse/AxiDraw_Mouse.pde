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

import processing.video.*;
Capture cam;
float resize_value = 0.1;
PImage peopleCam, drawImage, sandCam;
PVector[] trimmingPoint = new PVector[4];
int vectorIndex = 0;
boolean isMousePressed = false;
PVector _mouse = new PVector(0,0);

CNCServer cnc;
boolean bPlotterIsZeroed;
boolean bFollowingMouse;

float moveX, moveY;
boolean moveUp = false;
float uplevel = 0;
//=======================================

void setup() {
  size(1000, 1000); 
  background(0, 0, 0); 
  fill(255, 255, 255); 
  textSize(15);
  

  setupPlotter();
  setupPeopleCamera();
  setupSandCamera();
}

//=======================================
void draw() {
  
  drawPlotter();
  
  pushMatrix();
  translate(0, 100);
  drawPeopleCamera();
  drawSandCamera();
  popMatrix();
  
  isMousePressed = false;
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
  
  if (key == CODED) {
    if (keyCode == UP) {
      moveY = min(max(0, moveY+1),100);
    }
    if (keyCode == DOWN) {
      moveY = min(max(0, moveY-1),100);
    }
    if (keyCode == RIGHT) {
       moveX = min(max(0, moveX-1),100);
    }
    if (keyCode == LEFT) {
       moveX = min(max(0, moveX+1),100);
    }
  }
  if(key == ' '){
    moveUp = !moveUp;
  }
  if(key == 'h'){
    uplevel = min(max(0, uplevel+0.1), 1);
    println(uplevel);
  }
  if(key == 'y'){
    uplevel = min(max(0, uplevel-0.1), 1);
    println(uplevel);
  }
}
//=======================================
void mousePressed(){
  isMousePressed = true;
  _mouse.x = mouseX;
  _mouse.y = mouseY;
}
//=======================================
void exit() {
  cnc.penUp(0);
  cnc.unlock();
  println("Goodbye!");
  super.exit();
}

void stop() {
  super.exit();
}


/********************************************
  plotter
********************************************/
void setupPlotter(){
  text("Waiting for plotter to connect.", 20, 20); 

  bPlotterIsZeroed = false;
  bFollowingMouse = false;
  cnc = new CNCServer("http://localhost:4242");
  cnc.unlock();
  cnc.penUp(0);
  println("Plotter is at home? Press 'u' to unlock, 'z' to zero, 'd' to draw");
}
void drawPlotter(){
  if (bPlotterIsZeroed) {
    background(255, 255, 255); 
    if (bFollowingMouse) {
      float mx = constrain(mouseX/10.0, 0, 100);
      float my = constrain(mouseY/10.0, 0, 100);
      
      //debug---
      mx = moveX;
      my = moveY;
      
      cnc.moveTo(mx, my);
      
      if(moveUp){
        cnc.penUp(uplevel);
      }else{
        cnc.penDown();
      }
      /*
      if (mousePressed) {
        cnc.penDown();
      } else {
        cnc.penUp();
      }
      */
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

/********************************************
  sand camera
********************************************/

void setupSandCamera() {
  
    String[] cameras = Capture.list();
  
    if (cameras == null) {
      println("Failed to retrieve the list of available cameras, will try the default...");
      cam = new Capture(this, 480, 361);
    } if (cameras.length == 0) {
      println("There are no cameras available for capture.");
      exit();
    } else {
      println("Available cameras:");
      printArray(cameras);
  
      cam = new Capture(this, 480, 361, cameras[15]);
      cam.start();
    }
    
}
  
void drawSandCamera() {
  
  if (cam.available() == true) {
    cam.read();
  }
  
  imageMode(CENTER);  
  pushMatrix();
  translate(peopleCam.width/2, peopleCam.height/2*3);
  
  
  if(vectorIndex < 3){
    if(isMousePressed){
      int mX = mouseX;
      int mY = mouseY - 100 - peopleCam.height;
      trimmingPoint[vectorIndex] = new PVector(mX, mY);
      println(trimmingPoint[vectorIndex]);
      vectorIndex++;      
    }
    image(cam, 0, 0, 480, 361);  
    
  }
  else{
    
    rotate(PI);
    int x = floor(trimmingPoint[0].x);
    int y = floor(trimmingPoint[0].y);
    int w = abs(floor(trimmingPoint[1].x - trimmingPoint[0].x));
    int h = abs(floor(trimmingPoint[2].y - trimmingPoint[1].y));      
    sandCam = cam.get(x,y,w,h);
    sandCam.resize(peopleCam.width, peopleCam.height);
    sandCam.filter(GRAY);
    sandCam.filter(POSTERIZE, 3);
    image(sandCam, 0, 0, sandCam.width, sandCam.height);
    
  }
  
  popMatrix();
  
  pushMatrix();
  translate(peopleCam.width/2*3, peopleCam.height/2*3);
  rotate(PI);
  image(cam, 0, 0, 480, 361);
  popMatrix();
  
  imageMode(CORNER);
}
  
/********************************************
  peopleCamera
********************************************/
void setupPeopleCamera(){
  peopleCam = loadImage("circle.png"); //real-time web camera
  peopleCam.loadPixels();

  drawImage = loadImage("circle.png");
  drawImage.loadPixels();

}
void drawPeopleCamera(){
  
  pushMatrix();
  
 //real image
  image(peopleCam, 0, 0);
  fill(0, 0, 0);
  text("real image", 20, 20);

  //draw image
  PImage img = drawImage.copy();
  int w = floor(img.width*resize_value);
  int h = floor(img.height*resize_value);
  img.filter(GRAY);
  img.filter(POSTERIZE,3);
  
  
  image(img, peopleCam.width, 0);
  fill(0, 0, 0);
  text("draw image", 500, 20);
  
  popMatrix();
}
