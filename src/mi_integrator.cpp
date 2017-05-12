#include <cnbiloop/ClLoop.hpp>
#include <cnbiloop/ClTobiIc.hpp>
#include <cnbiloop/ClTobiId.hpp>
#include <cnbiconfig/CCfgConfig.hpp>

#include "Exponential.hpp"

bool TryAttach(ClTobiIc* ic, std::string& address, float timeout = 0.0f, float sleep = 1000.0f) {

	bool is_connected;
	CcTimeValue Tic;

	CcLogInfoS("Connecting TiC to "<< address <<" (timeout="<<timeout<<" ms) ...");
	
	CcTime::Tic(&Tic);
	while(ic->Attach(address) == false) {

		if((timeout > 0.0f) & (CcTime::Toc(&Tic) >= timeout))
			break;

		CcTime::Sleep(sleep);
	}

	if(ic->IsAttached()) {
		CcLogInfoS("TiC connected to " << address);
		is_connected = true;
	} else {
		CcLogFatalS("Cannot attach TiC to "<<address);
		is_connected = false;
	}

	return is_connected;
}

bool TryAttach(ClTobiId* id, std::string& address, float timeout = 0.0f, float sleep = 1000.0f) {

	bool is_connected;
	CcTimeValue Tic;

	CcLogInfoS("Connecting TiD to "<< address <<" (timeout="<<timeout<<" ms) ...");
	
	CcTime::Tic(&Tic);
	while(id->Attach(address) == false) {

		if((timeout > 0.0f) & (CcTime::Toc(&Tic) >= timeout))
			break;

		CcTime::Sleep(sleep);
	}

	if(id->IsAttached()) {
		CcLogInfoS("TiD connected to " << address);
		is_connected = true;
	} else {
		CcLogFatalS("Cannot attach TiD to "<<address);
		is_connected = false;
	}

	return is_connected;
}

unsigned int GetIntergratorType(CCfgConfig* config, std::string bname = "mi") {

	std::string iname, ids;
	unsigned int id;
	iname = ClLoop::nms.RetrieveConfig("mi", "integrator");	
	if(iname.empty()) {
		throw std::runtime_error("Integrator name not found in the nameserver");
	}

	try {
		config->RootEx()->QuickEx("integrators/" + iname)->SetBranch();
		ids = config->GetAttrEx("id");
		id  = std::stoul(ids);
	} catch (XMLException e) {
		throw std::runtime_error(e.Info());
	}

	CcLogConfigS("Configuring integrator as '"<< iname <<"' with id="<<id);

	return id;
}

