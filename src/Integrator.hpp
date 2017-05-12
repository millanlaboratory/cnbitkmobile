#ifndef CNBI_MOBILE_INTEGRATOR_HPP
#define CNBI_MOBILE_INTEGRATOR_HPP

#include <string>
#include <unordered_map>
#include <Eigen/Dense>
#include <cnbiconfig/CCfgConfig.hpp>
#include <cnbiconfig/XMLException.hpp>
#include <cnbiloop/ClLoop.hpp>



namespace cnbi {
	namespace mobile {

typedef Eigen::VectorXf Probability;

class Integrator {

	protected:
		Integrator(unsigned int type);

	public:
		unsigned int GetType(void);
		std::string GetName(void);
		void SetName(const std::string& name);

		bool Set(const std::string& name, float  value, bool overwrite = true);
		bool Get(const std::string& name, float& value);
		bool Erase(const std::string& name);

		bool XMLParameters(CCfgConfig* config, const std::string bname = "mi");

		unsigned int GetClasses(void);
		void SetClasses(unsigned int n);

		virtual void Reset(void);
		virtual bool Apply(const Probability& in, Probability& out) = 0;

		virtual void Dump(void);

		virtual ~Integrator(void);
		
	public:
		static const unsigned int AsExponential = 0;
		static const unsigned int AsAlpha 		= 1;
		static const unsigned int AsGrid 		= 2;

	protected:
		Probability 	prev_;

	private:
		unsigned int 	type_;
		std::string 	name_;
		unsigned int 	nclasses_;
		std::unordered_map<std::string, float> 	pmap_;


};


	}
}
#endif
