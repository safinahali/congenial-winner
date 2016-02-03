#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){

	#ifdef _USE_LIVE_VIDEO
        vidGrabber.setVerbose(true);
        vidGrabber.setup(320,240);
	#else
        vidPlayer.load("fingers.mov");
        vidPlayer.play();
        vidPlayer.setLoopState(OF_LOOP_NORMAL);
	#endif

    colorImg.allocate(320,240);
	grayImage.allocate(320,240);
	grayBg.allocate(320,240);
	grayDiff.allocate(320,240);

	bLearnBakground = true;
	threshold = 80;
    
    
    ofSetVerticalSync(true);
    ofEnableSmoothing();
    
    drawPadding = false;
    gui = new ofxUISuperCanvas("BUTTONS/TOGGLES");
    gui->addSpacer();
    gui->addLabel("BUTTONS");
    gui->addButton("B1", false, 44, 44);
    gui->addButton("B2", false);
    gui->addSpacer();
    gui->addLabel("TOGGLES");
    gui->addToggle("T1", false, 44, 44);
    gui->addToggle("T2", false);
    gui->addSpacer();
    
    vector<string> hnames; hnames.push_back("OPEN"); hnames.push_back("FRAME"); hnames.push_back("WORKS");
    gui->addLabel("HORIZONTAL RADIO");
    gui->addRadio("HR", hnames, OFX_UI_ORIENTATION_HORIZONTAL);
    
    gui->addSpacer();
    
    vector<string> vnames; vnames.push_back("ROCKS"); vnames.push_back("MY"); vnames.push_back("SOCKS");
    gui->addLabel("VERTICAL RADIO", OFX_UI_FONT_MEDIUM);
    ofxUIRadio *radio = gui->addRadio("VR", vnames, OFX_UI_ORIENTATION_VERTICAL);
    radio->activateToggle("SOCKS");
    
    gui->addSpacer();
    
    gui->addLabelButton("LABEL BTN", false);
    gui->addLabelButton("JUSTIFIED LEFT BTN", false, true);
    
    gui->addLabelToggle("LABEL TGL", false);
    gui->addLabelToggle("JUSTIFIED LEFT TGL", false, true);
    
    gui->setGlobalButtonDimension(24);
    gui->addSpacer();
    gui->addToggleMatrix("3X3 MATRIX", 3, 3);
    gui->addToggleMatrix("DISABLE MULTIPLE", 3, 3);
    
    ofxUIToggleMatrix* mtx = (ofxUIToggleMatrix *) gui->getWidget("DISABLE MULTIPLE");
    mtx->setAllowMultiple(false);
    gui->addToggleMatrix("CUSTOM SIZE", 1,6);
    
    gui->addSpacer();
    gui->addLabel("IMAGE BTN/TGL");
    
    gui->setGlobalButtonDimension(44);
    gui->addMultiImageButton("BUTTON", "GUI/toggle.png", false);
    gui->setWidgetPosition(OFX_UI_WIDGET_POSITION_RIGHT);
    gui->addMultiImageToggle("TOGGLE", "GUI/toggle.png", false);
    gui->setWidgetPosition(OFX_UI_WIDGET_POSITION_DOWN);
    
    gui->autoSizeToFitWidgets();
    ofAddListener(gui->newGUIEvent,this,&ofApp::guiEvent);
}

//--------------------------------------------------------------
void ofApp::update(){
	ofBackground(100,100,100);

    bool bNewFrame = false;

	#ifdef _USE_LIVE_VIDEO
       vidGrabber.update();
	   bNewFrame = vidGrabber.isFrameNew();
    #else
        vidPlayer.update();
        bNewFrame = vidPlayer.isFrameNew();
	#endif

	if (bNewFrame){

		#ifdef _USE_LIVE_VIDEO
            colorImg.setFromPixels(vidGrabber.getPixels());
	    #else
            colorImg.setFromPixels(vidPlayer.getPixels());
        #endif

        grayImage = colorImg;
		if (bLearnBakground == true){
			grayBg = grayImage;		// the = sign copys the pixels from grayImage into grayBg (operator overloading)
			bLearnBakground = false;
		}

		// take the abs value of the difference between background and incoming and then threshold:
		grayDiff.absDiff(grayBg, grayImage);
		grayDiff.threshold(threshold);

		// find contours which are between the size of 20 pixels and 1/3 the w*h pixels.
		// also, find holes is set to true so we will get interior contours as well....
		contourFinder.findContours(grayDiff, 20, (340*240)/3, 10, true);	// find holes
	}

}

