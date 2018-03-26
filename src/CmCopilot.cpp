#ifndef CNBI_MOBILE_COPILOT_CPP
#define CNBI_MOBILE_COPILOT_CPP

#include "CmCopilot.hpp"

namespace cnbi {
	namespace mobile {

CmCopilot::CmCopilot(void) {}
CmCopilot::~CmCopilot(void) {}


bool CmCopilot::SetClass(unsigned int id, unsigned noccurence, float threshold) {

	std::pair<std::map<unsigned int, unsigned int>::iterator, bool> reto;
	std::pair<std::map<unsigned int, float>::iterator, bool> rett;

	// Add class occurences
	reto = this->occurences_.insert( std::pair<unsigned int, unsigned int>(id, noccurence) );

	if(reto.second == false)
		return false;

	// Add class threshold
	rett = this->thresholds_.insert( std::pair<unsigned int, float>(id, threshold) );
	
	if(rett.second == false)
		return false;

}

unsigned int CmCopilot::GetNumberTrial(void) {

	unsigned int ntrials = 0;
	for(auto it=this->occurences_.begin(); it!=this->occurences_.end(); ++it)
		ntrials += it->second;

	return ntrials;
}

unsigned int CmCopilot::GetNumberTrial(unsigned int id) {

	unsigned int ntrials = 0;
	std::map<unsigned int, unsigned int>::iterator it;

	it = this->occurences_.find(id);

	if(it != this->occurences_.end())
		ntrials = it->second;

	return ntrials;
}

float CmCopilot::GetThreshold(unsigned int id) {

	float threshold = 0.0f;
	std::map<unsigned int, float>::iterator it;

	it = this->thresholds_.find(id);

	if(it != this->thresholds_.end())
		threshold = it->second;

	return threshold;
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
