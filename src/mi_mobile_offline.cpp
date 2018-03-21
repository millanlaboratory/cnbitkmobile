#ifndef CNBI_MI_MOBILE_OFFLINE_HPP
#define CNBI_MI_MOBILE_OFFLINE_HPP

#include <getopt.h>

#include <cnbiloop/ClLoop.hpp>
#include <cnbiloop/ClTobiId.hpp>

#include "CmFeedback.hpp"
#include "mi_mobile_configuration.hpp"

#define CNBITK_MOBILE_HARD_LEFT 	1
#define CNBITK_MOBILE_HARD_RIGHT 	2

void usage(void) { 
	printf("Usage: mi_mobile_offline [OPTION]\n\n");
	printf("  -h       display this help and exit\n");
	CcCore::Exit(EXIT_SUCCESS);
}

int main(int argc, char** argv) {

	std::string idpipe("/bus");

	// Tools for feedback
	cnbi::mobile::CmFeedback* 	feedback;

	// Tools for TOBI interfaces
	ClTobiId*	id;

	IDMessage			idm;	
	IDSerializerRapid* 	ids;
	
	// Initialization TobiId
	id  = new ClTobiId(ClTobiId::SetGet);
	ids = new IDSerializerRapid(&idm);
	idm.SetDescription("mi_mobile_offline");
	idm.SetFamilyType(IDMessage::FamilyBiosig);
	idm.SetEvent(0);

	// Initialization ClLoop
	CcCore::OpenLogger("mi_mobile_offline");
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

	// Stop feedback
	if(feedback->IsRunning()) {
		feedback->Stop();
		feedback->Join();
	}

	// Detach id
	if(id->IsAttached())
		id->Detach();
	
	// Free memory
	delete feedback;
	delete id;
	delete ids;

	CcCore::Exit(EXIT_SUCCESS);
}

#endif
