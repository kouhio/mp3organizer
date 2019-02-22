#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <cctype>
#include <cstdio>
#include <cstdlib>

#ifdef __cplusplus
extern "C" 
{
#endif

#if (OS_LINUX == 0)
#include <dos.h>
#include <dir.h>
#include <unistd.h>
#endif

#include <dirent.h>
#include <sys/stat.h>

#ifdef __cplusplus
}
#endif

#include "libraries.h"

#include "id3.h"
#include "id3_libs.h"
#include "include/field.h"
#include "include/globals.h"
#include "include/id3lib_frame.h"
#include "include/id3lib_streams.h"
#include "include/id3lib_strings.h"
#include "include/misc_support.h"
#include "include/reader.h"
#include "include/readers.h"
#include "include/sized_types.h"
#include "include/tag.h"
#include "include/utils.h"
#include "include/writer.h"
#include "include/writers.h"

char *find_tag_field_data(ID3_Tag *myTag, ID3_Frame *frame, ID3_FrameID type)
{
	ID3_Tag::Iterator *iter;

#if (DEBUG == 1)
cout << __FUNCTION__ << endl;
#endif
	iter = myTag->CreateIterator();

	while((frame = iter->GetNext()) != NULL)
	{
		if(type == frame->GetID())
		{
			//delete iter;
			return ID3_GetString(frame, ID3FN_TEXT);
		}
	}

	//delete iter;
	return (char *)"";
}

int mp3_file::add_tag(std::string data)
{
	size_t pos = 0, change = 0;
	ID3_Tag mp3_file;
	ID3_Frame frame;
	FILE *fd;
	std::string current, replacement;

#if (DEBUG == 1)
cout << __FUNCTION__ << endl;
#endif
	if((pos = data.rfind(".mp3")) != npos) {
		if((fd = fopen(data.c_str(), "ab")) == NULL) {
			handler->files.add_read_only();
			handler->options.add_changed_data(data, data, FILE_READ_ONLY);
			return 0;
		}
		fclose(fd);

		mp3_file.Link(data.c_str());
		if(mp3_file.HasV1Tag()) {
			mp3_file.Strip(ID3TT_ID3V1);
			//mp3_file.Clear();
			change |= 1;
		}

//handler->display.debug_row("IDing BAND:" + handler->file.current_file.get_band_name() + " - YEAR:" + handler->file.current_file.get_year() + " - ALBUM :" + handler->file.current_file.get_album_name());
		/*current = find_tag_field_data(&mp3_file, &frame, ID3FID_BAND);
		replacement = handler->file.current_file.get_band_name();
//handler->display.debug_row("BANDNAME: " + replacement);
		if(current != replacement)
		{
			frame.SetID(ID3FID_BAND);
			frame.GetField(ID3FN_TEXT)->Set(replacement.c_str());
			mp3_file.AddFrame(frame);
			change |= 2;
		}*/

		current = find_tag_field_data(&mp3_file, &frame, ID3FID_TRACKNUM);
		replacement = int_to_string(handler->file.current_file.get_track_number());
		//cout << "Track " << current << " and new " << replacement << endl;
		if(current != replacement) {
			//ID3_AddTrack(&mp3_file, int_to_string(handler->file.current_file.get_track_number()).c_str(),
			//						int_to_string(handler->file.current_file.get_track_count()).c_str(), true);
			frame.SetID(ID3FID_TRACKNUM);
			frame.GetField(ID3FN_TEXT)->Set(replacement.c_str());
			mp3_file.AddFrame(frame);
			change |= 32;
		}

		current = find_tag_field_data(&mp3_file, &frame, ID3FID_LEADARTIST);
		replacement = handler->file.current_file.get_band_name();
		if(current != replacement) {
			frame.SetID(ID3FID_LEADARTIST);
			frame.GetField(ID3FN_TEXT)->Set(replacement.c_str());
			mp3_file.AddFrame(frame);
			change |= 2;
		}

		current = find_tag_field_data(&mp3_file, &frame, ID3FID_TITLE);
		replacement = handler->file.current_file.get_track_name();
		if((pos = replacement.rfind(".mp3")) != npos) {
			replacement.resize(pos);
		}
		if(current != replacement) {
			frame.SetID(ID3FID_TITLE);
			frame.GetField(ID3FN_TEXT)->Set(replacement.c_str());
			mp3_file.AddFrame(frame);
			change |= 4;
		}

		current = find_tag_field_data(&mp3_file, &frame, ID3FID_ALBUM);
		replacement = handler->file.current_file.get_album_name();
		if(current != replacement) {
			frame.SetID(ID3FID_ALBUM);
			frame.GetField(ID3FN_TEXT)->Set(replacement.c_str());
			mp3_file.AddFrame(frame);
			change |= 8;
		}
	
		current = find_tag_field_data(&mp3_file, &frame, ID3FID_YEAR);
		replacement = handler->file.current_file.get_year();
		if(current != replacement) {
			frame.SetID(ID3FID_YEAR);
			frame.GetField(ID3FN_TEXT)->Set(replacement.c_str());
			mp3_file.AddFrame(frame);
			change |= 16;
		}

		if(change) {
			//if(quickRemove(data)) {
				mp3_file.SetPadding(true);
				//if(mp3_file.HasChanged()) {
				//	mp3_file.Render();
				//} 
				mp3_file.Update(ID3TT_ID3V2);
			//}
			//else return 0;
		} else return 0;
		return change;
	}
	return 0;
}

int mp3_file::quickRemove(std::string data) {
	FILE *fd;
	ID3_Tag mp3_file;

	if((fd = fopen(data.c_str(), "ab")) != NULL) {
		fclose(fd);

		mp3_file.Link(data.c_str());
		if((!mp3_file.HasV1Tag()) && (!mp3_file.HasV2Tag())) {
			return 1;
		}

		mp3_file.Strip(ID3TT_ALL);
		mp3_file.Clear();
		mp3_file.Update(ID3TT_ID3V2);
		return 1;
	}
	return 0;
}

int mp3_file::remove_all_tags(std::string data)
{
	size_t pos = 0;
	ID3_Tag mp3_file;
	FILE *fd;

#if (DEBUG == 1)
cout << __FUNCTION__ << endl;
#endif
	if((pos = data.rfind(".mp3")) != npos)
	{
		if((fd = fopen(data.c_str(), "ab")) == NULL)
		{
			handler->files.add_read_only();
			handler->options.add_changed_data(data, data, FILE_READ_ONLY);
			return 0;
		}
		fclose(fd);

		mp3_file.Link(data.c_str());
		if((!mp3_file.HasV1Tag()) && (!mp3_file.HasV2Tag()))
		{
			return 0;
		}

		mp3_file.Strip(ID3TT_ALL);
		mp3_file.Clear();
		mp3_file.Update(ID3TT_ID3V2);

		return 1;
	}
	return 0; 
}

