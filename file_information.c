#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <cctype>
#include <cstdio>
#include <cstdlib>
#include <cstring>

#ifdef __cplusplus
extern "C" 
{
#endif

#if (OS_LINUX == 0)
#include <dos.h>
#include <dir.h>
#include <unistd.h>
#include <conio.h>
#endif

#include <dirent.h>
#include <sys/stat.h>

#ifdef __cplusplus
}
#endif

#include "libraries.h"

file_information::file_information()
{
#if (DEBUG == 1)
cout << __FUNCTION__ << endl;
#endif
	track_number = 0;
	track_count = 0;
}

file_information::~file_information()
{
#if (DEBUG == 1)
cout << __FUNCTION__ << endl;
#endif
}

void file_information::parse_album_information(std::string data)
{
	size_t pos = 0, pos2 = 0;
	int minus_count = 0, len = 0, year_found = 0;
	char temp[256];

#if (DEBUG == 1)
cout << __FUNCTION__ << endl;
#endif
	while(((pos = data.find("-", pos)) != npos) && (pos < data.size()))
	{
		minus_count++;
		pos++;
	}
	band_name.clear();
	year.clear();
	album_name.clear();

	pos = 0;
	if(minus_count == 2)
	{
		pos = data.find("-");
		pos2 = data.rfind("-");

		memset(temp, 0, sizeof(temp));
		len = data.copy(temp, pos, 0);
		band_name = remove_excessive_data_from_stream(temp);

		memset(temp, 0, sizeof(temp));
		len = data.copy(temp, pos2 - pos, pos);
		year = remove_excessive_data_from_stream(temp);

		memset(temp, 0, sizeof(temp));
		len = data.copy(temp, data.size() - pos2, pos2);
		album_name = remove_excessive_data_from_stream(temp);
		//printf("minus 2 || %s || %s || %s\n", band_name.c_str(), year.c_str(), album_name.c_str());
	}
	else if(minus_count == 1)
	{
		pos = data.find("-");
		
		memset(temp, 0, sizeof(temp));
		len = data.copy(temp, pos, 0);
		band_name = remove_excessive_data_from_stream(temp);
		
		memset(temp, 0, sizeof(temp));
		len = data.copy(temp, data.size() - pos, pos);
		album_name = remove_excessive_data_from_stream(temp);
		//printf("minus 1 || %s || %s\n", band_name.c_str(), album_name.c_str());
	}
	else if(minus_count == 0)
	{
		band_name = data;
		//printf("minus 0 || %s\n", band_name.c_str());
	}
	else
	{
		while((!isdigit(data[pos++])) && (pos < data.size()));
		if(pos >= data.size())
		{
			if((pos = data.rfind("-")) == npos)
			{
				pos = data.size();
			}
		}
		else
		{
			pos--;
		}
		memset(temp, 0, sizeof(temp));
		data.copy(temp, pos - pos2, pos2);
		len = strlen(remove_excessive_data_from_stream(temp));
		band_name = remove_excessive_data_from_stream(temp);
		if(pos < data.size())
		{
			pos2 = pos;
			memset(temp, 0, sizeof(temp));

			while((!year_found) && (pos < data.size()))
			{
				while((isdigit(data[pos++])) && (pos < data.size()));
				memset(temp, 0, sizeof(temp));
				len = data.copy(temp, pos - pos2, pos2);
				if(len == 4)
				{
					year = remove_excessive_data_from_stream(temp);
					year_found = 1;
				}
				else
				{
					while((!isdigit(data[pos++])) && (pos < data.size()));
				}
			}

			memset(temp, 0, sizeof(temp));
			pos = (pos < data.size())? pos: pos2;
			len = data.copy(temp, data.size() - pos, pos);
			album_name = remove_excessive_data_from_stream(temp);
			//printf("minus %d || %s || %s || %s\n", minus_count, band_name.c_str(), year.c_str(), album_name.c_str());
		}
	}
}


void file_information::parseSingleMiscInformation(std::string data) {
	size_t pos = 0, pos2 = 0;
	std::string new_dir, cd_value;

	album_name.clear();
	album_name = "Miscellaneous";
	//album_name = band_name;
	band_name.clear();
	track_name.clear();

	if(handler->file.get_filetype(data) == AUDIO_FILE) {
		if((track_number = get_value_from_string(data)) > 0) {
			pos = data.find("-");
			while(((data[pos] == '-') || (data[pos] == ' ')) && (pos < data.size())) {
				pos++;
			}
			pos = (pos < data.size())? pos: 0;
		}
		if((pos2 = data.find("-", pos)) != npos) {
			while(((data[pos2] == '-') || (data[pos2] == ' ')) && (pos2 < data.size())) {
				pos2++;
			}
		}
		if(pos2 < data.size()) {
			band_name = &data[pos];
			band_name.erase(pos2-1, data.size() - (pos2-1));
			track_name = &data[pos2];
			if((pos = track_name.rfind(".")) != npos) {
				track_name.erase(pos, track_name.size() - pos);
			}
		} else {
			track_name = &data[pos];
		}
	} 

	track_count++;
}

