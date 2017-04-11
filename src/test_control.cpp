#include "CmControl.hpp"
#include <cnbicore/CcTime.hpp>

float sinewave(float a, float dt, float f) {
	return a*sin((2.0f*M_PI*dt*f));
}

int main(int argc, char** argv) {

	float angle;
	cnbi::mobile::CmControl control;

	control.Start();

	printf("[test_control] - Reset angle\n");
	control.Reset();
	CcTime::Sleep(500);

	printf("[test_control] - Hide fixation\n");
	control.Hide(cnbi::mobile::CmControl::Fixation);
	CcTime::Sleep(500);
	
	printf("[test_control] - Hide slice\n");
	control.Hide(cnbi::mobile::CmControl::Slice);
	CcTime::Sleep(500);
	
	printf("[test_control] - Show fixation and slice\n");
	control.Show(cnbi::mobile::CmControl::Fixation);
	control.Show(cnbi::mobile::CmControl::Slice);
	CcTime::Sleep(500);

	CcTimeValue start;

	CcTime::Tic(&start);
	while(true) {

		angle = sinewave(M_PI/2.0f, CcTime::Toc(&start)/1000.0f, 0.2f);
		control.Update(angle);

		if(CcTime::Toc(&start) > 6790)
			break;

		CcTime::Sleep(50);
	}

	printf("[test_control] - Reset angle\n");
	control.Reset();
	CcTime::Sleep(500);
	

	return 0;
}
