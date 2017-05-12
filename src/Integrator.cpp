#ifndef CNBI_MOBILE_INTEGRATOR_CPP
#define CNBI_MOBILE_INTEGRATOR_CPP

#include "Integrator.hpp"

namespace cnbi {
	namespace mobile {

Integrator::Integrator(unsigned int type) {
	this->type_ 	= type;
	this->nclasses_ = 0;
}

Integrator::~Integrator(void) {}

unsigned int Integrator::GetType(void) {
	return this->type_;
}

std::string Integrator::GetName(void) {
	return this->name_;
}

void Integrator::SetName(const std::string& name) {
	this->name_ = name;
}

bool Integrator::XMLParameters(CCfgConfig* config, const std::string bname) {

	XMLNode node;
	try {
		config->RootEx()->QuickEx("online/" + bname + "/" + this->GetName())->SetBranch();
		node = config->Leaf();

		for(XMLNode child = node->first_node("param"); child; child = child->next_sibling("param")) {
			this->Set(child->first_attribute("key")->value(), std::stof(child->value()));
		}
	} catch (XMLException e) {
		CcLogFatalS(e.Info());
		return false;
	}

	return true;
}

void Integrator::SetClasses(unsigned int n) {
	this->nclasses_ = n;
	this->prev_ = Eigen::VectorXf::Zero(n);
	this->prev_.fill(1.0f/n);
}

unsigned int Integrator::GetClasses(void) {
	return this->nclasses_;
}

void Integrator::Reset(void) {
	this->prev_.fill(1.0f/this->GetClasses());
}

bool Integrator::Set(const std::string& name, float value, bool overwrite) {

	bool result = true;	

	if(overwrite == true) {
		this->pmap_[name] = value;
	} else {
		result = this->pmap_.emplace(name, value).second;
	}

	return result;
}

bool Integrator::Get(const std::string& name, float& value) {

	bool result = false;
	std::unordered_map<std::string, float>::const_iterator it;

	it = this->pmap_.find(name);
	if(it != this->pmap_.end()) {
		value = it->second;
		result = true;
	}

	return result;
}

bool Integrator::Erase(const std::string& name) {

	bool result = false;
	std::unordered_map<std::string, float>::const_iterator it;

	it = this->pmap_.find(name);
	if(it != this->pmap_.end()) {
		this->pmap_.erase(it);
		result = true;
	}

	return result;
}

void Integrator::Dump(void) {

	std::string params;
	for(auto it = this->pmap_.begin(); it != this->pmap_.end(); ++it)
		params += it->first + "=" + std::to_string(it->second) + " ";


	CcLogInfo("Dumping integrator's configuration:");
	CcLogInfoS("Name:    "<< this->GetName());
	CcLogInfoS("Type:    "<< this->GetType());
	CcLogInfoS("Classes: "<< this->GetClasses());
	CcLogInfoS("Params:  "<< params);
}

	}
}

#endif
