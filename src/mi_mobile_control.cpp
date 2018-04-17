#ifndef CNBI_MI_MOBILE_CONTROL_HPP
#define CNBI_MI_MOBILE_CONTROL_HPP

#include <getopt.h>
#include <sstream>
#include <iomanip>
#include <cnbicore/CcTime.hpp>
#include <cnbiloop/ClLoop.hpp>
#include <cnbiloop/ClTobiId.hpp>
#include <cnbiloop/ClTobiIc.hpp>
#include <cnbiprotocol/CpProbability.hpp>

#include "CmWheel.hpp"
#include "CmCopilot.hpp"
#include "mi_mobile_configuration.hpp"

void usage(void) { 
	printf("Usage: mi_mobile_offline [OPTION]\n\n");
	printf("\t-a\t\taddress of the nameserver in ip:port format (default: 127.0.0.1:8123)\n");
	printf("\t-c\t\tname of the component to be retrieved from nameserver. It also represents\n"
		   "\t  \t\tthe block in xml and in the TiC message (default: 'mi')\n");
	printf("\t-h\t\tdisplay this help and exit\n");
}

int main(int argc, char** argv) {
	
	// Optional argument
	int opt;
	std::string optaddress("127.0.0.1:8123");
	std::string	optcomponent("mi");

	while((opt = getopt(argc, argv, "a:c:")) != -1) {
		switch(opt) {
			case 'a':
				optaddress.assign(optarg);
				break;
			case 'c':
				optcomponent.assign(optarg);
				break;
			case '?':
				usage();
				exit(opt == 'h' ? EXIT_SUCCESS : EXIT_FAILURE);
		}
	}

	// Generic variables
	const std::string	protocol("mi_mobile_control");
	const std::string	nmscomponent(optcomponent);
	std::string			message;

	// Tools for XML configuration
	std::string		xfile;
	std::string		xmode;
	std::string		xblock;
	std::string		xtaskset;
	CCfgConfig		config;
	CCfgTaskset*	taskset    = nullptr;
	mitiming_t*		timings    = nullptr;
	mievent_t*		mievents   = nullptr;
	devtiming_t*	devtimings = nullptr;
	devevent_t*		devevents  = nullptr;

	// Tools for Copilot configuration
	cnbi::mobile::CmCopilot copilot;
	unsigned int TrialIdx;
	unsigned int TaskIdx;
	unsigned int ClassIdx;
	unsigned int nhit = 0;
	unsigned int nrej = 0;
	std::ostringstream straccuracy;
	std::ostringstream strrejection;
	
	// Tools for TOBI interfaces
	IDMessage			idm;	
	ClTobiId*			id	= nullptr;
	IDSerializerRapid* 	ids	= nullptr;
	std::string			idpipe("/bus");
	ICMessage			icm;
	ClTobiIc*			ic = nullptr;
	ICSerializerRapid*	ics = nullptr;
	ICClassifier*		icc = nullptr;
	bool waitic	= true;
	CpProbability* probs = nullptr;
	CCfgTasksetConstIt tit;
	int fidx = TCBlock::BlockIdxUnset;
	
	// Tools for feedback
	cnbi::mobile::CmWheel*	feedback = nullptr;
	float value, step;
	float fdb_update = 10.0f;
	int hitclass;
	CcTimeValue tic;
	CcTimeValue elapsed;
	bool is_timeout;
	
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
	message = "Taskset XML configuration...";
	taskset = new CCfgTaskset(xtaskset);
	if(mi_mobile_get_taskset(&config, taskset, xmode, xblock) == false) {
		CcLogFatal(message + "failed.");
		goto shutdown;
	}
	CcLogConfig(message + "done.");

	/** Timings configuration **/
	message = "Timings XML configuration...";
	timings = new mitiming_t;
	if(mi_mobile_get_timings(&config, timings) == false) {
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

	/** Device events configuration **/
	message = "Device events XML configuration...";
	devevents = new devevent_t;
	if(mi_mobile_get_device_events(&config, devevents) == false) {
		CcLogFatal(message + "failed.");
		goto shutdown;
	}
	CcLogConfig(message + "done.");
	
	/** Device timings configuration **/
	message = "Device timings XML configuration...";
	devtimings = new devtiming_t;
	if(mi_mobile_get_device_timings(&config, devtimings) == false) {
		CcLogFatal(message + "failed.");
		goto shutdown;
	}
	CcLogConfig(message + "done.");

	/** Wheel feedback configuration **/
	message = "Feedback XML configuration...";
	feedback = new cnbi::mobile::CmWheel();
	if(mi_mobile_configure_wheel(feedback, taskset) == false) {
		CcLogFatal(message + "failed.");
		goto shutdown;
	}
	CcLogConfig(message + "done.");

	/** Tobi Interfaces configuration **/
	message = "Tobi interfaces XML configuration...";
	id  = new ClTobiId(ClTobiId::SetGet);
	ids = new IDSerializerRapid(&idm);
	ic  = new ClTobiIc(ClTobiIc::GetOnly);
	ics = new ICSerializerRapid(&icm);

	idm.SetDescription(protocol);
	idm.SetFamilyType(IDMessage::FamilyBiosig);
	idm.SetEvent(0);
	CcLogConfig(message + "done.");

	printf("%s\n", taskset->ndf.id.c_str());
	/**** Attach Id ****/
	message = "Connecting TiD to " + taskset->ndf.id + "...";
	if(id->Attach(taskset->ndf.id) == false) {
		CcLogFatalS(message + "failed.");
		goto shutdown;
	}
	CcLogInfo(message + "done.");
	
	/**** Attach Ic ****/
	message = "Connecting TiC to " + taskset->ndf.ic + "...";
	if(ic->Attach(taskset->ndf.ic) == false) {
		CcLogFatalS(message + "failed.");
		goto shutdown;
	}
	CcLogInfo(message + "done.");

	/**** Feedback starting ****/
	feedback->Start();

	/**** Waiting for Ic message ****/
	CcLogInfo("Waiting for Ic message...");
	feedback->ShowText("Waiting for Ic");
	while(waitic == true) {
		switch(ic->GetMessage(ics)) {
			case ClTobiIc::Detached:
				CcLogFatal("Ic detached");
				goto shutdown;
			case ClTobiIc::HasMessage:
				waitic = false;
				break;
			default:
			case ClTobiIc::NoMessage:
				break;
		}
		CcTime::Sleep(100.0f);
	}
	CcLogInfo("Ic message received");
	feedback->ShowText("");

	/**** Verification of Ic message ****/
	try { 
		icc = icm.GetClassifier(taskset->classifier.id);
		CcLogConfigS("Ic message verified: '" << taskset->classifier.id << 
				"' classifier found"); 
	} catch(TCException e) {
		CcLogFatalS("Wrong Ic messsage: '" << taskset->classifier.id << 
				"' classifier missing");
		goto shutdown;
	}

	/**** Mapping Ic message to GDF tasks ****/
	probs = new CpProbability(&icm, taskset->classifier.id);
	for(tit = taskset->Begin(); tit != taskset->End(); tit++) {
		CCfgTask* task = tit->second;
		if(icc->classes.Has(task->gdf) == false) {
			CcLogFatalS("Taskset class " << task->gdf << " not found in iC message");
			goto shutdown;
		}
		CcLogConfigS("Mapping taskset/iC class GDF=" << task->gdf 
				<< " '" << task->description << "'" 
				<< " as " << task->id);
		probs->MapICClass(task->gdf, task->id);	
	}


	/**** Waiting for user ****/
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

	/**** Start main loop ****/
	TrialIdx = 0;
	while(true) {
		TrialIdx++;

		CcLogInfoS("Trial " << TrialIdx);
	
		// Wait
		idm.SetEvent(mievents->wait);
		id->SetMessage(ids);
		CcTime::Sleep(devtimings->wait);
		idm.SetEvent(mievents->wait + mievents->off);
		id->SetMessage(ids);

		// Continuous Feedback
		idm.SetEvent(mievents->cfeedback);
		id->SetMessage(ids, TCBlock::BlockIdxUnset, &fidx);

		// Consume all messages
		while(ic->WaitMessage(ics) == ClTobiIc::HasMessage);

		while(true) {
		
			// Wait for Ic message
			while(true) { 
				switch(ic->WaitMessage(ics)) {
					case ClTobiIc::Detached:
						CcLogFatal("Ic detached");
						goto shutdown;
						break;
					case ClTobiIc::NoMessage:
						continue;
						break;
				}
				if(icm.GetBlockIdx() > fidx) {
					CcLogDebugS("Sync iD/iC : " << fidx << "/" << icm.GetBlockIdx());
					break;
				}
			}
			// Update probability
			probs->Update(icc);

			hitclass = feedback->Update(probs->Get(1));
			if( hitclass != -1) {
				break;
			}
		}
		idm.SetEvent(mievents->cfeedback + mievents->off);
		id->SetMessage(ids);

		CcLogInfoS("Threshold reached for class "<< taskset->GetTaskEx(hitclass)->gdf);
		feedback->Hard(hitclass);
		
		// Boom
		if(hitclass == TaskIdx) {
			idm.SetEvent(mievents->hit);
			id->SetMessage(ids);
			CcTime::Sleep(timings->boom);
			idm.SetEvent(mievents->hit + mievents->off);
			id->SetMessage(ids);
			nhit++;
			CcLogInfoS("Target hit");
		} else {
			idm.SetEvent(mievents->miss);
			id->SetMessage(ids);
			CcTime::Sleep(timings->boom);
			idm.SetEvent(mievents->miss + mievents->off);
			id->SetMessage(ids);
			CcLogInfoS("Target missed");
		}
		
		// Device
		idm.SetEvent(devevents->device + copilot.GetClass(hitclass));
		id->SetMessage(ids);
		CcLogInfoS("TiD event for device ("<< taskset->GetTaskEx(hitclass)->gdf <<")");


		// Reset feedback
		feedback->Reset();
	
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
		
	}
	printf("Elapsed: %f [ms]\n", CcTime::Toc(&tic));

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