//--------------------------------------------------------------
void ofApp::guiEvent(ofxUIEventArgs &e)
{
    string name = e.widget->getName();
    int kind = e.widget->getKind();
    
    if(kind == OFX_UI_WIDGET_BUTTON)
    {
        ofxUIButton *button = (ofxUIButton *) e.widget;
        cout << name << "\t value: " << button->getValue() << endl;
    }
    else if(kind == OFX_UI_WIDGET_TOGGLE)
    {
        ofxUIToggle *toggle = (ofxUIToggle *) e.widget;
        cout << name << "\t value: " << toggle->getValue() << endl;
    }
    else if(kind == OFX_UI_WIDGET_IMAGEBUTTON)
    {
        ofxUIImageButton *button = (ofxUIImageButton *) e.widget;
        cout << name << "\t value: " << button->getValue() << endl;
    }
    else if(kind == OFX_UI_WIDGET_IMAGETOGGLE)
    {
        ofxUIImageToggle *toggle = (ofxUIImageToggle *) e.widget;
        cout << name << "\t value: " << toggle->getValue() << endl;
    }
    else if(kind == OFX_UI_WIDGET_LABELBUTTON)
    {
        ofxUILabelButton *button = (ofxUILabelButton *) e.widget;
        cout << name << "\t value: " << button->getValue() << endl;
    }
    else if(kind == OFX_UI_WIDGET_LABELTOGGLE)
    {
        ofxUILabelToggle *toggle = (ofxUILabelToggle *) e.widget;
        cout << name << "\t value: " << toggle->getValue() << endl;
    }
    else if(name == "B1")
    {
        ofxUIButton *button = (ofxUIButton *) e.widget;
        cout << "value: " << button->getValue() << endl;
    }
    else if(name == "B2")
    {
        ofxUIButton *button = (ofxUIButton *) e.widget;
        cout << "value: " << button->getValue() << endl;
    }
    else if(name == "T1")
    {
        ofxUIToggle *toggle = (ofxUIToggle *) e.widget;
        cout << "value: " << toggle->getValue() << endl;
    }
    else if(name == "T2")
    {
        ofxUIToggle *toggle = (ofxUIToggle *) e.widget;
        cout << "value: " << toggle->getValue() << endl;
    }
}


//-----
void ofApp::draw(){

	// draw the incoming, the grayscale, the bg and the thresholded difference
	ofSetHexColor(0xffffff);
	colorImg.draw(20,20);
	grayImage.draw(360,20);
	grayBg.draw(20,280);
	grayDiff.draw(360,280);

	// then draw the contours:

	ofFill();
	ofSetHexColor(0x333333);
	ofDrawRectangle(360,540,320,240);
	ofSetHexColor(0xffffff);

	// we could draw the whole contour finder
	//contourFinder.draw(360,540);

	// or, instead we can draw each blob individually from the blobs vector,
	// this is how to get access to them:
    for (int i = 0; i < contourFinder.nBlobs; i++){
        contourFinder.blobs[i].draw(360,540);
		
		// draw over the centroid if the blob is a hole
		ofSetColor(255);
		if(contourFinder.blobs[i].hole){
			ofDrawBitmapString("hole",
				contourFinder.blobs[i].boundingRect.getCenter().x + 360,
				contourFinder.blobs[i].boundingRect.getCenter().y + 540);
		}
    }

	// finally, a report:
	ofSetHexColor(0xffffff);
	stringstream reportStr;
	reportStr << "bg subtraction and blob detection" << endl
			  << "press ' ' to capture bg" << endl
			  << "threshold " << threshold << " (press: +/-)" << endl
			  << "num blobs found " << contourFinder.nBlobs << ", fps: " << ofGetFrameRate();
	ofDrawBitmapString(reportStr.str(), 20, 600);

}



//--------------------------------------------------------------
void ofApp::keyPressed(int key){

	switch (key){
		case ' ':
			bLearnBakground = true;
			break;
		case '+':
			threshold ++;
			if (threshold > 255) threshold = 255;
			break;
		case '-':
			threshold --;
			if (threshold < 0) threshold = 0;
			break;
            
        case 'p':
        {
            drawPadding = !drawPadding;
            gui->setDrawWidgetPadding(drawPadding);
        }
            break;
            
        case 'g':
        {
            gui->toggleVisible();
        }
            break;
        default:
            break;
	}
    
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
