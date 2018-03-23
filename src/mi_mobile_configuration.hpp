#ifndef CNBI_MI_MOBILE_CONFIGURATION_HPP
#define CNBI_MI_MOBILE_CONFIGURATION_HPP

#include <cnbicore/CcBasic.hpp>
#include <cnbiconfig/CCfgConfig.hpp>

typedef struct {
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
} mitiming_t;

typedef struct {
	unsigned int wait;
	unsigned int fixation;
	unsigned int cfeedback;
	unsigned int off;
	unsigned int hit;
	unsigned int miss;
	unsigned int timeout;
} mievent_t;

typedef struct {
	unsigned int start;
	unsigned int stop;
	unsigned int pause;
	unsigned int quit;
} devevent_t;


bool mi_mobile_get_timings(CCfgConfig* config, mitiming_t* timings) {

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

bool mi_mobile_get_mi_events(CCfgConfig* config, mievent_t* mievents) {

	try {
		config->RootEx()->QuickEx("events/gdfevents/")->SetBranch();
		mievents->wait		= config->BranchEx()->QuickGDFIntEx("wait");
		mievents->fixation	= config->BranchEx()->QuickGDFIntEx("fixation");
		mievents->cfeedback	= config->BranchEx()->QuickGDFIntEx("cfeedback");
		mievents->off		= config->BranchEx()->QuickGDFIntEx("off");
		mievents->hit		= config->BranchEx()->QuickGDFIntEx("targethit");
		mievents->miss		= config->BranchEx()->QuickGDFIntEx("targetmiss");
		mievents->timeout	= config->BranchEx()->QuickGDFIntEx("timeout");

		return true;

	} catch(XMLException e) {
		CcLogException(e.Info());
		return false;
	}
}

bool mi_mobile_get_device_events(CCfgConfig* config, devevent_t* devevents) {

	try {
		config->RootEx()->QuickEx("events/gdfevents/")->SetBranch();
		devevents->start	= config->BranchEx()->QuickGDFIntEx("start");
		devevents->stop		= config->BranchEx()->QuickGDFIntEx("stop");
		devevents->pause	= config->BranchEx()->QuickGDFIntEx("pause");
		devevents->quit		= config->BranchEx()->QuickGDFIntEx("quit");

		return true;

	} catch(XMLException e) {
		CcLogException(e.Info());
		return false;
	}
}

bool mi_mobile_get_taskset(CCfgConfig* config, CCfgTaskset* taskset, 
						   const std::string& mode, const std::string& block) {

	try {

		config->RootEx()->QuickEx("tasksets/" + taskset->name)->SetBranch();
		taskset->description = config->BranchEx()->GetAttrEx("description");
		config->ParseTasksetEx(taskset->name, taskset);
		config->ParseConfigEx(mode, block, taskset->name, taskset);
		return true;

	} catch(XMLException e) {
		CcLogException(e.Info());
		CcLogFatal("Taskset configuration failed");
		return false;
	}

}

#endif