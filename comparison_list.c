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

int get_comparison_type(std::string data) {
#if (DEBUG == 1)
cout << __FUNCTION__ << endl;
#endif
	data = to_lowercase(data);
	if(!data.compare(0, 7, "remove=")) {
		return REMOVE_STRING;
	}
	else if(!data.compare(0, 8, "replace=")) {
		return REPLACE_STRING;
	}
	else if(!data.compare(0, 7, "delete=")) {
		return REMOVE_FILE;
	}
	else if(!data.compare(0, 7, "accept=")) {
		return ACCEPT_FILE;
	}
	else if(!data.compare(0, 5, "move=")) {
		return MOVE_FILE;
	}
	else if(!data.compare(0, 12, "delete_text=")) {
		return REMOVE_FILE_WITH_STRING;
	}
	else if(!data.compare(0, 11, "audio_file=")) {
		return FILE_AUDIO;
	}
	else if(!data.compare(0, 11, "video_file=")) {
		return FILE_VIDEO;
	}
	else if(!data.compare(0, 5, "skip=")) {
		return SKIP_A_FILE;
	}
	else if(!data.compare(0, 9, "skip_dir=")) {
		return SKIP_DIRECTORY;
	}
	return 0;
}

comparison_list::comparison_list() {
#if (DEBUG == 1)
cout << __FUNCTION__ << endl;
#endif
	remove_string.clear();
	replace_compare.clear();
	replace_replacement.clear();
	delete_file.clear();
	accept_extension.clear();
	move_extension.clear();
	move_path.clear();
	delete_file_with_string.clear();
	audio_file.clear();
	video_file.clear();
	skip_list.clear();
	skip_directory_list.clear();
}

comparison_list::~comparison_list() {
#if (DEBUG == 1)
cout << __FUNCTION__ << endl;
#endif
	remove_string.clear();
	replace_compare.clear();
	replace_replacement.clear();
	delete_file.clear();
	accept_extension.clear();
	move_extension.clear();
	move_path.clear();
	delete_file_with_string.clear();
	audio_file.clear();
	video_file.clear();
	skip_list.clear();
	skip_directory_list.clear();
}

int comparison_list::add_item_to_list(std::string read_string) {
	size_t pos = 0, pos2 = 0;
	int new_type = 0, len = 0;
	char data[256];
	
#if (DEBUG == 1)
cout << __FUNCTION__ << endl;
#endif
	if(!(new_type = get_comparison_type(read_string))) {
		return -1;
	}
	
	if((pos = read_string.find(";")) != npos) {
		read_string.resize(pos);
	}
	else if((pos = read_string.find("\n")) != npos) {
		read_string.resize(pos);
	}

	if((pos = read_string.find("=")) == npos) {
		return -1;
	}
	pos++;
	read_string = to_lowercase(read_string);
	
	switch(new_type) {
		case REMOVE_FILE:
			if(read_string[pos] != '.') {
				read_string.insert(pos, ".");
			}
			delete_file.push_back(&read_string[pos]);
		break;
		
		case ACCEPT_FILE:
			if(read_string[pos] != '.') {
				read_string.insert(pos, ".");
			}
			accept_extension.push_back(&read_string[pos]);
		break;

		case REMOVE_STRING:
			remove_string.push_back(&read_string[pos]);
		break;
		
		case REMOVE_FILE_WITH_STRING:
			delete_file_with_string.push_back(&read_string[pos]);
		break;

		case REPLACE_STRING:
			if((pos2 = read_string.find("->")) != npos) {
				len = pos2 - pos;
				len = read_string.copy(data, len, pos);
				data[len] = '\0';
				pos2 += 2;
				replace_compare.push_back(data);
				replace_replacement.push_back(&read_string[pos2]);
			}
		break;
		
		case MOVE_FILE:
			if(read_string[pos] != '.') {
				read_string.insert(pos, ".");
			}
			if((pos2 = read_string.find("->")) != npos) {
				len = pos2 - pos;
				len = read_string.copy(data, len, pos);
				data[len] = '\0';
				pos2 += 2;
				move_extension.push_back(data);
				move_path.push_back(&read_string[pos2]);
			}
		break;

		case FILE_AUDIO:
			if(read_string[pos] != '.') {
				read_string.insert(pos, ".");
			}
			audio_file.push_back(&read_string[pos]);
		break;

		case FILE_VIDEO:
			if(read_string[pos] != '.') {
				read_string.insert(pos, ".");
			}
			video_file.push_back(&read_string[pos]);
		break;

		case SKIP_A_FILE:
			skip_list.push_back(&read_string[pos]);
		break;

		case SKIP_DIRECTORY:
			skip_directory_list.push_back(&read_string[pos]);
		break;

		default:
		return -1;
	}

	return 0;
}

