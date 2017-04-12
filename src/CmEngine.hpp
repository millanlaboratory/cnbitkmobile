#ifndef CNBI_MOBILE_ENGINE_HPP
#define CNBI_MOBILE_ENGINE_HPP

#include <drawtk.h>
#include <cnbicore/CcBasic.hpp>
#include <cnbicore/CcThread.hpp>

namespace cnbi {
	namespace mobile {

class CmEngine : public CcThread {
	
	public:
		CmEngine(void);
		~CmEngine(void);

		void SetCaption(const std::string caption);
		void SetSize(const unsigned int w, const unsigned int h);
		void SetPosition(const unsigned int x, const unsigned int y);
		void SetBits(const unsigned int bpp);

		void AddShape(const std::string name, dtk_hshape shp, bool overwrite = true);
		bool RemoveShape(const std::string name);
		dtk_hshape GetShape(const std::string name);
		bool Exists(const std::string name);

		bool HideShape(const std::string name);
		bool ShowShape(const std::string name);
		bool SetColor(const std::string name, const float* color);

	protected:
		void Main(void);
		void Render(void);
		void Destroy(void);

	private:
		dtk_hwnd		win_ptr_;
		std::string		win_caption_;
		unsigned int 	win_w_;
		unsigned int 	win_h_;
		unsigned int 	win_x_;
		unsigned int 	win_y_;
		unsigned int 	win_bpp_;

		CcSemaphore 	win_sem_;
		CcSemaphore		shp_sem_;

		std::map<std::string, dtk_hshape> 	shapes_;

		float 			colormask_[4];
		
};

	}
}


#endif
