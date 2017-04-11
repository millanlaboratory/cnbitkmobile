#ifndef CNBI_MOBILE_ENGINE_CPP
#define CNBI_MOBILE_ENGINE_CPP

#include "CmEngine.hpp"

namespace cnbi {
	namespace mobile {

CmEngine::CmEngine(void){

	this->win_ptr_ = nullptr;
	this->win_caption_ = "Mobile control";
	this->win_w_ = 800;
	this->win_h_ = 600;
	this->win_x_ = 0;
	this->win_y_ = 0;
	this->win_bpp_ = 24;

	this->win_sem_.Wait();
};

CmEngine::~CmEngine(void){
	this->Destroy();
	this->win_sem_.Post();
};

void CmEngine::SetCaption(const std::string caption) {
	this->win_caption_ = caption;
}

void CmEngine::SetSize(const unsigned int w, const unsigned int h) {
	this->win_w_ = w;
	this->win_h_ = h;
}

void CmEngine::SetPosition(const unsigned int x, const unsigned int y) {
	this->win_x_ = x;
	this->win_y_ = y;
}

void CmEngine::SetBits(const unsigned int bpp) {
	this->win_bpp_ = bpp;
}

void CmEngine::AddShape(const std::string name, dtk_hshape shp, bool overwrite) {

	if(this->Exists(name) && overwrite == true) {
		this->RemoveShape(name);
	}

	this->shp_sem_.Wait();
	this->shapes_[name] = shp;
	this->shp_sem_.Post();

}

bool CmEngine::RemoveShape(const std::string name) {

	if(this->Exists(name)) {
		this->shp_sem_.Wait();
		auto it = this->shapes_.find(name);
		dtk_destroy_shape(it->second);
		this->shapes_.erase(name);
		this->shp_sem_.Post();
		return true;
	} else {
		return false;
	}
}

bool CmEngine::Exists(const std::string name) {
	bool has;	

	this->shp_sem_.Wait();
	auto it = this->shapes_.find(name);
	has = it != this->shapes_.end();
	this->shp_sem_.Post();

	return has;
}

dtk_hshape CmEngine::GetShape(const std::string name) {

	if(this->Exists(name)) {
		auto it = this->shapes_.find(name);
		return it->second;
	} else {
		return NULL;
	}
}

bool CmEngine::HideShape(const std::string name) {

	bool res = false;
	this->colormask_[0] = 0.0f;
	this->colormask_[1] = 0.0f;
	this->colormask_[2] = 0.0f;
	this->colormask_[3] = 0.0f;

	if(this->Exists(name) == false) {
		return false;
	}

	dtk_setcolor_shape(this->GetShape(name), this->colormask_, DTK_IGNRGB);
	
	return true;
}

bool CmEngine::ShowShape(const std::string name) {

	bool res = false;
	this->colormask_[0] = 0.0f;
	this->colormask_[1] = 0.0f;
	this->colormask_[2] = 0.0f;
	this->colormask_[3] = 1.0f;

	if(this->Exists(name) == false) {
		return false;
	}

	dtk_setcolor_shape(this->GetShape(name), this->colormask_, DTK_IGNRGB);
	
	return true;
}

void CmEngine::Main(void) {
	if(!CcThread::IsRunning()) {
		return;
	}
	
	CcLogConfig("Creating OpenGL window");
	this->win_ptr_ = dtk_create_window(this->win_w_, this->win_h_, this->win_x_, 
						this->win_y_, this->win_bpp_, this->win_caption_.c_str());
	dtk_make_current_window(this->win_ptr_);
	this->win_sem_.Post();

	CcLogConfig("Starting CmEngine");
	while(CcThread::IsRunning()) {
		this->win_sem_.Wait();
		dtk_clear_screen(this->win_ptr_);
		this->Render();
		dtk_update_screen(this->win_ptr_);
		this->win_sem_.Post();
	}
	
	CcLogConfig("Stopped CmEngine");
	this->win_sem_.Wait();
	dtk_close(this->win_ptr_);
	this->win_sem_.Post();
}

void CmEngine::Render(void) {
	this->shp_sem_.Wait();
	for(auto it = this->shapes_.begin(); it != this->shapes_.end(); ++it) {
		if(it->second != nullptr)
			dtk_draw_shape(it->second);
	}
	this->shp_sem_.Post();
}

void CmEngine::Destroy(void) {

	this->shp_sem_.Wait();
	for(auto it = this->shapes_.begin(); it!=this->shapes_.end(); ++it) {
		dtk_destroy_shape(it->second);
		this->shapes_.erase(it);
	}
	this->shp_sem_.Post();
}


	}
}
#endif
