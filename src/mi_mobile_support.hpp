#ifndef MI_MOBILE_SUPPORT_HPP
#define MI_MOBILE_SUPPORT_HPP

#include <unordered_map>
#include <cnbicore/CcBasic.hpp>
#include <cnbiconfig/CCfgTask.hpp>
#include <cnbiconfig/CCfgConfig.hpp>

#include "MobileTypes.hpp"

struct nmsparam {
	std::string bname;
	std::string mname;
	std::string tname;
	std::string xml;
};



bool mi_controller_get_params(nmsparam* param, std::string bname = "mi") {

	CcLogInfo("Retrieving parameters from nameserver...");
	param->bname = bname;

	param->mname = ClLoop::nms.RetrieveConfig(param->bname, "modality");
	param->tname = ClLoop::nms.RetrieveConfig(param->bname, "taskset");
	param->xml   = ClLoop::nms.RetrieveConfig(param->bname, "xml");

	if(param->mname.empty() || param->tname.empty() || param->xml.empty()) {
		CcLogFatal("Parameters not found in the nameserver");
		return false;
	}

	CcLogConfigS("Modality=" << param->mname << ", Block=" << param->bname <<
				 ", Taskset=" << param->tname << ", Configuration=" << param->xml);
	return true;
}

bool mi_controller_get_xml(CCfgConfig* config, std::string xml) {

	CcLogInfo("Importing xml file...");
	try {
		config->ImportFileEx(xml);
	} catch(XMLException e) {
		CcLogException(e.Info());
		return false;
	}
	
	CcLogInfoS("xml imported");
	return true;
}

bool mi_controller_get_taskset(CCfgConfig* config, CCfgTaskset** taskset,
							   nmsparam* params) {

	CcLogInfoS("Retrieving "<< params->mname <<" taskset from xml...");
	try { 
		if(params->mname.compare("online") == 0) {
			*taskset = config->OnlineEx(params->bname, params->tname);
		} else if(params->mname.compare("offline") == 0) {
			*taskset = config->OfflineEx(params->bname, params->tname);
		} else {
			CcLogFatalS("Taskset retrivial for modality " << 
						 params->mname <<" not implemented");
			return false;
		}

	} catch(XMLException e) {
		CcLogException(e.Info());
		return false;
	}

	CcLogInfoS("Taskset '"<< params->tname <<"' retrieved from xml");
	return true;
	
}


bool mi_controller_get_commands(CCfgConfig* config, CCfgTaskset* taskset, 
								cnbi::mobile::CommandMap& cmap) {

	XMLNode node;
	CCfgTask* task;
	
	CcLogInfoS("Retrieving task/command association from xml...");
	try {
		config->RootEx()->QuickEx("controller/commands")->SetBranch();
		node = config->Leaf();

		for(XMLNode child = node->first_node(); child; child = child->next_sibling()) {
			task  = taskset->GetTaskEx(child->name());
			cmap[child->value()] = task; 

			CcLogInfoS("Command retrieved: "<<child->value()<<"|"<<child->name());
		}

	} catch(XMLException e) {
		CcLogException(e.Info());
		return false;
	}

	return true;	
}

bool mi_controller_get_streams(CCfgConfig* config, cnbi::mobile::StreamMap* smap) {

	CcLogInfoS("Retrieving TiC streams from xml...");

	XMLNode node;
	try {

		config->RootEx()->QuickEx("controller")->SetBranch();
		node = config->Leaf();
		for(XMLNode child = node->first_node("ic"); child; child = child->next_sibling("ic")) {

			smap->insert(std::make_pair<std::string, std::string>
					     (child->first_attribute("label")->value(), child->value()));

			CcLogInfoS("TiC stream retrieved for "<<
					   child->first_attribute("label")->value()<<": "<<child->value());
		}

	} catch(XMLException e) {
		CcLogException(e.Info());
		return false;
	}

	return true;	


}

#endif
