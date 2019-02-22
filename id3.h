#ifndef __ID3_FILE_HANDLERS_H__
#define __ID3_FILE_HANDLERS_H__

#include "id3_libs.h"

class mp3_file
{
	public:
		int add_tag(std::string data);
		int remove_all_tags(std::string data);
	private:
		int quickRemove(std::string data);
};

#endif


