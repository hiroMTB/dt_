//
//  dt_circle_note_on.cpp
//  dial_t
//
//  Created by mtb on 5/5/14.
//
//

#include "dt_circle_note_on.h"
#include "ofMain.h"
#include "ofApp.h"
#include "dt_rhythm_lib.h"
#include "dt_dial_ui.h"
#include "dt_sequence_thread.h"
#include "dt_font_manager.h"
#include "dt_circle_container.h"

#include "dt_circle_param.h"
#include "dt_circle_synth.h"
#include "dt_circle_osc.h"
#include "dt_circle_midi.h"
#include "dt_circle_drawer.h"

dt_circle_note_on::dt_circle_note_on(){
	data.circle_type = DT_CIRCLE_NOTE_ON;

	ui = new dt_dial_ui(this);

	rshape_points.reserve(DT_RHYTHM_SHAPE_MAX_SLOT);
	data.indi_color.set(0.95);
	data.circle_color.set(0.3);
	data.line_color.set(0.1);
}


dt_circle_note_on::~dt_circle_note_on(){
	if(ui) {
		delete ui;
		ui = 0;
	}
}

/*
 
		speedに影響を受けるパラメータ
			- rev_radius
			- collision_radius
			- input/output_connection_radius
 
		beat num に影響を受けるパラメータ
			- sequencer
			- rev_speed
			- vbo
	
			- rev_radius
			- collision_radius
			- input/output_connection_radius

 */
void dt_circle_note_on::setup(int beat_num){

	seq = new dt_sequencer();

	set_beats(beat_num);
	set_speed( (int)round(ofRandom(1, 4)));

	// Quantize to beat position
	int steps_per_beat = DT_BEAT_RESOLUTION; //* data.speed;
	int sub_step = app->sequence_thread.master_step % steps_per_beat;
	wait_step = steps_per_beat - sub_step;
		
	// ROTATE little bit
	int quantize_step = DT_BEAT_RESOLUTION / DT_QUANTIZE_RESOLUTION;
	seq->indicator = quantize_step * (int)round(ofRandom(0, DT_QUANTIZE_RESOLUTION-1));
}

void dt_circle_note_on::set_beats(int beat_num){
	seq->setup(beat_num);
	data.rev_speed = (float)360.0 / (float)seq->total_steps;
	change_rshape(ofRandom(-1000, 1000));
	
	set_speed(data.speed);
}

void dt_circle_note_on::set_speed(int speed){
	data.speed = speed;
	data.rev_angle = seq->indicator * data.rev_speed;
	data.rev_radius = 15 + (seq->total_beats * data.speed)*0.003;
	data.collision_radius = data.rev_radius + 15;
	data.input_connection_radius = data.collision_radius + 100;
	data.output_connection_radius = data.collision_radius + 100;
}

void dt_circle_note_on::update(){
	data.fire_rate*=0.8;

	data.position += data.move_speed;
	//data.world_position = data.position + data.indi_current_point;
	
	float deg = 360.0 * seq->indicator/seq->total_steps * DEG_TO_RAD;
	data.world_position = data.position + ofVec2f(cos(deg), sin(deg)) * data.rev_radius;
	
	check_connection();
}

void dt_circle_note_on::check_connection(){
	
	input_circles.clear();
	output_circles.clear();
	
	//	input check
	ofApp::getInstance()->all_containers.param_container->check_connection(this, true);
	
	// output check
	ofApp::getInstance()->all_containers.output_container->check_connection(this, false);
	
}

void dt_circle_note_on::draw(){
	
	float waiting_rate = (float)(DT_BEAT_RESOLUTION-wait_step) / (float)DT_BEAT_RESOLUTION;
	
	ofPushMatrix();
	ofTranslate(data.position.x, data.position.y);
	
	float waiting_animation_rate = 0.5 + waiting_rate*0.5;
	ofScale(waiting_animation_rate, waiting_animation_rate);

	bool fired = data.fire_rate>0.3;
	bool selected = selected_circle == this;

	
	// shape
	int mode = GL_LINE_LOOP;

	glLineWidth(1);

	if(fired){
		ofSetColor(200);
	}else{
		ofSetColor(data.circle_color);
	}



	glPushMatrix();{
		glScalef(data.rev_radius, data.rev_radius, 1);
		rshape_vbo.bind();
		rshape_vbo.draw(mode, 0, rshape_points.size());
		rshape_vbo.unbind();
	}glPopMatrix();
	
	//if(data.bShowUI) ui->draw();
	
	ofPopMatrix();
}




void dt_circle_note_on::post_step(){
	data.indi_current_point += data.indi_point_adder;
}



void dt_circle_note_on::change_rshape(int type){
	seq->setRhythmShape(type);
	make_vbo();
}

