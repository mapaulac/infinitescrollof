#include "ofApp.h"

const int width = 1080;
const int height = 745;
int heightImg1 = 8821;
int heightImg2 = 9680;
int heightImg3 = 9500;

//--------------------------------------------------------------
void ofApp::setup(){
    ofBackground(0,0,0);
	ofSetWindowTitle("oscReceiveExample");
	ofSetFrameRate(60); // run at 60 fps
	ofSetVerticalSync(true);

	// listen on the given port
	ofLog() << "listening for osc messages on port " << PORT;
	receiver.setup(PORT);
    
    //FBO
    feedImg.load("samplefinal.jpg"); //load feed image
//    feedImg2.load("scrolltwitter33.jpg"); //load feed image
//    feedImg3.load("scrolltwitter333.jpg"); //load feed image
    
    //allocating fbos
    rgbaFbo.allocate(1080, 16000, GL_RGB); ///CHANGE: ACCORDING TO HEIGHT OF IMAGE

    
    // we have to clear all the fbos so that we don't see any artefacts
    // the clearing color does not matter here, as the alpha value is 0, that means the fbo is cleared from all colors
    // whenever we want to draw/update something inside the fbo, we have to write that inbetween fbo.begin() and fbo.end()
    rgbaFbo.begin();
    ofClear(255,255,255, 0);
    rgbaFbo.end();
    
    rgbaFboFloat.begin();
    ofClear(255,255,255, 0);
    rgbaFboFloat.end();
    
    //SYPHON
    counter = 0;
    ofSetCircleResolution(50);
    bSmooth = false;
    ofSetWindowTitle("Endless Scroll");
    individualTextureSyphonServer.setName("Texture Output");
}

//--------------------------------------------------------------
void ofApp::update(){
    //SYPHON
    counter = counter + 0.033f;
//    cout << heightValue <<endl;
    
    //FBO
    ofEnableAlphaBlending();

    rgbaFbo.begin();
    ofBackground(0,0,0);
    int newHeightValue = ofMap(heightValue, 0,125072,0,28000);
    feedImg.draw(0,-newHeightValue + 16000);
    //feedImg.draw(0,-heightValue + heightImg1); //drawing feedImg inside FBO IMPORTANT! CHANGE: ACCORDING TO HEIGHT OF JPG
//    feedImg2.draw(0,-heightValue + heightImg1*2);
//    feedImg3.draw(0,-heightValue + (heightImg1*2 + heightImg2));
    //KEEP DRAWING ADDITIONAL IMAGES HERE! 8821*3, ETC. CROP THESE IMAGES FROM THE LARGE MOTHER IMAGE
    rgbaFbo.end();

    //OSC
	// hide old messages
	for(int i = 0; i < NUM_MSG_STRINGS; i++){
		if(timers[i] < ofGetElapsedTimef()){
			msgStrings[i] = "";
		}
	}

	// check for waiting messages
	while(receiver.hasWaitingMessages()){

		// get the next message
		ofxOscMessage m;
		receiver.getNextMessage(m);
        
        //check for height scrolled message
        if (m.getAddress() == "/height"){
            heightValue = m.getArgAsInt32(0); //saving received scroll height in heightValue
        }
		else{

			// unrecognized message: display on the bottom of the screen
			string msgString;
			msgString = m.getAddress();
			msgString += ":";
			for(size_t i = 0; i < m.getNumArgs(); i++){

				// get the argument type
				msgString += " ";
				msgString += m.getArgTypeName(i);
				msgString += ":";

				// display the argument - make sure we get the right type
				if(m.getArgType(i) == OFXOSC_TYPE_INT32){
					msgString += ofToString(m.getArgAsInt32(i));
				}
				else if(m.getArgType(i) == OFXOSC_TYPE_FLOAT){
					msgString += ofToString(m.getArgAsFloat(i));
				}
				else if(m.getArgType(i) == OFXOSC_TYPE_STRING){
					msgString += m.getArgAsString(i);
				}
				else{
					msgString += "unhandled argument type " + m.getArgTypeName(i);
				}
			}

			// add to the list of strings to display
			msgStrings[currentMsgString] = msgString;
			timers[currentMsgString] = ofGetElapsedTimef() + 5.0f;
			currentMsgString = (currentMsgString + 1) % NUM_MSG_STRINGS;

			// clear the next line
			msgStrings[currentMsgString] = "";
		}
	}

}


//--------------------------------------------------------------
void ofApp::draw(){
    //SYPHON
    // Clear with alpha, so we can capture via syphon and composite elsewhere should we want.
    glClearColor(0.0, 0.0, 0.0, 0.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    
    //FBO
    ofSetColor(255, 255, 255);
    rgbaFbo.draw(0,0);
    
    feedImg.draw(0,0);

    individualTextureSyphonServer.publishTexture(&rgbaFbo.getTexture());
    
}

//--------------------------------------------------------------
void ofApp::drawFboTest(){
    //we clear the fbo if c is pressed.
    //this completely clears the buffer so you won't see any trails
    if( ofGetKeyPressed('c') ){
        ofClear(255,255,255, 0);
    }
    
    //some different alpha values for fading the fbo
    //the lower the number, the longer the trails will take to fade away.
    fadeAmnt = 40;
    if(ofGetKeyPressed('1')){
        fadeAmnt = 1;
    }else if(ofGetKeyPressed('2')){
        fadeAmnt = 5;
    }else if(ofGetKeyPressed('3')){
        fadeAmnt = 15;
    }
    
    //1 - Fade Fbo
    
    //this is where we fade the fbo
    //by drawing a rectangle the size of the fbo with a small alpha value, we can slowly fade the current contents of the fbo.
    ofFill();
    ofSetColor(255,255,255, fadeAmnt);
    ofDrawRectangle(0,0,400,400);
    
    //2 - Draw graphics
    
    ofNoFill();
    ofSetColor(255,255,255);
    
    //we draw a cube in the center of the fbo and rotate it based on time
    ofPushMatrix();
    ofTranslate(200,200,0);
    ofRotateDeg(ofGetElapsedTimef()*30, 1,0,0.5);
    ofDrawBox(0,0,0,100);
    ofPopMatrix();
    
    //also draw based on our mouse position
    ofFill();
    ofDrawCircle(ofGetMouseX() % 410, ofGetMouseY(), 8);
    
    //we move a line across the screen based on the time
    //the %400 makes the number stay in the 0-400 range.
    int shiftX   = (ofGetElapsedTimeMillis() / 8 ) % 400;
    
    ofDrawRectangle(shiftX, rgbaFbo.getHeight()-30, 3, 30);
    
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){

}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y){

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
