#ifndef CNBI_MI_MOBILE_OFFLINE_HPP
#define CNBI_MI_MOBILE_OFFLINE_HPP

#include <getopt.h>

#include <cnbiloop/ClLoop.hpp>
#include <cnbiloop/ClTobiId.hpp>

#include "CmFeedback.hpp"
#include "CmCopilot.hpp"
#include "mi_mobile_configuration.hpp"

#define CNBITK_MOBILE_HARD_LEFT 	1
#define CNBITK_MOBILE_HARD_RIGHT 	2

void usage(void) { 
	printf("Usage: mi_mobile_offline [OPTION]\n\n");
	printf("  -h       display this help and exit\n");
	CcCore::Exit(EXIT_SUCCESS);
}

int main(int argc, char** argv) {
	
	// Generic variables
	const std::string	nmscomponent("mi_mobile");
	const std::string	protocol("mi_mobile_offline");
	std::string			message;

	// Tools for XML configuration
	std::string		xfile;
	std::string		xmode;
	std::string		xblock;
	std::string		xtaskset;
	CCfgConfig		config;
	CCfgTaskset*	taskset   = nullptr;
	mitiming_t*		timings   = nullptr;
	mievent_t*		mievents  = nullptr;
	devevent_t*		devevents = nullptr;

	// Tools for Copilot configuration
	cnbi::mobile::CmCopilot copilot;
	unsigned int TrialIdx;
	
	// Tools for TOBI interfaces
	IDMessage			idm;	
	ClTobiId*			id	= nullptr;
	IDSerializerRapid* 	ids	= nullptr;
	std::string			idpipe("/bus");
	
	// Tools for feedback
	cnbi::mobile::CmFeedback*	feedback = nullptr;
	
	// Initialization ClLoop
	CcCore::OpenLogger(protocol);
	CcCore::CatchSIGINT();
	CcCore::CatchSIGTERM();
	ClLoop::Configure();

	// Connection to ClLoop
	message = "Connecting to loop...";
	if(ClLoop::Connect() == false) {
		CcLogFatal(message + "failed.");
		goto shutdown;
	}	
	CcLogInfo(message + "done.");
	
	// Getting general parameters from nameserver
	xfile		= ClLoop::nms.RetrieveConfig(nmscomponent, "xml");
	xmode		= ClLoop::nms.RetrieveConfig(nmscomponent, "modality");
	xblock		= ClLoop::nms.RetrieveConfig(nmscomponent, "block");
	xtaskset	= ClLoop::nms.RetrieveConfig(nmscomponent, "taskset");
	CcLogConfigS("Modality=" << xmode << ", Block=" << xblock << 
				 ", Taskset=" << xtaskset << ", Configuration=" << xfile);

	// Importing XML configuration
	message = "Importing XML configuration...";
	try {
		config.ImportFileEx(xfile);
	} catch(XMLException e) {
		CcLogException(e.Info());
		CcLogFatal(message + "failed.");
		goto shutdown;
	}
	CcLogInfo(message + "done.");

	// Configuration from XML
	
	// Taskset configuration
	message = "Taskset XML configuration...";
	taskset = new CCfgTaskset(xtaskset);
	if(mi_mobile_get_taskset(&config, taskset, xmode, xblock) == false) {
		CcLogFatal(message + "failed.");
		goto shutdown;
	}
	CcLogInfo(message + "done.");

	// Timings configuration
	message = "Timings XML configuration...";
	timings = new mitiming_t;
	if(mi_mobile_get_timings(&config, timings) == false) {
		CcLogFatal(message + "failed.");
		goto shutdown;
	}
	CcLogInfo(message + "done.");
	
	// MI events configuration
	message = "MI events XML configuration...";
	mievents = new mievent_t;
	if(mi_mobile_get_mi_events(&config, mievents) == false) {
		CcLogFatal(message + "failed.")
		goto shutdown;
	}
	CcLogInfo(message + "done.");

	// Device events configuration
	message = "Device events XML configuration...";
	devevents = new devevent_t;
	if(mi_mobile_get_device_events(&config, devevents) == false) {
		CcLogFatal(message + "failed.");
		goto shutdown;
	}
	CcLogInfo(message + "done.");

	// Copilot configuration
	message = "Copilot XML configuration...";
	if(mi_mobile_configure_copilot(&copilot, taskset) == false) {
		CcLogFatal(message + "failed.");
		goto shutdown;
	}
	CcLogInfo(message + "done.");

	TrialIdx = 1;
	for(auto it=copilot.Begin(); it!=copilot.End(); ++it) {
		std::cout<<"Trial "<<TrialIdx<<", class="<<(*it)<<std::endl;
		TrialIdx++;
	}


	CcCore::Exit(EXIT_SUCCESS);

	
	// Initialization TobiId
	id  = new ClTobiId(ClTobiId::SetGet);
	ids = new IDSerializerRapid(&idm);
	idm.SetDescription(protocol);
	idm.SetFamilyType(IDMessage::FamilyBiosig);
	idm.SetEvent(0);


	// Attach Id to /bus
	CcLogInfoS("Connecting Id to "<< idpipe <<"...");
	if(id->Attach(idpipe) == false) {
		CcLogFatalS("Cannot attach Id to "<<idpipe);
		goto shutdown;
	}
	CcLogInfo("Id connected");
	
	// Initialization feedback
	feedback = new cnbi::mobile::CmFeedback();
	feedback->Start();

	// Start main loop
	while(true) {

		feedback->Reset();

		/*
		while(ic->WaitMessage(ics) == ClTobiIc::HasMessage) {
			
			// Check the frame idx of the new message (SYNC TO BE CHECKED)
			if(icm.GetBlockIdx() < fidx && icm.GetBlockIdx() != TCBlock::BlockIdxUnset)
				continue;

			// Update the feedback position
			feedback->Update(icm.GetValue(iccname, icclabel));
			
			while(id->GetMessage(ids) == true) {
				
				switch(idm.GetEvent()) {
					case CNBITK_MOBILE_HARD_LEFT:
						feedback->Hard(cnbi::mobile::CmFeedback::ToLeft);
						CcLogInfo("Hard decision towards Left");
						hard = true;
						break;
					case CNBITK_MOBILE_HARD_RIGHT:
						feedback->Hard(cnbi::mobile::CmFeedback::ToRight);
						CcLogInfo("Hard decision towards Right");
						hard = true;
						break;
					default:
						hard = false;
						break;
				}
			
			}
			
			if(hard == true) {
				CcTime::Sleep(1000.0f);
				feedback->Reset();
				break;
			}
			
			if(ClLoop::IsConnected() == false) {
				CcLogFatal("Cannot connect to loop");
				goto shutdown;
			}
			
			if(CcCore::receivedSIGAny.Get()) {
				CcLogWarning("User asked to go down");
				goto shutdown;
			}

		}
	*/
	}



shutdown:

	// Taskset shutdown
	if(taskset != nullptr)
		delete taskset;
	
	// Timings shutdown
	if(timings != nullptr)
		delete timings;

	// Feedback shutdown
	if (feedback != nullptr) {
		if(feedback->IsRunning()) {
			feedback->Stop();
			feedback->Join();
		}
		delete feedback;
	}

	// ClTobiId shutdown
	if(id != nullptr) {
		if(id->IsAttached())
			id->Detach();
	
		delete id;
	}

	// IDSerializer shutdown
	if(ids != nullptr)
		delete ids;

	CcCore::Exit(EXIT_SUCCESS);
}

#endif
