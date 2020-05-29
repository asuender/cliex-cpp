#include "files.hpp"
#include <vector>

namespace fs = std::experimental::filesystem;//TODO: put that in files.hpp please

#ifndef _EXTSYS_HPP
#define _EXTSYS_HPP
namespace cliex::exts{
	class BaseExtension{
		protected:
			fs::path fp;//!< Path to file that is assigned to this Extension
			static constexpr const char* supexts[]={};//!< file extensions that can be opened by this extension
			static constexpr const char* name="BaseExtension";
			int scw;//!< Screen Width
			int sch;//!< Screen Height
		public:
			/**
			 * Sets the file path for the plugin to load
			 * Loading the file can occur here or later, when open() gets called, it doesn't matter, really
			 * @param fp Path to the File that should be opened
			 */
			void setFile(fs::path fp){
				this->fp=fp;
			}
			/**
			 * Sets the screen size for the extension to use
			 * @param w Screen Width
			 * @param h Screen Height
			 */
			void setScreenSize(uint16_t w,uint16_t h){
				this->sch=h;
				this->scw=w;
			}
			/**
			 * Initializes the Screen and any variables of use for this extension
			 */
			void initScreen();
			/**
			 * Draws the frame of the extension
			 */
			void drawFrame();
			/**
			 * Callback for when a key gets pressed
			 * @param key the key that got pressed
			 */
			void keyPressCallback(char key);
			/**
			 * Object initializer
			 * @param fp File path that the extension should open
			 */
			BaseExtension();
	};
	std::vector<BaseExtension*> extlist={};
}
#endif
