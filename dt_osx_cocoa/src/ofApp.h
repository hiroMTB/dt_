#pragma once

#include "ofMain.h"

#include "def.h"
#include "dt_midi_writer.h"
#include "dt_osc_sender.h"
#include "dt_midi_sender.h"
#include "dt_font_manager.h"
#include "dt_rhythm_lib.h"
#include "dt_touch.h"
#include "dt_circle_drawer.h"
#include "dt_linear_drawer.h"
#include "dt_sequence_thread.h"
#include "dt_circle_all_containers.h"

class ofApp : public ofBaseApp {

private:
	ofApp();
	
public:
	static ofApp * instance;	
	static ofApp * init();
	static ofApp * getInstance();
	
	void setup();
	void setupVisual();
	void setupModule();
	
	void update();
	void draw();
	void draw_info(int x, int y);
	void exit();
	
	void keyPressed(int key);
	void keyReleased(int key){};
	
	void mousePressed(int x, int y, int button);
	void mouseDragged(int x, int y, int button);
	void mouseReleased(int x, int y, int button);
	void mouseMoved(int x, int y){};
	
	void windowResized(int w, int h);
	void dragEvent(ofDragInfo dragInfo){};
	void gotMessage(ofMessage msg);
		
	// module
	dt_osc_sender		osc_sender;
	dt_midi_sender		midi_sender;
	dt_midi_writer		midi_writer;
	dt_font_manager		font_manager;
	dt_rhythm_lib		rhythm_lib;
	dt_touch			touch;
	dt_circle_drawer	circle_drawer;
	dt_linear_drawer	linear_drawer;
	dt_sequence_thread	sequence_thread;
	dt_circle_all_containers all_containers;

	
	ofRectangle canvas;
	bool bShow_linear_drawer;
};

