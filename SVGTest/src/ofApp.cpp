#include "ofApp.h"
ofxPDF pdf;
vector<ofPolyline> polys;
ofImage image;

ofxSVG svg;
//--------------------------------------------------------------
void ofApp::setup(){
    
    image.load("triangles23.png");
    pdf.load("triangles24.pdf");
    for(int i=0; i<pdf.getNumPath(); i++){
        ofPath& p = pdf.getPathAt(i);
        //path.append(p);
        const vector<ofPolyline> &tmpPoly = p.getOutline();
        for(int k=0; k<tmpPoly.size(); k++){
            polys.push_back(tmpPoly[k]);
            
        }
    }
    
    svg.load("triangles22.svg");
}

//--------------------------------------------------------------
void ofApp::update(){

}

//--------------------------------------------------------------
void ofApp::draw(){
    
    
    ofSetColor(255);
    /*
    image.draw(0, 0, 640, 480);
    
    int count = 0;
    for(int k=0; k<polys.size(); k++){
        auto verts = polys[k].getVertices();
        for(int j=0; j<verts.size(); j++){
            float x = verts[j].x;
            float y = verts[j].y;
            x *= 640/pdf.getWidth();
            y *= 480/pdf.getHeight();
            //ofSetColor(0);
            //ofDrawCircle(x, y, 10);
            
            ofSetColor(255, 0, 0);
            //ofDrawBitmapString(ofToString(count), x, y);
            
            ofDrawCircle(x, y, 10);
            count++;
        }
        //ofSetColor(0);
        //polys[k].draw();
        
    }
     */
    svg.draw();

}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){

}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}