int comparison_list::parse_configuration_ini(void) {
	FILE *fd;
	char data[1028];
	int i = 1;

#if (DEBUG == 1)
cout << __FUNCTION__ << endl;
#endif
	if((fd = fopen("organizer.ini", "rt")) == NULL) {
		return -1;
	}

	while(!feof(fd)) {
		fgets(data, sizeof(data), fd);
		if(feof(fd)) {
			break;
		}
		if(add_item_to_list(data)) {
			if(handler->options.parse_setup_data(data)) {
				if(data[0] != '#') {
					cout << "Problem in row " << i << ":" << data << endl; 
#if (OS_LINUX == 0)
					getch();
#endif
				}
			}
		}
		i++;
	}

	fclose(fd);

	return 0;
}

std::string comparison_list::check_for_string_removal(std::string data) {
	std::vector<std::string>::iterator vpos;
	size_t pos = 0;
	std::string temp;
	
	for(vpos = remove_string.begin(); vpos < remove_string.end(); vpos++) {
		if((pos = data.find(*vpos)) != npos) {
			temp = *vpos;
			data.erase(pos, temp.size());
		}
	}
	return data;
}

std::string comparison_list::check_for_string_replacement(std::string data) {
	std::vector<std::string>::iterator vpos, vpos2;
	size_t pos = 0;
	std::string temp;

	vpos2 = replace_replacement.begin();
	for(vpos = replace_compare.begin(); vpos < replace_compare.end(); vpos++) {
		if((pos = data.find(*vpos)) != npos) {
			temp = *vpos;
			data.replace(pos, temp.size(), *vpos2);
		}
		vpos2++;
	}
	
	return data;
}

int comparison_list::compare_extension(std::string data) {
	std::vector<std::string>::iterator vpos;
	size_t pos = 0;
	
	for(vpos = accept_extension.begin(); vpos < accept_extension.end(); vpos++) {
		if((pos = data.rfind(*vpos)) != npos) {
			return 1;
		}
	}
	
	return 0;
}

int comparison_list::check_for_file_removal(std::string data) {
	std::vector<std::string>::iterator vpos;
	size_t pos = 0;
	
	for(vpos = delete_file.begin(); vpos < delete_file.end(); vpos++) {
		if((pos = data.rfind(*vpos)) != npos) {
			return 1;
		}
	}
	
	for(vpos = delete_file_with_string.begin(); vpos < delete_file_with_string.end(); vpos++) {
		if((pos = data.rfind(*vpos)) != npos) {
			return 1;
		}
	}
	
	return 0;
}

int comparison_list::check_if_moveable(std::string data) {
	std::vector<std::string>::iterator vpos;
	size_t pos = 0;

	for(vpos = move_extension.begin(); vpos < move_extension.end(); vpos++) {
		if((pos = data.find(*vpos)) != npos) {
			return 1;
		}
	}
	return 0;
}

std::string comparison_list::get_target_directory(std::string data) {
	std::vector<std::string>::iterator vpos, vpos2;
	size_t pos = 0;

	vpos2 = move_path.begin();
	for(vpos = move_extension.begin(); vpos < move_extension.end(); vpos++) {
		if((pos = data.find(*vpos)) != npos) {
			return *vpos2;
		}
		vpos2++;
	}

	return 0;
}

int comparison_list::compare_audio_extension(std::string data) {
	std::vector<std::string>::iterator vpos;
	size_t pos = 0;

	for(vpos = audio_file.begin(); vpos < audio_file.end(); vpos++) {
		if((pos = data.find(*vpos)) != npos) {
			return 1;
		}
	}

	return 0;
}

int comparison_list::compare_video_extension(std::string data) {
	std::vector<std::string>::iterator vpos;
	size_t pos = 0;

	for(vpos = video_file.begin(); vpos < video_file.end(); vpos++) {
		if((pos = data.find(*vpos)) != npos) {
			return 1;
		}
	}

	return 0;
}

int comparison_list::compare_skip_extension(std::string data) {
	std::vector<std::string>::iterator vpos;
	size_t pos = 0;

	for(vpos = skip_list.begin(); vpos < skip_list.end(); vpos++) {
		if((pos = data.find(*vpos)) != npos) {
			return 1;
		}
	}

	return 0;
}

int comparison_list::compare_skip_directory(std::string data){
	std::vector<std::string>::iterator vpos;
	size_t pos = 0;

	for(vpos = skip_directory_list.begin(); vpos < skip_directory_list.end(); vpos++) {
		if((pos = data.find(*vpos)) != npos) {
			return 1;
		}
	}

	return 0;
}

