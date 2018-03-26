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
	

	this->rng_ = new cnbi::draw::Ring(this->rng_size_, this->rng_thick_, dtk_aluminium2_med);
	this->fix_ = new cnbi::draw::Cross(this->fix_size_, this->fix_thick_, dtk_white);
	this->cue_ = new cnbi::draw::Arrow(this->cue_width_, this->cue_height_, dtk_skyblue_dark);

	this->Add("0_ring", this->rng_);
	this->Add("1_fix", this->fix_);
	this->Add("2_cue", this->cue_);
}

CmWheel::~CmWheel(void) {

	this->Remove("0_rng");
	this->Remove("1_fix");
	this->Remove("2_cue");

	delete this->rng_;
	delete this->fix_;
	delete this->cue_;

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
