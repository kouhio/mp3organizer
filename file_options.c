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
#include <conio.h>
#endif

#include <dirent.h>
#include <sys/stat.h>

#ifdef __cplusplus
}
#endif

#include "libraries.h"

void file_options::add_unknown_file_data(std::string data)
{
	FILE *fd;
	static int first = 1;

#if (DEBUG == 1)
cout << __FUNCTION__ << endl;
#endif
	if(print_unknown)
	{
		if(first)
		{
			first = 0;
			remove("organizer.unknown.txt");
		}
		if((fd = fopen("organizer.unknown.txt", "at")) != NULL)
		{
			fprintf(fd, "%s\n", data.c_str());
			fclose(fd);
			handler->files.add_unknown();
		}
	}
}

void file_options::add_unknown_file_extension_data(std::string data)
{
	FILE *fd;
	static int first = 1;

#if (DEBUG == 1)
cout << __FUNCTION__ << endl;
#endif
	if(first)
	{
		first = 0;
		remove("organizer.unknown.extension.txt");
	}
	if((fd = fopen("organizer.unknown.extension.txt", "at")) != NULL)
	{
		fprintf(fd, "%s\n", data.c_str());
		fclose(fd);
		handler->files.add_unknown();
	}
}

void file_options::add_changed_data(std::string source, std::string target, int type)
{
	static int first = 1;
	
#if (DEBUG == 1)
cout << __FUNCTION__ << endl;
#endif
	if(print_output)
	{
		if(first)
		{
			first = 0;
			remove("organizer.changelog.txt");
		}
		if(!output_open)
		{
			if((fd = fopen("organizer.changelog.txt", "at")) != NULL)
			{
				output_open = 1;
			}
		}
		if((output_open) || (type >= 12))
		{
			switch(type)
			{
				case REMOVE_STRING:
				case REPLACE_STRING:
					fprintf(fd, "Renamed: %s -> %s\n", source.c_str(), target.c_str());
				break;

				case REMOVE_FILE:
				case REMOVE_FILE_WITH_STRING:
					fprintf(fd, "Removed: %s\n", source.c_str());
				break;

				case MOVE_FILE:
					fprintf(fd, "Moved: %s -> %s\n", source.c_str(), target.c_str());
				break;

				case ID3_ADDED:
					fprintf(fd, "Added ID3: %s\n", source.c_str());
				break;

				case ID3_REMOVED:
					fprintf(fd, "Removed ID3: %s\n", source.c_str());
				break;

				case FILE_READ_ONLY:
					fprintf(fd, "File: %s read only! Unable to handle ID3 functionality\n", source.c_str());
				break;
				
				case RENAME_FAILURE:
					fprintf(fd, "File: %s has renaming problems (not processed)! -> %s\n\n", source.c_str(), target.c_str());
				break;
				
				case MOVE_FAILURE:
					fprintf(fd, "File/Dir: %s has moving problem (not processed)! -> %s\n\n", source.c_str(), target.c_str());
				break;
				
				case DIR_OPEN_FAILURE:
					fprintf(fd, "Unable to process directory %s\n", source.c_str());
				break;
				
				case UNPROCESSABLE_CHARACTER:
					fprintf(fd, "Unable to add path correctly to m3u %s\n", source.c_str());
				break;
			}
		}
	}
}

void file_options::setup(void)
{
#if (DEBUG == 1)
cout << __FUNCTION__ << endl;
#endif
	print_output = 0;
	print_unknown = 1;
	reorder_cd_folders = 1;
	upper_romans = 1;
	remove_id3 = 0;
	add_id3 = 0;
	make_m3u = 0;
	m3u_open = 0;

	output_open = 0;
	mem_count = 0;
}