void dt_circle_note_on::fire(){

	int note_num_count =0, velocity_count=0, duration_count=0, pan_count=0, cc12_count=0, cc13_count=0, cc14_count=0;
	
	// input paramater
	for(int i=0; i<input_circles.size(); i++){
		dt_circle_param_base * pb = static_cast<dt_circle_param_base*>(input_circles[i]);
		bool marked = pb->data.bFired;
		//if(marked){
			float param = marked ? pb->param_on : pb->param_off;
			
			dt_circle_type &type = pb->data.circle_type;
			
			switch(type){
				case DT_CIRCLE_VELOCITY: velocity_count++; break;
				case DT_CIRCLE_NOTE_NUM: note_num_count++;	break;
				case DT_CIRCLE_DURATION: duration_count++; break;
				case DT_CIRCLE_L:		 pan_count++; break;
				case DT_CIRCLE_R:		 pan_count++; break;
				case DT_CIRCLE_CC12:	 cc12_count++; break;
				case DT_CIRCLE_CC13:	 cc13_count++; break;
				case DT_CIRCLE_CC14:	 cc14_count++; break;
				default: break;
			}
		//}
	}
	
	velocity_count  = (velocity_count>4) ? 4 : velocity_count;
	note_num_count  = (note_num_count>4) ? 4 : note_num_count;
	duration_count  = (duration_count>4) ? 4 : duration_count;
	cc12_count		= (cc12_count>4) ? 4 : cc12_count;
	cc13_count		= (cc13_count>4) ? 4 : cc13_count;
	cc14_count		= (cc14_count>4) ? 4 : cc14_count;
	pan_count		= (pan_count>4) ? 4 : pan_count;

	// output
	for(int i=0; i<output_circles.size(); i++){
		
		dt_circle_type &type = output_circles[i]->data.circle_type;
		switch (type) {
			case DT_CIRCLE_SYNTH:
			{
//				dt_circle_synth::note_off_all_synth();
//				dt_circle_synth * s = static_cast<dt_circle_synth*>(output_circles[i]);
//				s->make_note(note_num, velocity, duration*0.001, pan, cc12, cc13, cc14);
			}
			break;
			
			case DT_CIRCLE_OSC:
			{
				dt_circle_osc * o = static_cast<dt_circle_osc*>((output_circles[i]));
				o->send_osc(o->ch, note_num_count, velocity_count, duration_count, pan_count, cc12_count, cc13_count, cc14_count);
				data.fired_ch = o->ch;
				
//				dt_trigger_event e(ofApp::getInstance()->sequence_thread.master_step, o->ch, o->ch + 48, 100, 10, 9, 12, 13, 14, 15);
//				app->midi_writer.add_trigger_event(e);
			}
				break;
			
			case DT_CIRCLE_MIDI:
			{
				dt_circle_midi * m = static_cast<dt_circle_midi*>(output_circles[i]);
				m->send_midi(m->ch, note_num_count, velocity_count, duration_count, pan_count, cc12_count, cc13_count, cc14_count);
			}
			default:
				break;
		}
		
	}
	
//	update_world_position();
	
	if(output_circles.size()!=0){
		data.fire_rate = 1.0;
	}
}

void dt_circle_note_on::update_world_position(){

	int total_beats = seq->total_beats;
	int current_beat_num = seq->getCurrentBeat();
	int next_on_beat;
	int far = 0;
	for(int i=0; i<total_beats; i++){
		far++;
		int check_beat =  (current_beat_num+far)%total_beats;
		bool next_on = seq->getDataFromBeat(check_beat);
		if(next_on){
			next_on_beat = check_beat;
			break;
		}
	}

	//
//	if(next_on_baet == current_beat_num) return;
	
	float start_angle = 0;
	float angle = start_angle + next_on_beat * (360.0/seq->total_beats);
	float r = data.rev_radius * 0.95;
	float x = r * cosf(angle*DEG_TO_RAD);
	float y = r * sinf(angle*DEG_TO_RAD);
	data.indi_next_point.set(x, y);
	
	float angle_c = start_angle + (current_beat_num * (360.0/seq->total_beats));
	float x_c = r * cosf(angle_c*DEG_TO_RAD);
	float y_c = r * sinf(angle_c*DEG_TO_RAD);
	
	data.indi_current_point = ofVec2f(x_c, y_c);
	data.indi_point_adder = (data.indi_next_point - data.indi_current_point) / (float)(far * DT_BEAT_RESOLUTION);
}


void dt_circle_note_on::make_vbo(){
	rshape_points.clear();
	
	int beat_num = seq->total_beats;
	float start_angle = 0;
	float r = 1.0;
	
	for(int i=0; i<beat_num; i++){
		bool on = seq->getDataFromBeat(i);
		if(on){
			float angle = start_angle + (i *  data.rev_speed * DT_BEAT_RESOLUTION);
			
			float x = r * cosf(angle*DEG_TO_RAD);
			float y = r * sinf(angle*DEG_TO_RAD);
            
			rshape_points.push_back(ofVec2f(x,y));
		}
	}
	
	rshape_vbo.setVertexData(&rshape_points[0], rshape_points.size(), GL_DYNAMIC_DRAW);
}

void dt_circle_note_on::make_potato_shape(){
	
	int stepNum = 40;
	float stepAngle = 360.0 / stepNum;
	float r = data.rev_radius;
	
	
	vector<float> rs;
	
	for(int i=0; i<stepNum; i++){
		float newr = r * (1.0 + ofRandom(-0.9, 0.9));
		rs.push_back(newr);
	}
	
	// low pass filter
	for (int k=0; k<15; k++) {
		float pastr = rs[stepNum-1];
		for(int i=0; i<stepNum; i++){
			rs[i]= pastr*0.5 + rs[i]*0.5;
			pastr = rs[i];
		}
	}
	
	for(int i=0; i<stepNum; i++){
		float r = rs[i];
		float angle = i * stepAngle;
		float x = r * (cosf(angle * DEG_TO_RAD));
		float y = r * (sinf(angle * DEG_TO_RAD));
		rshape_points.push_back(ofVec2f(x, y));
	}
	
	rshape_vbo.setVertexData(&rshape_points[0], rshape_points.size(), GL_DYNAMIC_DRAW);
}