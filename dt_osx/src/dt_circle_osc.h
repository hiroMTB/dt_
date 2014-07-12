//
//  dt_circle_osc.h
//  dialt_osx
//
//  Created by mtb on 2014/06/15.
//
//

#pragma once

#include "ofMain.h"
#include "dt_circle_base.h"
#include "dt_circle_output_base.h"
#include "dt_circle_osc.h"
#include "ofxOsc.h"
#include "dt_osc_sender.h"
#include "ofApp.h"

class dt_circle_osc : public dt_circle_output_base{
	
	
public:
	

    string address_prefix;

	dt_circle_osc(){}
	
	virtual void setup(int beat_num){
		data.circle_color = data.indi_color = ofFloatColor(0.5);
		data.line_color = ofFloatColor(0.2);
		
		data.radius = 3;
		data.rev_radius = 12;
		data.bCollide = true;
		data.collision_radius = data.rev_radius + ofRandom(10, 30);
		
		data.circle_type = DT_CIRCLE_OSC;
		ch = (int)ceil(ofRandom(0, dt_config::DT_MAX_OSC_CH));
		initial = ofToString(ch);
        
        address_prefix = dt_config::DT_OSC_OUT_ADDRESS_PREFIX + initial + "/";	// eg /dt/1/
        
        setup_text(initial);
	}
	
	
	/*
	 * override with empty function because we dont need step sequence
	 */
	virtual void fire(){};
	virtual void step(){};

	
	// send osc
	void send_osc(int _ch, int _noteNum, int _vel, int _dur, int _pan,  int _cc12, int _cc13, int _cc14, int _cc16){
		ofxOscMessage noteOn;

		noteOn.setAddress	(address_prefix + "noteOn");
		noteOn.addIntArg(1);
		noteOn.addIntArg(_noteNum);
		noteOn.addIntArg(_vel);
		noteOn.addIntArg(_dur);
		noteOn.addIntArg(_pan);
		
		noteOn.addIntArg(_cc12);
		noteOn.addIntArg(_cc13);
		noteOn.addIntArg(_cc14);
		noteOn.addIntArg(16);
		
		app->osc_sender.send_message(noteOn);
	};
};