#include "CmEngine.hpp"
#include <cnbicore/CcTime.hpp>
#include <drawtk.h>
#include <dtk_colors.h>

int main(int argc, char** argv) {

	cnbi::mobile::CmEngine engine;
	dtk_hshape circle;

	circle = dtk_create_circle(NULL, 0.0f, 0.0f, 0.1f, 1, dtk_white, 100);

	engine.Start();
	engine.AddShape("circle", circle);

	CcTimeValue tic;

	CcTime::Tic(&tic);
	while(true) {
	
		dtk_relmove_shape(circle, 0.01f, 0.0f); 

		if(CcTime::Toc(&tic) > 5000)
			break;

		CcTime::Sleep(50);
	}


	return 0;
}
