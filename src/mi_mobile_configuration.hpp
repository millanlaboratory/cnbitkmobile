#ifndef CNBI_MI_MOBILE_CONFIGURATION_HPP
#define CNBI_MI_MOBILE_CONFIGURATION_HPP

#include <cnbicore/CcBasic.hpp>
#include <cnbiconfig/CCfgConfig.hpp>

typedef struct timing_struct {
	float wait;
	float begin;
	float end;
	float waitmin;
	float waitmax;
	float fixation;
	float cue;
	float cfeedback;
	float boom;
	float timeout;
} mitiming;

typedef struct event_struct {
	unsigned int wait;
	unsigned int fixation;
	unsigned int cfeedback;
	unsigned int off;
	unsigned int hit;
	unsigned int miss;
	unsigned int timeout;
	unsigned int device;
} mievent;


bool mi_mobile_get_timings(CCfgConfig* config, mitiming* timings) {

	try {
		config->RootEx()->QuickEx("protocol/mi/")->SetBranch();
		timings->wait		= config->BranchEx()->QuickFloatEx("scenes/wait");
		timings->begin		= config->BranchEx()->QuickFloatEx("scenes/begin");
		timings->end		= config->BranchEx()->QuickFloatEx("scenes/end");
		timings->waitmin	= config->BranchEx()->QuickFloatEx("trial/waitmin");
		timings->waitmax	= config->BranchEx()->QuickFloatEx("trial/waitmax");
		timings->fixation	= config->BranchEx()->QuickFloatEx("trial/fixation");
		timings->cue		= config->BranchEx()->QuickFloatEx("trial/cue");
		timings->cfeedback	= config->BranchEx()->QuickFloatEx("trial/cfeedback");
		timings->boom		= config->BranchEx()->QuickFloatEx("trial/boom");
		timings->timeout	= config->BranchEx()->QuickFloatEx("trial/timeout");

		return true;

	} catch(XMLException e) {
		CcLogException(e.Info());
		return false;
	}
}

#endif