int file_options::parse_setup_data(std::string data)
{
	size_t pos = 0;

#if (DEBUG == 1)
cout << __FUNCTION__ << endl;
#endif
	if((pos = data.find(";")) != npos)
	{
		data.erase(pos, (data.size() - pos));
	}

	if((pos = data.find("=")) == npos)
	{
		return - 1;
	}
	pos++;

	if(data.find("print_unknown") != npos)
	{
		print_unknown = get_true_or_false(&data[pos]); 
	}
	else if(data.find("print_data") != npos)
	{
		print_output = get_true_or_false(&data[pos]);
	}
	else if(data.find("upper_romans") != npos)
	{
		upper_romans = get_true_or_false(&data[pos]);
	}
	else if(data.find("reorder_cds") != npos)
	{
		reorder_cd_folders = get_true_or_false(&data[pos]);
	}
	else if(data.find("id3_rem") != npos)
	{
		remove_id3 = get_true_or_false(&data[pos]);
	}
	else if(data.find("id3_add") != npos)
	{
		add_id3 = get_true_or_false(&data[pos]);
	}
	else if(data.find("make_m3u") != npos)
	{
		make_m3u = get_true_or_false(&data[pos]);
	}
	else
	{
		return -1;
	}
	return 0;
}

char file_options::get_reorder(void)
{
#if (DEBUG == 1)
cout << __FUNCTION__ << endl;
#endif
	return reorder_cd_folders;
}

char file_options::get_romans(void)
{
#if (DEBUG == 1)
cout << __FUNCTION__ << endl;
#endif
	return upper_romans;
}

char file_options::remove_id(void)
{
#if (DEBUG == 1)
cout << __FUNCTION__ << endl;
#endif
	return remove_id3;
}

char file_options::add_id(void)
{
#if (DEBUG == 1)
cout << __FUNCTION__ << endl;
#endif
	return add_id3;
}

int file_options::close_file(void)
{
#if (DEBUG == 1)
cout << __FUNCTION__ << endl;
#endif
	if(output_open)
	{
		fclose(fd);
		output_open = 0;
	}
	return 0;
}

