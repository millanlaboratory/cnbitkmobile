#ifndef CNBITK_MOBILE_CONTROL_CPP
#define CNBITK_MOBILE_CONTROL_CPP

#include "CmControl.hpp"

namespace cnbi {
	namespace mobile {

CmControl::CmControl(float radius, float arc, float thick) {

	dtk_hshape outer = dtk_create_circle(NULL, 0.0f, 0.0f, radius, 1, dtk_aluminium2_med, 100);
	dtk_hshape slice = this->create_slice(NULL, 0.0f, 0.0f, radius, arc, dtk_aluminium_dark);
	dtk_hshape inner = dtk_create_circle(NULL, 0.0f, 0.0f, radius - thick, 1, dtk_black, 100);
	dtk_hshape limit = this->create_limits(NULL, radius, 0.01);
	dtk_hshape fix   = this->create_fixation(NULL, 0.0f, 0.0f, 0.2f, 0.03f, dtk_aluminium_dark);

	this->AddShape("0_outer", outer);
	this->AddShape("1_slice", slice);
	this->AddShape("2_limit", limit);
	this->AddShape("3_inner", inner);
	this->AddShape("4_fix", fix);

}

CmControl::~CmControl(void) {
	
	delete[] this->indices_;
	delete[] this->vertices_;
	delete[] this->colors_;
}

void CmControl::Reset(void) {
	this->Update(0.0f);
}

void CmControl::Update(float angle) {

	dtk_rotate_shape(this->GetShape("1_slice"), -180*angle/M_PI);
	this->angle_ = angle;
}

bool CmControl::Hide(unsigned int elem) {

	bool res = false;
	
	switch(elem) {
		case CmControl::Fixation:
			res = this->HideShape("3_fix");
			break;
		case CmControl::Slice:
			res = this->HideShape("1_slice");
			break;
		default:
			break;
	}
	this->Render();
	return res;
}

bool CmControl::Show(unsigned int elem) {
	
	bool res = false;
	
	switch(elem) {
		case CmControl::Fixation:
			res = this->ShowShape("5_fix");
			break;
		case CmControl::Slice:
			res = this->ShowShape("1_slice");
			break;
		default:
			break;
	}
	this->Render();
	return res;
}

dtk_hshape CmControl::create_slice(dtk_hshape shp, float cx, float cy, float r, float size, const float* color) {

	unsigned int numpoints;
	unsigned int numvert;
	unsigned int numind;
	unsigned int i, j;

	i = j = 0;

	numpoints = 100;
	numvert   = numpoints + 1;
	numind 	  = numpoints + 2;

	this->vertices_ = new float[numvert*2];
	this->indices_  = new unsigned int[numind];
	this->colors_   = new float[numvert*4];

	// Indices
	this->vertices_[0] 	  = cx;
	this->vertices_[1] 	  = cy;
	this->indices_[0]  	   = 0;
	this->indices_[numind-1] = 0;
	i++;

	while(i<numvert) {
		this->indices_[i] = i;
		this->vertices_[2*i]   = r*cos((M_PI/2.0f - size/2.0f) + (float)j*size/numpoints) + cx;
		this->vertices_[2*i+1] = r*sin((M_PI/2.0f - size/2.0f) + (float)j*size/numpoints) + cy;
		i++, j++;
	}

	// Colors
	for(auto c = 0; c < numvert*4; c+=4) {
		this->colors_[c  ] = color[0];
		this->colors_[c+1] = color[1];
		this->colors_[c+2] = color[2];
		this->colors_[c+3] = color[3];
	}

	
	dtk_hshape shapes[] = {
						dtk_create_complex_shape(NULL, numvert, this->vertices_, this->colors_, 
									 NULL, numind, this->indices_, DTK_TRIANGLE_FAN, NULL),
			   			   dtk_create_rectangle_2p(NULL, cx-0.005, cy, cx+0.005, cy+r, 1, dtk_aluminium2_dark)
	};

	shp = dtk_create_composite_shape(shp, sizeof(shapes)/sizeof(shapes[0]), shapes, 1);

	return shp;
}

dtk_hshape CmControl::create_fixation(dtk_hshape shp, float cx, float cy, float size, float width, const float* color) {

	dtk_hshape shapes[] = { 
						dtk_create_rectangle_2p(NULL, 
									cx - size/2.0f, cy - width/2.0f, 
					 			   	cx + size/2.0f, cy + width/2.0f,
									1, color),
						dtk_create_rectangle_2p(NULL, 
									cx - width/2.0f, cy - size/2.0f, 
					 			   	cx + width/2.0f, cy + size/2.0f,
									1, color)
	};

	shp = dtk_create_composite_shape(shp, sizeof(shapes)/sizeof(shapes[0]), shapes, 1);
	return shp;
}

dtk_hshape CmControl::create_limits(dtk_hshape shp, float r, float thick) {
	
	dtk_hshape shapes[] = {
			dtk_create_rectangle_2p(NULL, -r, -thick/2.0f, 0.0f, thick/2.0f, 1, dtk_skyblue_dark),
			dtk_create_rectangle_2p(NULL, 0.0f, -thick/2.0f, r, thick/2.0f, 1, dtk_scarletred_dark)
	};

	shp = dtk_create_composite_shape(shp, sizeof(shapes)/sizeof(shapes[0]), shapes, 1);
	return shp;
}

	}
}

#endif
