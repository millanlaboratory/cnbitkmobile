#ifndef CNBI_MOBILE_EXPONENTIAL_CPP
#define CNBI_MOBILE_EXPONENTIAL_CPP

#include "Exponential.hpp"

namespace cnbi {
	namespace mobile {

Exponential::Exponential(void) : Integrator(Integrator::AsExponential) {
	this->SetName("exponential");
}

Exponential::Exponential(float alpha, float rejection) : Integrator(Integrator::AsExponential) {

	this->SetName("exponential");
	this->Set("rejection", rejection);
	this->Set("alpha", alpha);
}

Exponential::~Exponential(void) {};


bool Exponential::Apply(const Probability& in, Probability& out) {

	float rejection, alpha;
	bool result = false;

	/* Check if the previous probabilities have been set */
	if(this->GetClasses() == 0)
		throw std::runtime_error("Number of classes not set");

	/* Retrieving exponential parameters */
	if(this->Get("rejection", rejection) == false)
		throw std::runtime_error(this->GetName() + ": rejection parameter not set");

	if(this->Get("alpha", alpha) == false) 
		throw std::runtime_error(this->GetName() + ": alpha parameter not set");

	/* Applying rejection and smoothing */
	if(in.maxCoeff() > rejection)
		out = alpha*this->prev_ + (1.0 - alpha)*in;

	this->prev_ = out;
	result = true;

	return result;

}

	}
}

#endif
