#ifndef CNBITK_MOBILE_CONTROL_HPP
#define CNBITK_MOBILE_CONTROL_HPP

#include <math.h>
#include <dtk_colors.h>
#include "CmEngine.hpp"

namespace cnbi {
	namespace mobile {

class CmControl : public CmEngine {

	public:
		CmControl(float radius = 0.6f, float arc = M_PI/2.0f, float thick = 0.1f);
		~CmControl(void);

		void Update(float angle);
		void Reset(void);

		bool Hide(unsigned int element);
		bool Show(unsigned int element);

	private:
		dtk_hshape create_slice(dtk_hshape shp, float cx, float cy, float r, 
												float size, const float* color);
		dtk_hshape create_fixation(dtk_hshape shp, float cx, float cy, float size, 
												   float width, const float* color);
		dtk_hshape create_limits(dtk_hshape shp, float r, float thick);

	public:
		static const unsigned int Fixation 	= 0;
		static const unsigned int Slice 	= 1;
		
	private:
		float 		angle_;

		dtk_hshape 	outer_;
		dtk_hshape  slice_;
		dtk_hshape  inner_;
		dtk_hshape  llimit_;
		dtk_hshape  rlimit_;
		dtk_hshape  fix_;
		float* 	vertices_;
		float*  colors_;
		unsigned int* indices_;
};


	}
}

#endif
