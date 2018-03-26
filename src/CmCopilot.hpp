#ifndef CNBI_MOBILE_COPILOT_HPP
#define CNBI_MOBILE_COPILOT_HPP

#include <map>
#include <vector>
#include <algorithm>
#include <random>


namespace cnbi {
	namespace mobile {

class CmCopilot {

	public:
		CmCopilot(void);
		virtual ~CmCopilot(void);


		bool SetClass(unsigned int id, unsigned int noccurence, float threshold);

		unsigned int GetNumberTrial(void);
		unsigned int GetNumberTrial(unsigned int id);
		float GetThreshold(unsigned int id);

		bool Generate(void);

		std::vector<unsigned int>::iterator Begin(void);
		std::vector<unsigned int>::iterator End(void);
		std::vector<unsigned int>::const_iterator Begin(void) const;
		std::vector<unsigned int>::const_iterator End(void) const;

	private:
		std::map<unsigned int, unsigned int>	occurences_;
		std::map<unsigned int, float>			thresholds_;
		std::vector<unsigned int>				trials_;


};


	}
}


#endif
