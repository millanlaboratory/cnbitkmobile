#ifndef CNBITK_MOBILE_WHEEL_CPP
#define CNBITK_MOBILE_WHEEL_CPP

#include "CmWheel.hpp"

namespace cnbi {
	namespace mobile {

CmWheel::CmWheel(float radius, float arc, float thick) {

	this->is_space_pressed_ = false;
	this->is_esc_pressed_   = false;

	this->rng_size_		= radius;
	this->rng_thick_	= thick;
	this->fix_size_		= 0.25f;
	this->fix_thick_	= 0.05f;
	this->cue_width_	= 0.3f;
	this->cue_height_	= 0.3f;
	this->fdb_length_   = M_PI/2.0f;
	this->fdb_thick_    = thick;
	this->fdb_radius_	= radius;
	this->mrk_width_	= 0.01f;
	this->mrk_height_	= thick;
	

	this->rng_  = new cnbi::draw::Ring(this->rng_size_, this->rng_thick_, dtk_aluminium2_med);
	this->fix_  = new cnbi::draw::Cross(this->fix_size_, this->fix_thick_, dtk_white);
	this->cue_  = new cnbi::draw::Arrow(this->cue_width_, this->cue_height_, dtk_skyblue_med);
	this->fdb_  = new cnbi::draw::Arc(this->fdb_radius_, this->fdb_length_, this->fdb_thick_, dtk_aluminium_dark);
	this->mrkm_ = new cnbi::draw::Rectangle(this->mrk_height_, this->mrk_width_, dtk_chameleon_dark);
	this->mrkl_ = new cnbi::draw::Rectangle(this->mrk_height_, this->mrk_width_, dtk_skyblue_med);
	this->mrkr_ = new cnbi::draw::Rectangle(this->mrk_height_, this->mrk_width_, dtk_scarletred_med);
	this->mrkf_ = new cnbi::draw::Rectangle(this->mrk_height_, this->mrk_width_, dtk_aluminium2_dark);

	// Default thresholds, and cue directions
	this->mrkf_->Move(0.0f, this->rng_size_ - this->rng_thick_/2.0f);
	this->mrkl_->Move(-(this->rng_size_ - this->rng_thick_/2.0f), 0.0f); 
	this->mrkr_->Move(this->rng_size_ - this->rng_thick_/2.0f, 0.0f); 
	this->mrkm_->Move(0.0f, this->rng_size_ - this->rng_thick_/2.0f);
	this->mrkm_->Rotate(90.0f);
	this->mrkf_->Rotate(90.0f);
	this->fdb_->Rotate(this->fdb_length_*180.0f/(2.0f*M_PI));
	this->cue_->Move(0.0f, 0.0f);

	// Text
	this->font_ = new cnbi::draw::Font("arial:style=bold");	
	this->text_ = new cnbi::draw::String(" ", 0.1f, dtk_white);
	this->text_->SetFont(this->font_);

	// Images
	this->imgl_  = new cnbi::draw::Image(0.5f, 0.5f*0.6f);
	this->imgl_->Move(0.0f, -0.3f);
	this->imgr_  = new cnbi::draw::Image(0.5f, 0.5f*0.6f);
	this->imgr_->Move(0.0f, -0.3f);
	
	this->Add("ring", this->rng_);
	this->Add("mrkm", this->mrkm_);
	this->Add("mrkl", this->mrkl_);
	this->Add("mrkr", this->mrkr_);
	this->Add("fix",  this->fix_);
	this->Add("cue",  this->cue_);
	this->Add("fdb",  this->fdb_);
	this->Add("mrkf", this->mrkf_);
	this->Add("text", this->text_);
	this->Add("imgl", this->imgl_);
	this->Add("imgr", this->imgr_);

	this->RaiseTop("fdb");
	this->RaiseTop("mrkf");
	this->RaiseTop("mrkm");
	this->RaiseTop("mrkl");
	this->RaiseTop("mrkr");
	this->LowerBottom("imgl");
	this->LowerBottom("imgr");

	this->cue_->Hide();
	this->fix_->Hide();
	this->text_->Hide();
	this->imgl_->Hide();
	this->imgr_->Hide();


	// Events
	this->events_ = new cnbi::draw::Events(this);
	this->events_->onKeyboard = std::bind(&cnbi::mobile::CmWheel::on_keyboard_event, 
										  this, std::placeholders::_1);
	this->events_->Start();
}

CmWheel::~CmWheel(void) {

	this->Remove("ring");
	this->Remove("mrkm");
	this->Remove("mrkl");
	this->Remove("mrkr");
	this->Remove("fix");
	this->Remove("cue");
	this->Remove("fdb");
	this->Remove("mrkf");
	this->Remove("text");
	this->Remove("imgl");
	this->Remove("imgr");

	delete this->rng_;
	delete this->mrkm_;
	delete this->fix_;
	delete this->cue_;
	delete this->fdb_;
	delete this->mrkl_;
	delete this->mrkr_;
	delete this->mrkf_;
	delete this->text_;
	delete this->font_;
	delete this->events_;
	delete this->imgl_;
	delete this->imgr_;


}

void CmWheel::SetImage(unsigned int taskId, std::string path) {

	std::string message;
	this->map_images_[taskId] = path;

	message = "Assigned image " + path;
	switch(taskId) {
		case cnbi::mobile::Task::Left:
			this->imgl_->Set(path);
			message += " to cue left";
			CcLogConfigS(message);
			break;
		case cnbi::mobile::Task::Right:
			this->imgr_->Set(path);
			message += " to cue right";
			CcLogConfigS(message);
			break;
		default:
			CcLogFatalS("Unknown task (" + std::to_string(taskId) + ")");
			break;
	}
}

void CmWheel::SetThreshold(unsigned int taskId, float threshold) {

	float angle, x, y, w, th;
	std::string message;
	cnbi::draw::Rectangle* mrk;

	th = threshold;
	mrk = this->mrkl_;
	
	if(taskId == cnbi::mobile::Task::Right) {
		th = 1.0f - th;
		mrk = this->mrkr_;
	}

	this->value2polar(th, &x, &y, &angle);
	w = angle*180.0f/M_PI;
	
	mrk->Move(x, y); 
	mrk->Rotate(w);
	this->map_thresholds_[taskId] = th;

	
	message = "Assigned threshold " + std::to_string(threshold);
	switch(taskId) {
		case cnbi::mobile::Task::Left:
			message += " to task left";
			CcLogConfigS(message);
			break;
		case cnbi::mobile::Task::Right:
			message += " to task right";
			CcLogConfigS(message);
			break;
		default:
			CcLogFatalS("Unknown task (" + std::to_string(taskId) + ")");
			break;
	}

}

bool CmWheel::GetThreshold(unsigned int taskId, float* threshold) {

	bool res = false;
	float th;
	auto it = this->map_thresholds_.find(taskId);

	if( it != this->map_thresholds_.end() ) {
		
		th = it->second;
		if(taskId == cnbi::mobile::Task::Right)
			th = 1.0f - th;
		
		*threshold = th;
		res = true;
	} else {
		threshold = nullptr;
	}

	return res;
}


void CmWheel::ShowFixation(void) {
	this->cue_->Hide();
	this->imgl_->Hide();
	this->imgr_->Hide();
	this->text_->Hide();
	this->fix_->Show();
}

void CmWheel::ShowCue(unsigned int taskId) {

	float angle;
	bool found = false;

	this->fix_->Hide();
	this->text_->Hide();
	switch(taskId) {
		case cnbi::mobile::Task::Left:
			this->cue_->SetColor(dtk_skyblue_med);
			this->cue_->Rotate(0.0f);
			break;
		case cnbi::mobile::Task::Right:
			this->cue_->SetColor(dtk_scarletred_med);
			this->cue_->Rotate(180.0f);
			break;
	}

	this->cue_->Show();
}

void CmWheel::ShowImage(unsigned int taskId) {

	switch(taskId) {
		case cnbi::mobile::Task::Left:
			this->imgl_->Show();
			this->imgr_->Hide();
			break;
		case cnbi::mobile::Task::Right:
			this->imgl_->Hide();
			this->imgr_->Show();
			break;
		default:
			this->imgl_->Hide();
			this->imgr_->Hide();
			break;
	}


	//auto it = this->map_images_.find(taskId);

	//if (it != this->map_images_.end()) {
	//	
	//	if(this->img_->Set(it->second))
	//		this->img_->Show();
	//}
}

void CmWheel::ShowText(std::string text, float x, float y) {

	this->fix_->Hide();
	this->cue_->Hide();
	this->imgl_->Hide();
	this->imgr_->Hide();
	this->text_->SetText(text);
	this->text_->Show();
}

int CmWheel::Update(float value) {

	int retvalue = -1;
	float angle;
	float fdb_w, mrk_x, mrk_y, mrk_w;

	this->value2polar(value, &mrk_x, &mrk_y, &angle);
	mrk_w = angle*180.0f/M_PI;
	fdb_w = (angle-this->fdb_length_/2.0f)*180.0f/M_PI;

	this->fdb_->Rotate(fdb_w);
	this->mrkf_->Move(mrk_x, mrk_y);
	this->mrkf_->Rotate(mrk_w);


	if(value >= this->map_thresholds_[cnbi::mobile::Task::Left]) {
		retvalue = cnbi::mobile::Task::Left;
	} else if( value <= this->map_thresholds_[cnbi::mobile::Task::Right] ) {
		retvalue = cnbi::mobile::Task::Right;
	}
		
	return retvalue;
}

void CmWheel::Reset(void) {
	this->cue_->Hide();
	this->text_->Hide();
	this->fix_->Hide();
	this->fdb_->SetColor(dtk_aluminium_dark);
	this->Update(0.5f);
	this->imgl_->Hide();
	this->imgr_->Hide();
	
}

void CmWheel::Hard(unsigned int taskId) {

	switch(taskId) {
		case cnbi::mobile::Task::Left:
			this->fdb_->SetColor(dtk_skyblue_med);
			break;
		case cnbi::mobile::Task::Right:
			this->fdb_->SetColor(dtk_scarletred_med);
			break;
	}
}

bool CmWheel::IsStartRequested(void) {
	return this->is_space_pressed_;
}

bool CmWheel::IsQuitRequested(void) {
	return this->is_esc_pressed_;
}

void CmWheel::value2polar(float value, float* x, float* y, float* w) {

	*w = value*M_PI;
	*x = (this->rng_size_-this->rng_thick_/2.0f)*cos( *w );
	*y = (this->rng_size_-this->rng_thick_/2.0f)*sin( *w );
}

void CmWheel::on_keyboard_event(cnbi::draw::EventKeyboard* evt) {

	if(evt->IsPressed(KEY_SPACE)) {
		is_space_pressed_ = true;
	} else if(evt->IsPressed(KEY_ESC)) {
		is_esc_pressed_ = true;
	}

}




	}
}


#endif
