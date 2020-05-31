#include "extsys.hpp"

namespace cliex::exts{
	class CLIAP: public BaseExtension{
		protected:
			static constexpr const char* supexts[]={"mp3","ogg","wav","flac","aac","opus","m4a"};//!< file extensions that can be opened by this extension
			static constexpr const char* name="CLIAP";
		public:
			CLIAP():
				BaseExtension(){}
			void initWin(WINDOW* win){
				this->win=win;
			}
	};
}
