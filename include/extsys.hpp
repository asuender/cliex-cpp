#include "files.hpp"
#include <vector>

#ifndef _EXTSYS_HPP
#define _EXTSYS_HPP

namespace fs = std::experimental::filesystem;//TODO: put that in files.hpp please

namespace cliex::exts{
	class BaseExtension{
		public:
			fs::path fp;//!< Path to file that is assigned to this Extension
			static constexpr const char* name="BaseExtension";
			static constexpr const char* supexts[]={};//!< file extensions that can be opened by this extension
			WINDOW* win=nullptr;
			/**
			 * Sets the file path for the plugin to load
			 * Loading the file can occur here or later, when open() gets called, it doesn't matter, really
			 * @param fp Path to the File that should be opened
			 */
			void setFile(fs::path fp){
				this->fp=fp;
			}
			/**
			 * Initializes the Screen and any variables of use for this extension
			 */
			void initWin(WINDOW* win){
				this->win=win;
			}
			/**
			 * Updates the window of the extension
			 */
			void updateWin();
			/**
			 * Callback for when a key gets pressed
			 * @param key the key that got pressed
			 */
			void keyPressCallback(int key);
			/**
			 * Object initializer
			 * @param fp File path that the extension should open
			 */
			BaseExtension();
	};
	std::vector<BaseExtension*> extlist={};
}

//include extensions here
#include "../src/exts/cliap.cpp"

namespace cliex::exts{
	void initAllExtensions(){
		CLIAP cliap;
		extlist.push_back(&cliap);
	}
}
#endif