void file_information::parseSingleTrackInformation(std::string data) {
	size_t pos = 0, pos2 = 0;
	std::string new_dir, cd_value;

	album_name.clear();
	year.clear();
	track_name.clear();

	if(handler->file.get_filetype(data) == AUDIO_FILE) {
		if((track_number = get_value_from_string(data)) > 0) {
			pos = data.find("-");
			while(((data[pos] == '-') || (data[pos] == ' ')) && (pos < data.size())) {
				pos++;
			}
			pos = (pos < data.size())? pos: 0;
		}
		if((pos2 = data.find("-", pos)) != npos) {
			while(((data[pos2] == '-') || (data[pos2] == ' ')) && (pos2 < data.size())) {
				pos2++;
			}
		}
		if(pos2 < data.size()) {
			band_name = &data[pos];
			band_name.erase(pos2-1, data.size() - (pos2-1));
			track_name = &data[pos2];
			if((pos = track_name.rfind(".")) != npos) {
				track_name.erase(pos, track_name.size() - pos);
			}
		} else {
			track_name = &data[pos];
		}
	}

	track_count++;
}

void file_information::parse_track_information(std::string data)
{
	size_t pos = 0;
	std::string new_dir, cd_value;

#if (DEBUG == 1)
cout << __FUNCTION__ << endl;
#endif
	track_name.clear();

	new_dir = data;
	if(((track_number = get_value_from_string(data)) >= 100) && (handler->options.get_reorder()) &&
			(handler->file.get_current_filetype() == AUDIO_FILE) )
	{
		if(new_dir[0] != '0')
		{
			cd_value = new_dir[0];
			new_dir = handler->file.get_current_directory();
			if(new_dir[new_dir.size() - 1] == DIR_MARK)
			{
				new_dir[new_dir.size() - 1] = '\0';
			}
			data.erase(0,1);
			new_dir += " CD" + cd_value;
			handler->file.set_target_directory(new_dir);
		}
	}
	if((pos = data.find("-")) != npos)
	{
		while(((data[pos] == '-') || (data[pos] == ' ')) && (pos < data.size()))
		{
			pos++;
		}
		if(pos < data.size())
		{
			track_name = &data[pos];
		}
	}

	track_count++;
}

int file_information::get_track_number(void)
{
#if (DEBUG == 1)
cout << __FUNCTION__ << endl;
#endif
	return track_number;
}

int file_information::get_track_count(void)
{
#if (DEBUG == 1)
cout << __FUNCTION__ << endl;
#endif
	return track_count;
}

std::string file_information::get_track_name(void)
{
#if (DEBUG == 1)
cout << __FUNCTION__ << endl;
#endif
	return track_name;
}

std::string file_information::get_band_name(void)
{
#if (DEBUG == 1)
cout << __FUNCTION__ << endl;
#endif
	return band_name;
}

std::string file_information::get_album_name(void)
{
#if (DEBUG == 1)
cout << __FUNCTION__ << endl;
#endif
	return album_name;
}

std::string file_information::get_year(void)
{
#if (DEBUG == 1)
cout << __FUNCTION__ << endl;
#endif
	return year;
}

void file_information::reset_information(void)
{
#if (DEBUG == 1)
cout << __FUNCTION__ << endl;
#endif
	track_number = 0;
	track_count = 0;
}

void file_information::setOriginal(void) {
	tmp_band_name.clear();
	tmp_album_name.clear();
	tmp_track_name.clear();
	if(band_name.size() > 0) {
		tmp_band_name = &band_name[0];
	}
	if(album_name.size() > 0) {
		tmp_album_name = &album_name[0];
	}
	if(track_name.size() > 0) {
		tmp_track_name = &track_name[0];
	}
}

void file_information::resetOriginal(void) {
	band_name.clear();
	album_name.clear();
	track_name.clear();
	if(tmp_band_name.size() > 0) {
		band_name = &tmp_band_name[0];	
	}
	if(tmp_album_name.size() > 0) {
		album_name = &tmp_album_name[0];
	}
	if(tmp_track_name.size() > 0) {
		track_name = &tmp_track_name[0];
	}
}

