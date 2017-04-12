#include <getopt.h>

#include <cnbiloop/ClLoop.hpp>
#include <cnbiloop/ClTobiIc.hpp>
#include <cnbiloop/ClTobiId.hpp>

#include "CmFeedback.hpp"

#define CNBITK_MOBILE_HARD_LEFT 	1
#define CNBITK_MOBILE_HARD_RIGHT 	2

void usage(void) { 
	printf("Usage: mi_mobile_feedback [OPTION]\n\n");
	printf("  -n       Ic pipe name ('/ctrl6' default)\n");
	printf("  -c       Ic classifier name ('control' default)\n");
	printf("  -l       Ic class label ('standard' default)\n");
	printf("  -h       display this help and exit\n");
	CcCore::Exit(EXIT_SUCCESS);
}

int main(int argc, char** argv) {

	// Optional arguments
	int opt;
	std::string icpipe("/ctrl6");
	std::string iccname("control");
	std::string icclabel("standard");
	std::string idpipe("/bus");

	while((opt = getopt(argc, argv, "n:c:l:h")) != -1) {
		if(opt == 'n') {
			icpipe.assign(optarg);
		} else if(opt == 'c') {
			iccname.assign(optarg);
		} else if(opt == 'l') {
			icclabel.assign(optarg);
		} else {
			usage();
			CcCore::Exit(opt == 'h' ? EXIT_SUCCESS : EXIT_FAILURE);
		}
	}

	// Tools for feedback
	cnbi::mobile::CmFeedback* 	feedback;

	// Tools for TOBI interfaces
	ClTobiId*	id;
	ClTobiIc*	ic;

	IDMessage			idm;	
	IDSerializerRapid* 	ids;
	ICMessage 			icm;
	ICSerializerRapid* 	ics;
	ICClassifier* 		icc;
	ICClass*			icl;
	bool waitic;
	bool icreceived;
	int fidx;
	bool hard;

	// Initialization TobiId
	id  = new ClTobiId(ClTobiId::SetGet);
	ids = new IDSerializerRapid(&idm);
	idm.SetDescription("mi_mobile_feedback");
	idm.SetFamilyType(IDMessage::FamilyBiosig);
	idm.SetEvent(0);

	// Initialization TobiIc
	ic   = new ClTobiIc(ClTobiIc::GetOnly);
	ics  = new ICSerializerRapid(&icm);
	icc  = NULL;
	icl  = NULL;
	fidx = TCBlock::BlockIdxUnset;

	// Initialization ClLoop
	CcCore::OpenLogger("mi_mobile_feedback");
	CcCore::CatchSIGINT();
	CcCore::CatchSIGTERM();
	ClLoop::Configure();

	// Connection to ClLoop
	CcLogInfo("Connecting to loop...");
	if(ClLoop::Connect() == false) {
		CcLogFatal("Cannot connect to loop");
		goto shutdown;
	}	
	CcLogInfo("Loop connected");

	// Attach Id to /bus
	CcLogInfoS("Connecting Id to "<< idpipe <<"...");
	if(id->Attach(idpipe) == false) {
		CcLogFatalS("Cannot attach Id to "<<idpipe);
		goto shutdown;
	}
	CcLogInfo("Id connected");
	
	// Attach Ic
	CcLogInfoS("Connecting Ic to "<< icpipe << "...");
	if(ic->Attach(icpipe) == false) {
		CcLogFatalS("Cannot attach Ic to "<<icpipe);
		goto shutdown;
	}
	CcLogInfo("Ic connected");

	// Initialization feedback
	feedback = new cnbi::mobile::CmFeedback();
	feedback->Start();


	// Wait for the first Ic message
	waitic = true;
	while(waitic == true) {
		switch(ic->GetMessage(ics)) {
			case ClTobiIc::Detached:
				CcLogFatal("iC detached");
				goto shutdown;
			case ClTobiIc::HasMessage:
				CcLogInfo("iC message received");
				waitic = false;
				break;
			default:
				case ClTobiIc::NoMessage:
				break;
			}
	}
	// Verify the incoming IC message
	try {
		icc = icm.GetClassifier(iccname);
		CcLogConfigS("iC message verified: '" << iccname << "' classifier found"); 
	} catch(TCException e) {
		CcLogFatalS("Wrong iC message: '" << iccname << "' classifier missing");
		goto shutdown;
	}


	// Start main loop
	while(true) {

		hard = false;
		idm.SetEvent(781);
		id->SetMessage(ids, TCBlock::BlockIdxUnset, &fidx);
		feedback->Reset();

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
			
			/* Handbrake */
			if(ClLoop::IsConnected() == false) {
				CcLogFatal("Cannot connect to loop");
				goto shutdown;
			}
			
			if(CcCore::receivedSIGAny.Get()) {
				CcLogWarning("User asked to go down");
				goto shutdown;
			}

		}
	}



shutdown:

	// Stop feedback
	if(feedback->IsRunning()) {
		feedback->Stop();
		feedback->Join();
	}

	// Detach id
	if(id->IsAttached())
		id->Detach();
	
	// Detach ic
	if(ic->IsAttached())
		ic->Detach();

	// Free memory
	delete feedback;
	delete id;
	delete ic;
	delete ids;
	delete ics;
	delete icc;


	CcCore::Exit(EXIT_SUCCESS);
}
