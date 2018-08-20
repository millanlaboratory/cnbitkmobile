#ifndef CNBI_MI_MOBILE_RESTING_CPP
#define CNBI_MI_MOBILE_RESTING_CPP

#include <getopt.h>
#include <cnbicore/CcTime.hpp>
#include <cnbiloop/ClLoop.hpp>
#include <cnbiloop/ClTobiId.hpp>

#include "CmWheel.hpp"
#include "CmCopilot.hpp"
#include "mi_mobile_configuration.hpp"

void usage(void) { 
	printf("Usage: mi_mobile_offline [OPTION]\n\n");
	printf("\t-a\t\taddress of the nameserver in ip:port format (default: 127.0.0.1:8123)\n");
	printf("\t-h\t\tdisplay this help and exit\n");
}

int main(int argc, char** argv) {

	// Optional argument
	int opt;
	std::string optaddress("127.0.0.1:8123");
	std::string	optcomponent("resting");

	while((opt = getopt(argc, argv, "a:")) != -1) {
		switch(opt) {
			case 'a':
				optaddress.assign(optarg);
				break;
			case '?':
				usage();
				exit(opt == 'h' ? EXIT_SUCCESS : EXIT_FAILURE);
		}
	}

	// Generic variables
	const std::string	protocol("resting_mobile_offline");
	const std::string	nmscomponent(optcomponent);
	std::string			message;

	// Tools for XML configuration
	std::string		xfile;
	std::string		xmode;
	std::string		xblock;
	std::string		xtaskset;
	CCfgConfig		config;
	CCfgTaskset*	taskset    = nullptr;
	rsttiming_t*	timings    = nullptr;
	mievent_t*		mievents   = nullptr;

	
	// Tools for TOBI interfaces
	IDMessage			idm;	
	ClTobiId*			id	= nullptr;
	IDSerializerRapid* 	ids	= nullptr;
	std::string			idpipe("/bus");
	
	// Tools for feedback
	cnbi::mobile::CmWheel*	feedback = nullptr;
	float DurationMS;
	float DurationMIN;
	CcTimeValue tic;
	
	// Initialization ClLoop
	CcCore::OpenLogger(protocol);
	CcCore::CatchSIGINT();
	CcCore::CatchSIGTERM();
	ClLoop::Configure(optaddress);

	/**** Connection to ClLoop ****/
	message = "Connecting to loop...";
	if(ClLoop::Connect() == false) {
		CcLogFatal(message + "failed.");
		goto shutdown;
	}	
	CcLogInfo(message + "done.");
	
	/**** Getting general parameters from nameserver ****/
	xfile		= ClLoop::nms.RetrieveConfig(nmscomponent, "xml");
	xmode		= ClLoop::nms.RetrieveConfig(nmscomponent, "modality");
	xblock		= ClLoop::nms.RetrieveConfig(nmscomponent, "block");
	xtaskset	= ClLoop::nms.RetrieveConfig(nmscomponent, "taskset");
	CcLogConfigS("Modality=" << xmode << ", Block=" << xblock << 
				 ", Taskset=" << xtaskset << ", Configuration=" << xfile);

	/**** Configuration from XML ****/
	message = "Importing XML configuration...";
	try {
		config.ImportFileEx(xfile);
	} catch(XMLException e) {
		CcLogException(e.Info());
		CcLogFatal(message + "failed.");
		goto shutdown;
	}
	CcLogConfig(message + "done.");
	
	/** Taskset configuration **/
	message = "XML configuration...";
	taskset = new CCfgTaskset(xtaskset);
	if(mi_mobile_get_taskset(&config, taskset, xmode, xblock) == false) {
		CcLogFatal(message + "failed.");
		goto shutdown;
	}
	CcLogConfig(message + "done.");
	
	/** Timings configuration **/
	message = "Timings XML configuration...";
	timings = new rsttiming_t;
	if(resting_mobile_get_timings(&config, timings) == false) {
		CcLogFatal(message + "failed.");
		goto shutdown;
	}
	CcLogConfig(message + "done.");
	
	/** MI events configuration **/
	message = "MI events XML configuration...";
	mievents = new mievent_t;
	if(mi_mobile_get_mi_events(&config, mievents) == false) {
		CcLogFatal(message + "failed.")
		goto shutdown;
	}
	CcLogConfig(message + "done.");

	
	/** Wheel feedback configuration **/
	message = "Feedback XML configuration...";
	feedback = new cnbi::mobile::CmWheel();
	if(resting_mobile_configure_wheel(feedback, taskset) == false) {
		CcLogFatal(message + "failed.");
		goto shutdown;
	}
	CcLogConfig(message + "done.");

	/**** Initialization TobiId ****/
	id  = new ClTobiId(ClTobiId::SetGet);
	ids = new IDSerializerRapid(&idm);
	idm.SetDescription(protocol);
	idm.SetFamilyType(IDMessage::FamilyBiosig);
	idm.SetEvent(0);

	/**** Attach Id to /bus ****/
	message = "Connecting TiD to " + idpipe + "...";
	if(id->Attach(idpipe) == false) {
		CcLogFatalS(message + "failed.");
		goto shutdown;
	}
	CcLogInfo(message + "done.");
	
	/**** Feedback starting ****/
	feedback->Start();

	CcLogInfo("Waiting for user...");
	feedback->ShowText("Press SPACE");
	while(feedback->IsStartRequested() == false) {
		if(feedback->IsQuitRequested() == true) {
			CcLogWarning("User asked to quit");
			goto shutdown;
		}
		CcTime::Sleep(100.0f);
	}
	CcLogInfo("User asked to start");
	feedback->ShowText("");
	

	CcTime::Sleep(timings->begin);

	feedback->ShowFixation();
	CcTime::Sleep(1000.0);
	
	idm.SetEvent(mievents->fixation);
	id->SetMessage(ids);
	feedback->ShowImage(0);

	DurationMS  = (timings->fixate)*60000.0f;
	DurationMIN = timings->fixate;
	
	CcLogInfoS("Resting duration: "<<DurationMIN<<" min ("<<DurationMS<< " ms)");
	CcTime::Tic(&tic);

	while(true) {

		// Interrupts if time elapsed
		if(CcTime::Toc(&tic) >= DurationMS)
			break;
	

		// Interrupts for exit
		if(ClLoop::IsConnected() == false) {
			CcLogFatal("Cannot connect to loop");
			goto shutdown;
		}
		
		if(CcCore::receivedSIGAny.Get()) {
			CcLogWarning("User asked to go down");
			goto shutdown;
		}
		
		if(feedback->IsQuitRequested() == true) {
			CcLogWarning("User asked to quit");
			goto shutdown;
		}
		CcTime::Sleep(100.0f);

	}
	idm.SetEvent(mievents->fixation + mievents->off);
	id->SetMessage(ids);

	CcLogInfoS("Resting period finished ("<<DurationMIN<<" min, "<<DurationMS<< " ms)");

	CcTime::Sleep(timings->end);

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

