#ifndef CNBITK_MOBILE_WHEEL_CPP
#define CNBITK_MOBILE_WHEEL_CPP

#include "CmWheel.hpp"

namespace cnbi {
	namespace mobile {

CmWheel::CmWheel(float radius, float arc, float thick) {


	this->rng_size_		= radius;
	this->rng_thick_	= thick;
	this->fix_size_		= 0.2f;
	this->fix_thick_	= 0.03f;
	this->cue_width_	= 0.3f;
	this->cue_height_	= 0.3f;
	this->mrk_width_	= 0.005f;
	this->mrk_height_	= thick;
	

	this->rng_  = new cnbi::draw::Ring(this->rng_size_, this->rng_thick_, dtk_aluminium2_med);
	this->fix_  = new cnbi::draw::Cross(this->fix_size_, this->fix_thick_, dtk_white);
	this->cue_  = new cnbi::draw::Arrow(this->cue_width_, this->cue_height_, dtk_skyblue_dark);
	this->mrkm_ = new cnbi::draw::Rectangle(this->mrk_width_, this->mrk_height_, dtk_chameleon_dark);
	this->mrkl_ = new cnbi::draw::Rectangle(this->mrk_width_, this->mrk_height_, dtk_skyblue_dark);
	this->mrkr_ = new cnbi::draw::Rectangle(this->mrk_width_, this->mrk_height_, dtk_red);

	this->mrkm_->Move(0.0f, radius - thick/2.0f);

	this->Add("ring", this->rng_);
	this->Add("mrkm", this->mrkm_);
	this->Add("mrkl", this->mrkl_);
	this->Add("mrkr", this->mrkr_);
	this->Add("fix",  this->fix_);
	this->Add("cue",  this->cue_);
	
	this->RaiseTop("mrkm");
	this->RaiseTop("mrkl");
	this->RaiseTop("mrkr");
}

CmWheel::~CmWheel(void) {

	this->Remove("ring");
	this->Remove("mrkm");
	this->Remove("mrkl");
	this->Remove("mrkr");
	this->Remove("fix");
	this->Remove("cue");

	delete this->rng_;
	delete this->mrkm_;
	delete this->fix_;
	delete this->cue_;

}

void CmWheel::ConfigureTaskset(CCfgTaskset* taskset) {


	float radius = this->rng_size_;
	float thick  = this->rng_thick_;
	float angle;
	float threshold;

	for(auto it=taskset->Begin(); it!=taskset->End(); ++it) {

		if(it->second->id == 0) {
			if(it->second->HasConfig("threshold") == true) {
				threshold = it->second->config["threshold"].Float();
			} else {
				CcLogFatalS("Task " << it->second->gdf <<" does not have 'threshold' field"); 
			}

			angle = M_PI/2.0f*(threshold-0.5f)/0.5f;

			this->mrkl_->Move( (radius-thick/2.0f)*cos( angle + M_PI/2.0f), 
							   (radius-thick/2.0f)*sin( angle + M_PI/2.0f));
			this->mrkl_->RelRotate( angle*180.0f/M_PI );
							  
		} else if(it->second->id == 1) {

			if(it->second->HasConfig("threshold") == true) {
				threshold = it->second->config["threshold"].Float();
			} else {
				CcLogFatalS("Task " << it->second->gdf <<" does not have 'threshold' field"); 
			}

			angle = M_PI/2.0f*(threshold-0.5f)/0.5f;

			this->mrkr_->Move(-( radius-thick/2.0f)*cos( angle + M_PI/2.0f), 
							   ( radius-thick/2.0f)*sin( angle + M_PI/2.0f));
			this->mrkr_->RelRotate(-angle*180.0f/M_PI );
		}
	}


}

void CmWheel::Show(unsigned int element) {

	switch(element) {
		case cnbi::mobile::Feedback::Fixation:
			this->cue_->Hide();
			this->fix_->Show();
			break;
		case cnbi::mobile::Feedback::Cue:
			this->fix_->Hide();
			this->cue_->Show();
			break;
		case cnbi::mobile::Feedback::Slice:
			break;
		defaul:
			break;
	}
}


	}
}


#endif
