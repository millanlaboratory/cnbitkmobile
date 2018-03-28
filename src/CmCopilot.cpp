#ifndef CNBI_MOBILE_COPILOT_CPP
#define CNBI_MOBILE_COPILOT_CPP

#include "CmCopilot.hpp"

namespace cnbi {
	namespace mobile {

CmCopilot::CmCopilot(void) {}
CmCopilot::~CmCopilot(void) {}


bool CmCopilot::SetClass(unsigned int taskid, unsigned int classid, unsigned noccurence, float threshold) {

	std::pair<std::map<unsigned int, unsigned int>::iterator, bool> reto;
	std::pair<std::map<unsigned int, unsigned int>::iterator, bool> retc;
	std::pair<std::map<unsigned int, float>::iterator, bool> rett;

	// Add class occurences
	reto = this->occurences_.insert( std::pair<unsigned int, unsigned int>(taskid, noccurence) );

	if(reto.second == false)
		return false;

	// Add class threshold
	rett = this->thresholds_.insert( std::pair<unsigned int, float>(taskid, threshold) );
	
	if(rett.second == false)
		return false;
	
	// Add class id
	retc = this->classes_.insert( std::pair<unsigned int, unsigned int>(taskid, classid) );
	
	if(retc.second == false)
		return false;

	return true;

}

unsigned int CmCopilot::GetNumberTrial(void) {

	unsigned int ntrials = 0;
	for(auto it=this->occurences_.begin(); it!=this->occurences_.end(); ++it)
		ntrials += it->second;

	return ntrials;
}

unsigned int CmCopilot::GetNumberTrial(unsigned int taskid) {

	unsigned int ntrials = 0;
	std::map<unsigned int, unsigned int>::iterator it;

	it = this->occurences_.find(taskid);

	if(it != this->occurences_.end())
		ntrials = it->second;

	return ntrials;
}

float CmCopilot::GetThreshold(unsigned int taskid) {

	float threshold = 0.0f;
	std::map<unsigned int, float>::iterator it;

	it = this->thresholds_.find(taskid);

	if(it != this->thresholds_.end())
		threshold = it->second;

	return threshold;
}

unsigned int CmCopilot::GetClass(unsigned int taskid) {

	unsigned int classid = 0;
	std::map<unsigned int, unsigned int>::iterator it;	

	it = this->classes_.find(taskid);

	if(it != this->classes_.end())
		classid = it->second;

	return classid;
}

float CmCopilot::GetStep(unsigned int taskid, float time, float update) {

	float threshold = 0.5f;
	float length, step;
	std::map<unsigned int, float>::iterator it;

	it = this->thresholds_.find(taskid);

	if(it != this->thresholds_.end())
		threshold = it->second;

	length = threshold - 0.5f;

	step = (length*update)/time;

	return step;
}

bool CmCopilot::Generate(void) {

	std::vector<unsigned int> indexes;
	std::vector<unsigned int> trials;

	// Generate the full list of trials according to the occurence per class
	for(auto ito=this->occurences_.begin(); ito!=this->occurences_.end(); ++ito) {
		for(auto i=0; i<ito->second; i++)
			trials.push_back(ito->first);
	}

	// Resize index vector
	indexes.resize(this->GetNumberTrial());	

	/*** Create an index vector ***/
	std::iota(indexes.begin(), indexes.end(), 0);

	/*** Shuffle the order of the index vector ***/
	std::random_device rd;
	std::mt19937 g(rd());
	std::shuffle(indexes.begin(), indexes.end(), g);

	for(auto itt=indexes.begin(); itt!=indexes.end(); ++itt)
		this->trials_.push_back(trials.at((*itt)));
}

std::vector<unsigned int>::iterator CmCopilot::Begin(void) {
	return this->trials_.begin();
}

std::vector<unsigned int>::iterator CmCopilot::End(void) {
	return this->trials_.end();
}

std::vector<unsigned int>::const_iterator CmCopilot::Begin(void) const {
	return this->trials_.begin();
}

std::vector<unsigned int>::const_iterator CmCopilot::End(void) const {
	return this->trials_.end();
}


	}
}

#endif
