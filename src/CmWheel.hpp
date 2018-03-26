#ifndef CNBITK_MOBILE_WHEEL_HPP
#define CNBITK_MOBILE_WHEEL_HPP

#include <cmath>
#include <cnbidraw/Engine.hpp>
#include <cnbidraw/Ring.hpp>
#include <cnbidraw/Cross.hpp>
#include <cnbidraw/Arrow.hpp>

namespace cnbi {
	namespace mobile {

enum Feedback {Fixation, Slice, Cue};

class CmWheel : public cnbi::draw::Engine {

	public:
		CmWheel(float radius = 0.6f, float arc = M_PI/2.0f, float thick = 0.1f);
		virtual ~CmWheel(void);

		void Update(float angle) {};
		void Reset(void) {};
		void Show(unsigned int element);

		void Hard(unsigned int direction) {};
	
	protected:
		cnbi::draw::Ring*	rng_;
		cnbi::draw::Cross*	fix_;
		cnbi::draw::Arrow*	cue_;

		float rng_size_;
		float rng_thick_;
		float fix_size_;
		float fix_thick_;
		float cue_width_;
		float cue_height_;
		

};

	}
}


#endif
