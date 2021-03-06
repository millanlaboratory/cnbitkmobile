#ifndef CNBI_MI_MOBILE_CONFIGURATION_HPP
#define CNBI_MI_MOBILE_CONFIGURATION_HPP
#include <regex>
#include <stdlib.h>

#include <cnbicore/CcBasic.hpp>
#include <cnbiconfig/CCfgConfig.hpp>
#include <cnbiprotocol/cp_utilities.hpp>
#include "CmWheel.hpp"

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
	//float device;
	float refractory;
} mitiming_t;

typedef struct {
	float begin;
	float end;
	float fixate;
} rsttiming_t;

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
	unsigned int device;
	unsigned int start;
	unsigned int stop;
	unsigned int pause;
	unsigned int quit;
	unsigned int forward;
	unsigned int backward;
	unsigned int left;
	unsigned int right;
	unsigned int reached;
	unsigned int docked;
} devevent_t;


void ExpandPath(std::string& text) {
	static std::regex env("/?\\$\\{?([^}/]+)\\}?/?");
	std::smatch match;
	while(std::regex_search(text, match, env)) {
		const char* s = getenv(match[1].str().c_str());
		const std::string var(s==NULL ? "" : s);
		text.replace(match[0].first, match[0].second, var+"/");
	}
	
}

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
		timings->refractory	= config->BranchEx()->QuickFloatEx("trial/refractory");

		return true;

	} catch(XMLException e) {
		CcLogException(e.Info());
		return false;
	}
}

bool resting_mobile_get_timings(CCfgConfig* config, rsttiming_t* timings) {

	try {
		config->RootEx()->QuickEx("protocol/resting/")->SetBranch();
		timings->begin	= config->BranchEx()->QuickFloatEx("begin");
		timings->end	= config->BranchEx()->QuickFloatEx("end");
		timings->fixate = config->BranchEx()->QuickFloatEx("fixate");

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
		devevents->device	= config->BranchEx()->QuickGDFIntEx("device");
		devevents->start	= config->BranchEx()->QuickGDFIntEx("start");
		devevents->stop		= config->BranchEx()->QuickGDFIntEx("stop");
		devevents->pause	= config->BranchEx()->QuickGDFIntEx("pause");
		devevents->quit		= config->BranchEx()->QuickGDFIntEx("quit");
		devevents->forward  = config->BranchEx()->QuickGDFIntEx("forward");
		devevents->backward = config->BranchEx()->QuickGDFIntEx("backward");
		devevents->left		= config->BranchEx()->QuickGDFIntEx("left");
		devevents->right	= config->BranchEx()->QuickGDFIntEx("right");
		devevents->reached	= config->BranchEx()->QuickGDFIntEx("reached");
		devevents->docked	= config->BranchEx()->QuickGDFIntEx("docked");

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

		if(mode.compare("online") == 0) 
			config->ParseClassifierEx(block, taskset->name, taskset, nullptr, nullptr);

		return true;

	} catch(XMLException e) {
		CcLogException(e.Info());
		CcLogFatal("Taskset configuration failed");
		return false;
	}
}

bool mi_mobile_configure_copilot(cnbi::mobile::CmCopilot* copilot, CCfgTaskset* taskset) {

	unsigned int ntrials;
	float		 threshold;
	bool		 hasconfig = true;

	for(auto its=taskset->Begin(); its!=taskset->End(); ++its) {
		
		if(its->second->HasConfig("trials") == true) {
			ntrials = (unsigned int)its->second->config["trials"].Int();
		} else {
			CcLogFatalS("Task " << its->second->gdf <<" does not have 'trials' field"); 
			hasconfig = false;
		}
		
		if(its->second->HasConfig("threshold") == true) {
			threshold = its->second->config["threshold"].Float();
		} else {
			CcLogFatalS("Task " << its->second->gdf <<" does not have 'threshold' field"); 
			hasconfig = false;
		}

		if(hasconfig == true) {
			copilot->SetClass(its->second->id, its->second->gdf, ntrials, threshold);
		} else {
			CcLogFatalS("Cannot add " << its->second->gdf <<" to the copilot"); 
		}
	}
	
	if(hasconfig == true)
		copilot->Generate();

	return hasconfig;
}

bool mi_mobile_configure_wheel(cnbi::mobile::CmWheel* wheel, CCfgTaskset* taskset) {

	bool res = true;
	std::string path;
	for(auto it=taskset->Begin(); it!=taskset->End(); ++it) {

		if(it->second->HasConfig("threshold") == true) {
			wheel->SetThreshold(it->second->id, it->second->config["threshold"].Float());
		} else {
			CcLogFatalS("Task " << it->second->gdf <<" does not have 'threshold' field"); 
			res = false;
		}

		if(it->second->HasConfig("image") == true) {
			path = it->second->config["image"].String();
			ExpandPath(path);
			wheel->SetImage(it->second->id, path);
			CcLogConfigS("Image for task "<< it->second->gdf<<": " << path);
		}

	}
	return res;
}

bool resting_mobile_configure_wheel(cnbi::mobile::CmWheel* wheel, CCfgTaskset* taskset) {

	bool res = true;
	std::string path;
	for(auto it=taskset->Begin(); it!=taskset->End(); ++it) {

		if(it->second->HasConfig("image") == true) {
			path = it->second->config["image"].String();
			ExpandPath(path);
			wheel->SetImage(it->second->id, path);
			CcLogConfigS("Image for task "<< it->second->gdf<<": " << path);
		}

	}
	return res;
}


bool mi_mobile_update_threshold(CCfgTaskset* taskset, cnbi::mobile::CmWheel* wheel) {

	bool res = false;
	float curr_threshold;

	for(auto it = taskset->Begin(); it != taskset->End(); it++) {
		auto id = it->second->id; 
		auto task = it->second->name;

		if(wheel->GetThreshold(id, &curr_threshold) == false) {
			CcLogFatalS("Cannot retrieve threshold value for class id " << id);
		} 

		if(cp_parameter_update("mi", task, "threshold", &curr_threshold)) {
			wheel->SetThreshold(id, curr_threshold);
			CcLogConfigS("Threshold changed to "<< curr_threshold<<" for mi|" << task);
			res = true;
		}
	}

	return res;
}

bool mi_mobile_update_timing(std::string label, float* time) {

	if(cp_parameter_update("mi", "time", label, time)) {
		CcLogConfigS(label << " period changed to "<< time << " [ms]");
			}
}


#endif
