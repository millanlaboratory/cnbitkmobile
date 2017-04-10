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

		void SetCaption(std::string caption);
		void SetSize(unsigned int w, unsigned int h);
		void SetPosition(unsigned int x, unsigned int y);
		void SetBits(unsigned int bpp);

		void Add(std::string name, dtk_hshape shp, bool overwrite = true);
		void Remove(std::string name);
		bool Exists(std::string name);

		bool Move(float input);
		bool Reset(void);

	protected:
		void Main(void);
		void Render(void);

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
		
};

	}
}


#endif