int main(int argc, char** argv) {


	/****** Declarations *******/
	CCfgConfig 			config;
	
	ClTobiIc*			icbci;
	ClTobiIc*			icfdb;
	ICMessage			icm;
	ICSerializerRapid* 	ics;
	std::string 		addrbci = "/ctrl0";
	std::string 		addrfdb = "/ctrl1"; 

	ClTobiId*			idbus;
	IDMessage			idm;
	IDSerializerRapid*	ids;
	std::string 		addrbus = "/bus"; 
	unsigned int 		fidx = TCBlock::BlockIdxUnset;

	ICClassifierIter 	itc;	
	unsigned int 		nclasses;
	bool 				waitic = true;
	std::string 		xml;

	cnbi::mobile::Probability 	rpp, ipp;
	cnbi::mobile::Integrator* 	integrator = nullptr;
	unsigned int 	itype;


	/****** Initialization *******/
	icbci = new ClTobiIc(ClTobiIc::GetOnly);
	icfdb = new ClTobiIc(ClTobiIc::SetOnly);
	ics   = new ICSerializerRapid(&icm);

	idbus = new ClTobiId(ClTobiId::GetOnly);
	ids   = new IDSerializerRapid(&idm);

	/****** Cnbi Loop configuration ******/
	CcCore::OpenLogger("mi_integrator");
	CcCore::CatchSIGINT();
	CcCore::CatchSIGTERM();
	ClLoop::Configure();

	CcLogInfo("Connecting to loop...");
	if(ClLoop::Connect() == false) {
		CcLogFatal("Cannot connect to loop");
		goto shutdown;
	}	
	CcLogInfo("Loop connected");
	
	/****** Import cnbi xml ******/
	xml = ClLoop::nms.RetrieveConfig("mi", "xml");
	CcLogInfo("Importing xml file...");
	try {
		config.ImportFileEx(xml);
	} catch(XMLException e) {
		CcLogException(e.Info());
		goto shutdown;
	}
	CcLogInfoS("xml imported");
	
	/****** TiC/TiD connections ******/
	if(TryAttach(icbci, addrbci, 1000.0f) == false) 
		goto shutdown;
	
	if(TryAttach(icfdb, addrfdb) == false) 
		goto shutdown;
	
	if(TryAttach(idbus, addrbus) == false) 
		goto shutdown;

	/****** Wait for the first icbci message ******/
	while(waitic == true) {
		switch(icbci->GetMessage(ics)) {
			case ClTobiIc::Detached:
				CcLogFatalS("TiC '"<<addrbci<<"' detached");
				goto shutdown;
			case ClTobiIc::HasMessage:
				CcLogInfoS("Received first message on '"<<addrbci<<"'");
				waitic = false;
				break;
			default:
				break;
		}
	}

	/****** Retrieving information from icbci message ******/
	if(icm.classifiers.Size() != 1) {
		CcLogFatalS("IC message has more than one classifier");
		goto shutdown;
	}

	itc = icm.classifiers.Begin();
	nclasses = itc->second->classes.Size();
	CcLogInfoS("Classifier="<<itc->second->GetName()<<", NClasses="<<nclasses);

	/****** Initializing eigen probability vectors ******/
	rpp = Eigen::VectorXf::Zero(nclasses);
	ipp = Eigen::VectorXf::Zero(nclasses);

	rpp.fill(1.0f/nclasses);
	ipp.fill(1.0f/nclasses);

	/****** Initializing integrator ******/
	try {
		itype = GetIntergratorType(&config);
	} catch (std::runtime_error e) {
		CcLogFatalS(e.what());
		goto shutdown;
	}

	switch(itype) {
		case cnbi::mobile::Integrator::AsExponential:
			integrator = new cnbi::mobile::Exponential();
			break;
		case cnbi::mobile::Integrator::AsAlpha:
			CcLogFatal("Alpha integrator not implemented yet");
			goto shutdown;
			break;
		case cnbi::mobile::Integrator::AsGrid:
			CcLogFatal("Grid integrator not implemented yet");
			goto shutdown;
		default:
			CcLogFatal("Unknown integrator type");
			goto shutdown;
	}

	integrator->SetClasses(nclasses);
	
	if(integrator->XMLParameters(&config, "mi") == false)
		goto shutdown;

	integrator->Dump();

	/****** Main Loop ******/
	while(true) {

		/* If the icbci is detached, then exit */
		if(icbci->WaitMessage(ics) == ClTobiIc::Detached) {
			CcLogFatalS("TiC '"<<addrbci<<"' detached");
			goto shutdown;
		}

		/* If TiC fb is detached, try to attach */
		if(icfdb->IsAttached() == false) {
			goto shutdown;
		}

		/* Get continuous feedback event on idbus */
		if(idbus->GetMessage(ids) == true & idm.GetEvent() == 781) {
			fidx = idm.GetBlockIdx();
			integrator->Reset();
			CcLogInfoS("Reset event received at fidx="<<fidx);
		}
		
		/* If there is no message on icbci, then continue */
		if(icbci->WaitMessage(ics) == ClTobiIc::NoMessage) 
			continue;

		/* Export class values in eigen vector */
		itc = icm.classifiers.Begin();
		for(auto itl = itc->second->classes.Begin(); itl != itc->second->classes.End(); ++itl) {
			rpp(std::distance(itc->second->classes.Begin(), itl)) = itl->second->GetValue();
		}

		/* Apply integration */
		try {
			integrator->Apply(rpp, ipp);
		} catch (const std::runtime_error& e) {
			CcLogFatalS(e.what());
			goto shutdown;
		}

		/* Forwarding messages on icfdb */
		for(auto itl = itc->second->classes.Begin(); itl != itc->second->classes.End(); ++itl) {
			itl->second->SetValue(ipp(std::distance(itc->second->classes.Begin(), itl)));
		}

		if(icfdb->IsAttached()) {
			icfdb->SetMessage(ics, icm.GetBlockIdx());
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

shutdown:
	
	if(idbus->IsAttached())
		idbus->Detach();
	
	if(icbci->IsAttached())
		icbci->Detach();
	
	delete icbci;
	delete icfdb;
	delete ics;
	delete idbus;
	delete ids;
	delete integrator;
	
	CcCore::Exit(EXIT_SUCCESS);

}
