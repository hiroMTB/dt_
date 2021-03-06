//
//  dt_midi_sender.h
//  dialt_osx
//
//  Created by mtb on 2014/06/15.
//
//

#pragma once


#include "ofxMidi.h"
//#undef check
#include <boost/thread.hpp>
#include <boost/chrono.hpp>
using namespace boost;

class dt_midi_sender{
	
	
public:
	dt_midi_sender(){
		//midi_out.listPorts();
	}
	
	void open_port(int port_num){
		midi_out.openVirtualPort("dial_midiout_" + ofToString(port_num));
	}
	
	~dt_midi_sender(){
		midi_out.closePort();
	}
	
	void send_note_on(int ch, int noteNum, int vel, int dur_micro_sec){
		midi_out.sendNoteOn(ch, noteNum, vel);
		if(dur_micro_sec>0){
			boost::thread noteOffThread(&dt_midi_sender::send_note_off, this, ch, noteNum, dur_micro_sec);
		}
	}
	
	void send_cc(int ch, int cc, int value){
		
		midi_out.sendControlChange(ch, cc, value);
		
	}
	
private:
	ofxMidiOut midi_out;

	void send_note_off(int ch, int noteNum, int after_micro_sec){
		midi_out.sendNoteOff(ch, noteNum);
	}
	
};