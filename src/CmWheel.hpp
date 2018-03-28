#ifndef CNBITK_MOBILE_WHEEL_HPP
#define CNBITK_MOBILE_WHEEL_HPP

#include <cmath>
#include <cnbicore/CcBasic.hpp>

#include <cnbidraw/Engine.hpp>
#include <cnbidraw/Events.hpp>
#include <cnbidraw/EventKeyboard.hpp>
#include <cnbidraw/Ring.hpp>
#include <cnbidraw/Cross.hpp>
#include <cnbidraw/Arrow.hpp>
#include <cnbidraw/Rectangle.hpp>
#include <cnbidraw/Arc.hpp>
#include <cnbidraw/String.hpp>
#include <cnbidraw/Font.hpp>

#define KEY_SPACE	32
#define KEY_ESC		27

namespace cnbi {
	namespace mobile {

enum Feedback {Fixation, Slice, Cue};
enum Task {Right, Left};
enum State {Running, Hit, Missed, Timeout};

class CmWheel : public cnbi::draw::Engine {

	public:
		CmWheel(float radius = 0.6f, float arc = M_PI/2.0f, float thick = 0.1f);
		virtual ~CmWheel(void);

		void SetThreshold(unsigned int taskId, float threshold);

		void ShowFixation(void);
		void ShowCue(unsigned int taskId);
		void ShowText(std::string text, float x=0.0f, float y=0.0f);
		
		int Update(float angle);
		void Reset(void);

		void Hard(unsigned int taskId);

		bool IsStartRequested(void);
		bool IsQuitRequested(void);
	
	private:
		void value2polar(float value, float* x, float* y, float* w);
		void on_keyboard_event(draw::EventKeyboard* evt);

	protected:
		cnbi::draw::Ring*	rng_;
		cnbi::draw::Cross*	fix_;
		cnbi::draw::Arrow*	cue_;
		cnbi::draw::Arc*	fdb_;
		cnbi::draw::Rectangle*	mrkm_;
		cnbi::draw::Rectangle*	mrkl_;
		cnbi::draw::Rectangle*	mrkr_;
		cnbi::draw::Rectangle*	mrkf_;
		cnbi::draw::Font*		font_;
		cnbi::draw::String*		text_;

		float rng_size_;
		float rng_thick_;
		float fix_size_;
		float fix_thick_;
		float cue_width_;
		float cue_height_;
		float fdb_length_;
		float fdb_radius_;
		float fdb_thick_;
		float mrk_width_;
		float mrk_height_;

	private:
		std::map<unsigned int, float>	map_thresholds_;
		cnbi::draw::Events*				events_;
		bool	is_space_pressed_;
		bool	is_esc_pressed_;

};

	}
}


#endif
