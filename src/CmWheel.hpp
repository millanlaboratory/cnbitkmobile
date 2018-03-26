#ifndef CNBITK_MOBILE_WHEEL_HPP
#define CNBITK_MOBILE_WHEEL_HPP

#include <cmath>
#include <cnbicore/CcBasic.hpp>
#include <cnbiconfig/CCfgTaskset.hpp>

#include <cnbidraw/Engine.hpp>
#include <cnbidraw/Ring.hpp>
#include <cnbidraw/Cross.hpp>
#include <cnbidraw/Arrow.hpp>
#include <cnbidraw/Rectangle.hpp>

namespace cnbi {
	namespace mobile {

enum Feedback {Fixation, Slice, Cue};

class CmWheel : public cnbi::draw::Engine {

	public:
		CmWheel(float radius = 0.6f, float arc = M_PI/2.0f, float thick = 0.1f);
		virtual ~CmWheel(void);

		void ConfigureTaskset(CCfgTaskset* taskset);

		void Update(float angle) {};
		void Reset(void) {};
		void Show(unsigned int element);

		void Hard(unsigned int direction) {};
	
	protected:
		cnbi::draw::Ring*	rng_;
		cnbi::draw::Cross*	fix_;
		cnbi::draw::Arrow*	cue_;
		cnbi::draw::Rectangle*	mrkm_;
		cnbi::draw::Rectangle*	mrkl_;
		cnbi::draw::Rectangle*	mrkr_;

		float rng_size_;
		float rng_thick_;
		float fix_size_;
		float fix_thick_;
		float cue_width_;
		float cue_height_;
		float mrk_width_;
		float mrk_height_;
		

};

	}
}


#endif