std::string format_string_to_winamp_mode(std::string data)
{
	size_t pos;
	
	for(pos=0;pos<data.size();pos++)
	{
		if(data[pos] == -128)
		{
			data[pos] = -57;
		}
		else if(data[pos] == -127)
		{
			data[pos] = -4;
		}
		else if(data[pos] == -126)
		{
			data[pos] = -23;
		}
		else if(data[pos] == -125)
		{
			data[pos] = -30;
		}
		else if(data[pos] == -124)
		{
			data[pos] = -28;
		}
		else if(data[pos] == -123)
		{
			data[pos] = -32;
		}
		else if(data[pos] == -122)
		{
			data[pos] = -27;
		}
		else if(data[pos] == -121)
		{
			data[pos] = -25;
		}
		else if(data[pos] == -120)
		{
			data[pos] = -22;
		}
		else if(data[pos] == -119)
		{
			data[pos] = -21;
		}
		else if(data[pos] == -118)
		{
			data[pos] = -24;
		}
		else if(data[pos] == -117)
		{
			data[pos] = -17;
		}
		else if(data[pos] == -116)
		{
			data[pos] = -18;
		}
		else if(data[pos] == -115)
		{
			data[pos] = -20;
		}
		else if(data[pos] == -114)
		{
			data[pos] = -60;
		}
		else if(data[pos] == -113)
		{
			data[pos] = -59;
		}
		else if(data[pos] == -112)
		{
			data[pos] = -55;
		}
		else if(data[pos] == -111)
		{
			data[pos] = -26;
		}
		else if(data[pos] == -110)
		{
			data[pos] = -58;
		}
		else if(data[pos] == -109)
		{
			data[pos] = -12;
		}
		else if(data[pos] == -108)
		{
			data[pos] = -10;
		}
		else if(data[pos] == -107)
		{
			data[pos] = -14;
		}
		else if(data[pos] == -106)
		{
			data[pos] = -5;
		}
		else if(data[pos] == -105)
		{
			data[pos] = -7;
		}
		else if(data[pos] == -104)
		{
			data[pos] = -1;
		}
		else if(data[pos] == -103)
		{
			data[pos] = -42;
		}
		else if(data[pos] == -102)
		{
			data[pos] = -36;
		}
		else if(data[pos] == -101)
		{
			data[pos] = -8;
		}
		else if(data[pos] == -100)
		{
			data[pos] = -93;
		}
		else if(data[pos] == -99)
		{
			data[pos] = -40;
		}
		else if(data[pos] == -98)
		{
			data[pos] = -41;
		}
		else if(data[pos] == -97)
		{
			data[pos] = -125;
		}
		else if(data[pos] == -96)
		{
			data[pos] = -31;
		}
		else if(data[pos] == -95)
		{
			data[pos] = -19;
		}
		else if(data[pos] == -94)
		{
			data[pos] = -13;
		}
		else if(data[pos] == -93)
		{
			data[pos] = -6;
		}
		else if(data[pos] == -92)
		{
			data[pos] = -15;
		}
		else if(data[pos] == -91)
		{
			data[pos] = -47;
		}
		else if(data[pos] == -90)
		{
			data[pos] = -86;
		}
		else if(data[pos] == -89)
		{
			data[pos] = -70;
		}
		else if(data[pos] == -88)
		{
			data[pos] = -65;
		}
		else if(data[pos] == -87)
		{
			data[pos] = -82;
		}
		else if(data[pos] == -86)
		{
			data[pos] = -84;
		}
		else if(data[pos] == -85)
		{
			data[pos] = -67;
		}
		else if(data[pos] == -84)
		{
			data[pos] = -68;
		}
		else if(data[pos] == -83)
		{
			data[pos] = -95;
		}
		else if(data[pos] == -82)
		{
			data[pos] = -85;
		}
		else if(data[pos] == -81)
		{
			data[pos] = -69;
		}
		else if(data[pos] == -80)
		{
			data[pos] = -90;
		}
		else if(data[pos] == -79)
		{
			data[pos] = -90;
		}
		else if(data[pos] == -78)
		{
			data[pos] = -90;
		}
		else if(data[pos] == -77)
		{
			data[pos] = -90;
		}
		else if(data[pos] == -76)
		{
			data[pos] = -90;
		}
		else if(data[pos] == -75)
		{
			data[pos] = -63;
		}
		else if(data[pos] == -74)
		{
			data[pos] = -62;
		}
		else if(data[pos] == -73)
		{
			data[pos] = -64;
		}
		else if(data[pos] == -72)
		{
			data[pos] = -87;
		}
		else if(data[pos] == -71)
		{
			data[pos] = -90;
		}
		else if(data[pos] == -70)
		{
			data[pos] = -90;
		}
		else if(data[pos] == -69)
		{
			data[pos] = 43;
		}
		else if(data[pos] == -68)
		{
			data[pos] = 43;
		}
		else if(data[pos] == -67)
		{
			data[pos] = -94;
		}
		else if(data[pos] == -66)
		{
			data[pos] = -91;
		}
		else if(data[pos] == -65)
		{
			data[pos] = 43;
		}
		else if(data[pos] == -64)
		{
			data[pos] = 43;
		}
		else if(data[pos] == -63)
		{
			data[pos] = 45;
		}
		else if(data[pos] == -62)
		{
			data[pos] = 45;
		}
		else if(data[pos] == -61)
		{
			data[pos] = 43;
		}
		else if(data[pos] == -60)
		{
			data[pos] = 45;
		}
		else if(data[pos] == -59)
		{
			data[pos] = 43;
		}
		else if(data[pos] == -58)
		{
			data[pos] = -29;
		}
		else if(data[pos] == -57)
		{
			data[pos] = -61;
		}
		else if(data[pos] == -56)
		{
			data[pos] = 43;
		}
		else if(data[pos] == -55)
		{
			data[pos] = 43;
		}
		else if(data[pos] == -54)
		{
			data[pos] = 45;
		}
		else if(data[pos] == -53)
		{
			data[pos] = 45;
		}
		else if(data[pos] == -52)
		{
			data[pos] = -90;
		}
		else if(data[pos] == -51)
		{
			data[pos] = 45;
		}
		else if(data[pos] == -50)
		{
			data[pos] = 43;
		}
		else if(data[pos] == -49)
		{
			data[pos] = -92;
		}
		else if(data[pos] == -48)
		{
			data[pos] = -16;
		}
		else if(data[pos] == -47)
		{
			data[pos] = -48;
		}
		else if(data[pos] == -46)
		{
			data[pos] = -54;
		}
		else if(data[pos] == -45)
		{
			data[pos] = -53;
		}
		else if(data[pos] == -44)
		{
			data[pos] = -56;
		}
		else if(data[pos] == -43)
		{
			data[pos] = 105;
		}
		else if(data[pos] == -42)
		{
			data[pos] = -51;
		}
		else if(data[pos] == -41)
		{
			data[pos] = -50;
		}
		else if(data[pos] == -40)
		{
			data[pos] = -49;
		}
		else if(data[pos] == -39)
		{
			data[pos] = 43;
		}
		else if(data[pos] == -38)
		{
			data[pos] = 43;
		}
		else if(data[pos] == -37)
		{
			data[pos] = -90;
		}
		else if(data[pos] == -36)
		{
			data[pos] = 95;
		}
		else if(data[pos] == -35)
		{
			data[pos] = -90;
		}
		else if(data[pos] == -34)
		{
			data[pos] = -52;
		}
		else if(data[pos] == -33)
		{
			data[pos] = -81;
		}
		else if(data[pos] == -32)
		{
			data[pos] = -45;
		}
		else if(data[pos] == -31)
		{
			data[pos] = -33;
		}
		else if(data[pos] == -30)
		{
			data[pos] = -44;
		}
		else if(data[pos] == -29)
		{
			data[pos] = -46;
		}
		else if(data[pos] == -28)
		{
			data[pos] = -11;
		}
		else if(data[pos] == -27)
		{
			data[pos] = -43;
		}
		else if(data[pos] == -26)
		{
			data[pos] = -75;
		}
		else if(data[pos] == -25)
		{
			data[pos] = -2;
		}
		else if(data[pos] == -24)
		{
			data[pos] = -34;
		}
		else if(data[pos] == -23)
		{
			data[pos] = -38;
		}
		else if(data[pos] == -22)
		{
			data[pos] = -37;
		}
		else if(data[pos] == -21)
		{
			data[pos] = -39;
		}
		else if(data[pos] == -20)
		{
			data[pos] = -3;
		}
		else if(data[pos] == -19)
		{
			data[pos] = -35;
		}
		else if(data[pos] == -18)
		{
			data[pos] = -81;
		}
		else if(data[pos] == -17)
		{
			data[pos] = -76;
		}
		else if(data[pos] == -16)
		{
			data[pos] = -83;
		}
		else if(data[pos] == -15)
		{
			data[pos] = -79;
		}
		else if(data[pos] == -14)
		{
			data[pos] = 61;
		}
		else if(data[pos] == -13)
		{
			data[pos] = -66;
		}
		else if(data[pos] == -12)
		{
			data[pos] = -74;
		}
		else if(data[pos] == -11)
		{
			data[pos] = -89;
		}
		else if(data[pos] == -10)
		{
			data[pos] = -9;
		}
		else if(data[pos] == -9)
		{
			data[pos] = -72;
		}
		else if(data[pos] == -8)
		{
			data[pos] = -80;
		}
		else if(data[pos] == -7)
		{
			data[pos] = -88;
		}
		else if(data[pos] == -6)
		{
			data[pos] = -73;
		}
		else if(data[pos] == -5)
		{
			data[pos] = -71;
		}
		else if(data[pos] == -4)
		{
			data[pos] = -77;
		}
		else if(data[pos] == -3)
		{
			data[pos] = -78;
		}
		else if(data[pos] == -2)
		{
			data[pos] = -90;
		}
		else if(data[pos] == -1)
		{
			data[pos] = -96;
		}
		else if(((unsigned char)data[pos] >= 128) && ((unsigned char)data[pos] <= 165))
		{
			handler->options.add_changed_data(data, data, UNPROCESSABLE_CHARACTER);
		}
	}
	return data;
}

int file_options::make_m3u_file(std::string data)
{
#if (DEBUG == 1)
cout << __FUNCTION__ << endl;
#endif

	if(make_m3u)
	{
		if(!m3u_open)
		{
			if((m3u = fopen("organizer.complete_playlist.m3u", "wt")) != NULL)
			{
				m3u_open = 1;
			}
		}
		data = format_string_to_winamp_mode(data);
		if(handler->compare_audio_extension(data))
		{
			fprintf(m3u, "%s\n", data.c_str());
		}
		else if((data.find(".mp3") != npos) || (data.find(".mpc") != npos) || (data.find(".ogg") != npos))
		{
			fprintf(m3u, "%s\n", data.c_str());
		}
	}
	return 0;
}

void file_options::close_m3u(void)
{
#if (DEBUG == 1)
cout << __FUNCTION__ << endl;
#endif
	if(m3u_open)
	{
		fclose(m3u);
	}
}

