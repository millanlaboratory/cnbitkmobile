#include "CmFeedback.hpp"
#include <cnbicore/CcTime.hpp>

float sinewave(float a, float dt, float f) {
	return a*sin((2.0f*M_PI*dt*f));
}

int main(int argc, char** argv) {

	float angle;
	cnbi::mobile::CmFeedback feedback;

	feedback.Start();

	printf("[test_control] - Reset angle\n");
	feedback.Reset();
	CcTime::Sleep(500);

	printf("[test_control] - Hide fixation\n");
	feedback.Hide(cnbi::mobile::CmFeedback::Fixation);
	CcTime::Sleep(500);
	
	printf("[test_control] - Hide slice\n");
	feedback.Hide(cnbi::mobile::CmFeedback::Slice);
	CcTime::Sleep(500);
	
	printf("[test_control] - Show fixation and slice\n");
	feedback.Show(cnbi::mobile::CmFeedback::Fixation);
	feedback.Show(cnbi::mobile::CmFeedback::Slice);
	CcTime::Sleep(500);

	CcTimeValue start;

	CcTime::Tic(&start);
	while(true) {

		angle = sinewave(M_PI/2.0f, CcTime::Toc(&start)/1000.0f, 0.2f);
		feedback.Update(angle);

		if(CcTime::Toc(&start) > 6790)
			break;

		CcTime::Sleep(50);
	}

	printf("[test_control] - Reset angle\n");
	feedback.Reset();
	CcTime::Sleep(500);
	

	return 0;
}
