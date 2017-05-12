#ifndef CNBI_MOBILE_EXPONENTIAL_HPP
#define CNBI_MOBILE_EXPONENTIAL_HPP

#include "Integrator.hpp"

namespace cnbi {
	namespace mobile {

class Exponential : public Integrator {

	public:
		Exponential(void);
		Exponential(float alpha, float rejection);
		~Exponential(void);

		bool Apply(const Probability& in, Probability& out);

};

	}
}


#endif
