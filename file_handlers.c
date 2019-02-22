#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>
#include <cstring>
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
#include <stdio.h>
#include <conio.h>
#endif

#include <unistd.h>
#include <dirent.h>
#include <sys/stat.h>

#ifdef __cplusplus
}
#endif

#include "libraries.h"

file_handlers::file_handlers()
{
#if (DEBUG == 1)
cout << __FUNCTION__ << endl;
#endif
	move_to_new_folder = 0;
	current_level = 0;
	repeat = 0;
}

file_handlers::~file_handlers()
{
#if (DEBUG == 1)
cout << __FUNCTION__ << endl;
#endif
}

int id3_done_already = 0;

void file_handlers::handleID3Shit(void) {
#if (ID3 == 1)
	if(handler->options.remove_id()) {
		if(mp3.remove_all_tags(directory + DIR_MARK + filename)) {
			handler->files.add_id3_removed();
			handler->options.add_changed_data(directory + DIR_MARK + filename, filename, ID3_REMOVED);
		}
	} else if(handler->options.add_id()) {
		if(mp3.add_tag(directory + DIR_MARK + filename)) {
			handler->files.add_id3_added();
			handler->options.add_changed_data(directory + DIR_MARK + filename, filename, ID3_ADDED);
		}
	}
#endif
}

void file_handlers::get_current_path(void)
{
	char path[1028];
	size_t pos = 0;
	//int r = readlink("/proc/self/cwd", path, sizeof(path));

#if (DEBUG == 1)
cout << __FUNCTION__ << endl;
#endif
	//if(r < 0) {
		//cout << "FUCK " << path << endl;
	//} else {
		getcwd(path, sizeof(path));
		directory = path;
	//}
	
	while((pos = directory.find(DIR_REPLACE)) != npos)
	{
		directory.replace(pos, 1, DIR_REPLACEMENT);
	}
}

std::string file_handlers::get_highest_directory(std::string path)
{
	std::string temp_source = path;
	while((get_data_count(temp_source, IS_DIRECTORY) == 1) && (get_data_count(temp_source, IS_FILE) == 0))
	{
		temp_source = get_next_dir_name(temp_source);
	}
	return temp_source;
}


int file_handlers::handle_files_and_directories(std::string path)
{
	struct dirent *namelist;
	DIR *curr_directory;
	int type = 0, rcount = 0;
	std::string new_path, curr_path, temp, target_path, temp_source;

#if (DEBUG == 1)
cout << __FUNCTION__ << endl;
#endif
	if((curr_directory = opendir(path.c_str())) == NULL)
	{
		handler->options.add_changed_data(path, temp, DIR_OPEN_FAILURE);
		return -1;
	}
	curr_path = path;

	do
	{
		if((namelist = readdir(curr_directory)) != NULL)
		{
			temp = path + DIR_MARK + namelist->d_name;
			if(!(type = check_dir_or_file(temp)))
			{
				continue;
			}
			if((handler->compare_skip_directory(namelist->d_name))) {
				continue;
			}

			if(!strncmp(namelist->d_name, "organizer.", 10))
			{
				new_path = path + DIR_MARK + namelist->d_name;
				if(type == IS_DIRECTORY)
				{
					if(!get_data_count(new_path, IS_BOTH))
					{
						remove(new_path.c_str());
					}
				}
				continue;
			}
#if (OS_LINUX == 1)
			else if(!strncmp(namelist->d_name, "organizer", 9))
			{
				continue;
			}
#endif

			handler->display.update_information();
			if(type == IS_FILE)
			{
				repeat = 0;
				handler->files.add_checked();
				handler->display.update_file(namelist->d_name);
				if(!current_level) {
					current_file.parseSingleTrackInformation(namelist->d_name);
				} else if ((!strncmp(current_file.get_band_name().c_str(), "Miscellaneous", 13)) ||
							 (!strncmp(current_file.get_album_name().c_str(), "Miscellaneous", 13)) ) {
					current_file.setOriginal();
					current_file.parseSingleMiscInformation(namelist->d_name);
					filename = namelist->d_name;
					handleID3Shit();
					id3_done_already = 1;
					current_file.resetOriginal();
				} else {
					current_file.parse_track_information(namelist->d_name);
				}
				temp = handle_file(namelist->d_name);
				if(repeat) {
					do {
						temp = handle_file(temp);
					}
					while((repeat) && (rcount++ < 5));
				}
				handler->options.make_m3u_file(path + DIR_MARK + temp);
			}
			else if(type == IS_DIRECTORY)
			{
				handler->directories.add_checked();
				if(!current_level)
				{
					handler->display.update_directory(namelist->d_name);
					handler->display.update_subdirectory(" ");
				}
				else
				{
					handler->display.update_subdirectory(namelist->d_name);
				}
				new_path = path + DIR_MARK + namelist->d_name;
				add_level();
				if(handler->options.get_reorder())
				{
					if(get_data_count(directory, IS_FILE) < 99)
					{
						handler->tracks.add_items_from_path(new_path);
					}
				}
				if(!get_data_count(new_path, IS_BOTH))
				{
					temp = namelist->d_name;
					handler->display.update_last_procedure("Deleted empty directory: " + temp);
					remove(new_path.c_str());
					handler->directories.add_deleted();
					continue;
				}
				else if((get_data_count(new_path, IS_DIRECTORY) == 1) && (get_data_count(new_path, IS_FILE) == 0) && (current_level))
				{
					temp = namelist->d_name;
					temp_source = get_highest_directory(new_path);

					target_path = parse_directories_down(new_path, 1) + DIR_MARK + parse_filename(temp_source);
					handler->display.update_last_procedure("Moved directory: " + parse_filename(temp_source));
					if(parse_filename(temp_source) == parse_filename(target_path))
					{
						target_path += " org";
					}
				
					if(move_directory(temp_source, target_path))
					{
						handler->options.add_changed_data(new_path, target_path, MOVE_FAILURE);
					}
					if(!get_data_count(new_path, IS_BOTH))
					{
						remove(new_path.c_str());
						handler->display.update_last_procedure("Deleted empty directory: " + new_path);
					}
					temp_source = get_next_dir_name(new_path);
					temp_source = get_highest_directory(new_path);
					while(temp_source.size() > new_path.size())
					{
						temp_source = get_highest_directory(new_path);
						while((get_data_count(temp_source, IS_DIRECTORY) == 1) && (get_data_count(temp_source, IS_FILE) == 0))
						{
							temp_source = get_highest_directory(temp_source);
						}
						if(!get_data_count(temp_source, IS_BOTH))
						{
							remove(temp_source.c_str());
							handler->display.update_last_procedure("Deleted empty directory: " + temp_source);
						}
						else
						{
							break;
						}
					}
					
					if(!get_data_count(path, IS_BOTH))
					{
						remove(path.c_str());
						handler->display.update_last_procedure("Deleted empty directory: " + path);
					}
					current_file.parse_album_information(parse_filename(target_path));
//handler->display.debug_row("1 BAND:" + current_file.get_band_name() + " - YEAR:" + current_file.get_year() + " - ALBUM :" + current_file.get_album_name());
					temp_source = path + DIR_MARK + handle_directory(parse_filename(target_path));
					add_level();
					handle_files_and_directories(temp_source);
					decrease_level();
					directory = path;
					continue;
				}
				current_file.parse_album_information(namelist->d_name);
//handler->display.debug_row("2 BAND:" + current_file.get_band_name() + " - YEAR:" + current_file.get_year() + " - ALBUM :" + current_file.get_album_name());
				new_path = path + DIR_MARK + handle_directory(namelist->d_name);
				directory = new_path;
				handle_files_and_directories(new_path);
				if(!get_data_count(new_path, IS_BOTH))
				{
					remove(new_path.c_str());
					handler->display.update_last_procedure("Deleted empty directory: " + new_path);
				}
				decrease_level();
				directory = path;
			}
		}
	}
	while(namelist != NULL);
	
	return 0;
}

std::string file_handlers::handle_directory(std::string path)
{
	std::string temp, source, target, dirname, multiple;
	int move_dir = 0;
	size_t pos;

	if((handler->compare_skip_directory(path))) {
		return path;
	}

#if (DEBUG == 1)
cout << __FUNCTION__ << endl;
#endif
	current_file.reset_information();
	temp = path;
	dirname = path;
	directory += DIR_MARK + path;
	
	temp = to_lowercase(temp);
	
	if((pos = temp.rfind(" org")) != npos)
	{
		if(pos == (temp.size() - 4))
		{
			temp.erase(pos, 4);
		}
	}
	
	temp = remove_excess_dots(temp, extension);
	temp = handler->check_for_string_removal(temp);
	temp = handler->check_for_string_replacement(temp);

	temp = remove_underscores(temp);
	temp = check_for_commas(temp);
	temp = check_character_spacing(temp);
	temp = check_for_thes(temp);
	temp = check_for_minus_spacing(temp);

	multiple = check_for_multiple_cds(temp, directory);
	if(multiple != temp)
	{
		move_dir++;
	}

	extension.clear();

	temp = correct_string_char_sizes(temp);
	if(handler->options.get_romans())
	{
		temp = upper_romans(temp);
	}
	temp = upper_all_necessary_data(temp);
	current_file.parse_album_information(path);
//handler->display.debug_row("0 BAND:" + current_file.get_band_name() + " - YEAR:" + current_file.get_year() + " - ALBUM :" + current_file.get_album_name());
	temp = remove_excess_space(temp);

	multiple += " " + temp;
	if((move_dir) && (multiple != directory) && (current_level))
	{
		handler->options.add_changed_data(directory, multiple, MOVE_FILE);
		move_directory(directory, multiple);
		if(!get_data_count(parse_directories_down(directory, 1), IS_BOTH))
		{
			remove(parse_directories_down(directory, 1).c_str());
			handler->display.update_last_procedure("Deleted empty directory: " + parse_directories_down(directory, 1));
		}
		handler->directories.add_moved();
		handler->display.update_last_procedure("Moved directory: " + directory + " -> " + multiple);
	}
	else if(temp != dirname)
	{
		handler->options.add_changed_data(directory, temp, REMOVE_STRING);
		rename_current_directory(temp);
		handler->directories.add_renamed();
		handler->display.update_last_procedure("Renamed directory: " + path + " -> " + temp);
	}

	return temp;
}

std::string file_handlers::handle_file(std::string file)
{
	int known_file = 0, i = 0;
	size_t pos = 0, ext_pos;
	std::string temp, source, target;
	std::vector<std::string>::iterator ipos2 = handler->tracks.get_corresponting_iterator(file);

#if (DEBUG == 1)
cout << __FUNCTION__ << endl;
#endif
	temp = file;
	filename = file;

	temp = to_lowercase(temp);
	get_current_extension(temp);

	if(get_current_filetype() == SKIP_FILE) {
		return temp;
	}

	known_file = handler->compare_extension(temp);
	if((ext_pos = temp.rfind(extension)) == npos)
	{
		ext_pos = temp.size();
	}
	
	if(handler->check_for_file_removal(temp))
	{
		handler->display.update_last_procedure("Deleted file: " + filename);
		delete_current_file();
		handler->files.add_deleted();
		return temp;
	}
	
	target.clear();
	if(handler->check_if_moveable(temp))
	{
		target = handler->get_target_directory(temp);
		if(!target.empty())
		{
			move_current_file(target);
			handler->files.add_moved();
			handler->display.update_last_procedure("Moved file: " + filename);
		}
		return temp;
	}

	temp = remove_excess_dots(temp, extension);
	temp = removeEmptyParenthesis(temp);
	temp = removeDoubleValues(temp);
	temp = handler->check_for_string_removal(temp);
	for(i=0;i<2;i++)
	{
		if((!get_level()) || (get_data_count(directory, IS_DIRECTORY)))
		{
			break;
		}
		source = (!i)? current_file.get_band_name(): current_file.get_album_name();
		if((source.empty()) || (source.size() <= 2))
		{
			continue;
		}
		source = to_lowercase(source);
		if(((pos = temp.find(source)) != npos) && (pos < ext_pos))
		{
			if((temp.size() - source.size()) > (extension.size() + 5))
			{
				temp.erase(pos, source.size());
			}
		}
	}
	
	temp = handler->check_for_string_replacement(temp);
	temp = remove_underscores(temp);
	temp = check_for_commas(temp);
	temp = check_character_spacing(temp);
	temp = remove_excess_characters(temp);

	while((temp[0] == ' ') || (temp[0] == '-'))
	{
		temp.erase(0, 1);
	}

	temp = check_for_minus_spacing(temp);

	if(!known_file)
	{
		handler->options.add_unknown_file_data(directory + DIR_MARK + file);
	}

	if((current_level) && (!get_data_count(directory, IS_DIRECTORY)) && (get_current_filetype() == AUDIO_FILE))
	{
		if(get_data_count(directory, IS_FILE) < 99)
		{ 
			if(handler->options.get_reorder())
			{
				if(ipos2 < handler->tracks.get_last_comparison())
				{
					*ipos2 = temp;
				}
				temp = handler->tracks.check_filename_against_filelist(temp);
			}
			else
			{
				temp = correct_file_numbering(temp, current_file.get_track_count());
			}
		}
		else
		{
			temp = correct_file_numbering(temp, 100); //current_file.get_track_count());
		}
	}
	else if(get_current_filetype() == VIDEO_FILE)
	{
		temp = correct_file_numbering(temp, current_file.get_track_count());
	}

	temp = correct_string_char_sizes(temp);
	if(handler->options.get_romans())
	{
		temp = upper_romans(temp);
	}
	temp = upper_all_necessary_data(temp);
	temp = remove_excess_space(temp);

	if(!id3_done_already) {
		handleID3Shit();
	} else {
		id3_done_already = 0;
	}

	if((temp != filename) && (temp.find(extension) != npos))
	{
		handler->display.update_last_procedure("Renamed file: " + filename + " -> " + temp);
		if(!repeat)
		{
			handler->options.add_changed_data(filename, temp, REMOVE_STRING);
			handler->files.add_renamed();
		}
		if((move_to_new_folder) && (handler->options.get_reorder()))
		{
			if(move_and_rename_file(temp))
			{
				handler->options.add_changed_data(directory + DIR_MARK + filename, temp, RENAME_FAILURE);
			}
			move_to_new_folder = 0;
			repeat = 0;
			return filename;
		}
		else
		{
			if(rename_current_file(temp))
			{
				handler->options.add_changed_data(directory + DIR_MARK + filename, temp, RENAME_FAILURE);
				return filename;
			}
			repeat = 1;
		}
		if(ipos2 < handler->tracks.get_last_comparison())
		{
			*ipos2 = temp;
		}
	}
	else if((temp.find(extension) == npos) && (temp != filename))
	{
		handler->options.add_changed_data(directory + DIR_MARK + filename, temp, RENAME_FAILURE);
		return filename;
	}
	else if((move_to_new_folder) && (handler->options.get_reorder()))
	{
		if(move_and_rename_file(temp))
		{
			handler->options.add_changed_data(directory + DIR_MARK + filename, temp, RENAME_FAILURE);
			return filename;
		}
		move_to_new_folder = 0;
		repeat = 0;
	}
	else
	{
		repeat = 0;
	}

	return temp;
}

int file_handlers::move_and_rename_file(std::string file)
{
	std::string target, source;

#if (DEBUG == 1)
cout << __FUNCTION__ << endl;
#endif
	source = directory + DIR_MARK + filename;
    target = target_dir + DIR_MARK + file;

	if(!check_dir_or_file(target_dir))
	{
		makedir(target_dir);
	}

	return rename(source.c_str(), target.c_str());	
}

int file_handlers::delete_current_file(void)
{
	std::string file;

#if (DEBUG == 1)
cout << __FUNCTION__ << endl;
#endif
	file = directory + DIR_MARK + filename;
	handler->options.add_changed_data(file, file, REMOVE_FILE);

	return remove(file.c_str());
}

int file_handlers::rename_current_file(std::string new_filename)
{
	std::string target, source;

#if (DEBUG == 1)
cout << __FUNCTION__ << endl;
#endif
       
	source = directory + DIR_MARK + filename;
	target = directory + DIR_MARK + new_filename;

	return rename(source.c_str(), target.c_str());
}

int file_handlers::rename_current_directory(std::string new_dirname)
{
	std::string target;

#if (DEBUG == 1)
cout << __FUNCTION__ << endl;
#endif
      
	target = parse_directories_down(directory, 1) + DIR_MARK + new_dirname;
	return rename(directory.c_str(), target.c_str());
}

int file_handlers::move_current_file(std::string path)
{
	std::string source;

#if (DEBUG == 1)
cout << __FUNCTION__ << endl;
#endif
	source = directory + DIR_MARK + filename;
	if(check_dir_or_file(path) != IS_DIRECTORY)
	{
		makedir(path);
	}
	path += DIR_MARK + filename;
	
	handler->options.add_changed_data(source, path, MOVE_FILE);

	return rename(source.c_str(), path.c_str());
}

std::string file_handlers::get_current_extension(std::string data)
{
	size_t pos = data.rfind(".");

#if (DEBUG == 1)
cout << __FUNCTION__ << endl;
#endif
	if(pos != npos)
	{
		extension = &data[pos];
		if(extension.size() == data.size())
		{	
			extension.clear();
		}
		if((extension.size() <= 0) || (extension.size() > 5))
		{
			handler->options.add_unknown_file_extension_data(directory + DIR_MARK + data);
		}
		return &data[pos];
	}
	return data;
}

std::string file_handlers::get_current_extension(void)
{
#if (DEBUG == 1)
cout << __FUNCTION__ << endl;
#endif
	return extension;
}

std::string file_handlers::correct_string_char_sizes(std::string data)
{
	size_t i = 0, upper_next=1, ext_pos = 0;
	std::string final;

#if (DEBUG == 1)
cout << __FUNCTION__ << endl;
#endif
	if((ext_pos = data.rfind(extension)) == npos)
	{
		ext_pos = data.size();
	}
	else
	{
		ext_pos--;
	}

	for(i=0;i<data.size();i++)
	{
		if(isalpha(data[i]))
		{
			if((upper_next) && (i <= ext_pos))
			{
				final += char_toupper(data[i]); 
			}
			else
			{
				final += char_tolower(data[i]);
			}
		} 
		else
		{
			final += data[i];
		}
		upper_next = 0;
		if((data[i] == ' ') || (data[i] == '(') || (data[i] == '[') || (data[i] == '-'))
		{
			upper_next = 1;
		}
		else if(data[i] == '.')
		{
			if((data.compare(i, extension.size(), extension)) || (extension.size() == 0))
			{
				upper_next = 1;
			}
		}
		else if(data[i] == '\'')
		{
			if(isblank(data[i-1]))
			{
				upper_next = 1;
			}
		}
		/*else if(isdigit(data[i]))
		{
			upper_next = 1;
		}*/
	}

	return final;
}

char ROMANS[] = {'i', 'v', 'x', 'l', 'c', 'd', 'm', 0 };

std::string file_handlers::upper_romans(std::string data)
{
	size_t start=0, end = 0, pos = 0, i = 0, len = strlen(ROMANS), upper_part=0, at_end = 0;

#if (DEBUG == 1)
cout << __FUNCTION__ << endl;
#endif
	while(end < data.size())
	{
		if((end = data.find(" ", end)) == npos)
		{
			if((end = data.rfind(".")) == npos)
			{
				end = data.size();
			}
			at_end = 1;
		}
		pos = start;
		upper_part = 0;
		while((pos < end) && (pos < data.size()))
		{
			if(data[pos] == '-')
			{
				if(pos-1 > start)
				{
					end = pos;
				}
				break;
			}
			if(!isalpha(data[pos]))
			{
				upper_part = 0;
				break;
			}
			for(i=0;i<len;i++)
			{
				if(char_tolower(data[pos]) == ROMANS[i])
				{
					upper_part++;
					continue;
				}
			}
			pos++;
		}
		if(upper_part == (end - start))
		{
			for(pos=start;pos<end;pos++)
			{
				data[pos] = char_toupper(data[pos]);
			}
		}
		end++;
		start = end;
		if(at_end)
		{
			end = data.size();
		}
	}
	return data;
}

void file_handlers::set_target_directory(std::string data)
{
#if (DEBUG == 1)
cout << __FUNCTION__ << endl;
#endif
	target_dir = data;
	move_to_new_folder = 1;
}

int file_handlers::get_data_count(std::string path, int type)
{
	struct dirent *namelist;
	DIR *curr_directory;
	int d_cnt = 0, f_cnt = 0, r_type = 0;
	std::string temp;

#if (DEBUG == 1)
cout << __FUNCTION__ << endl;
#endif
	if((curr_directory = opendir(path.c_str())) == NULL)
	{
		return -1;
	}

	do
	{
		if((namelist = readdir(curr_directory)) != NULL)
		{
			temp = path + DIR_MARK + namelist->d_name;
			if((r_type = check_dir_or_file(temp)) == IS_DIRECTORY)
			{
				d_cnt++;
			}
			else if(r_type == IS_FILE)
			{
				f_cnt++;
			}
		}
	}
	while(namelist != NULL);
	closedir(curr_directory);

	if(type == IS_DIRECTORY)
	{
		return d_cnt;
	}
	else if(type == IS_FILE)
	{
		return f_cnt;
	}
	else if(IS_BOTH)
	{
		return f_cnt + d_cnt;
	}

	return -1;
}

std::string file_handlers::get_current_directory(void)
{
#if (DEBUG == 1)
cout << __FUNCTION__ << endl;
#endif
	return directory;
}

std::string file_handlers::get_current_filename(void)
{
#if (DEBUG == 1)
cout << __FUNCTION__ << endl;
#endif
	return filename;
}

int move_file(std::string source, std::string target)
{
#if (DEBUG == 1)
cout << __FUNCTION__ << endl;
#endif
	if(source == target)
	{
		return -1;
	}
	if(check_dir_or_file(parse_directories_down(target, 1)) != IS_DIRECTORY)
	{
		makedir(parse_directories_down(target, 1));
	}

	return rename(source.c_str(), target.c_str());
}

int file_handlers::get_level(void)
{
#if (DEBUG == 1)
cout << __FUNCTION__ << endl;
#endif
	return current_level;
}

void file_handlers::add_level(void)
{
#if (DEBUG == 1)
cout << __FUNCTION__ << endl;
#endif
	current_level++;
}

void file_handlers::decrease_level(void)
{
#if (DEBUG == 1)
cout << __FUNCTION__ << endl;
#endif
	current_level--;
}

int file_handlers::get_current_filetype(void)
{
	std::string tmp_extension = get_current_extension();
#if (DEBUG == 1)
cout << __FUNCTION__ << endl;
#endif
	if(handler->compare_skip_extension(tmp_extension)) {
		return SKIP_FILE;
	}
	else if(handler->compare_audio_extension(tmp_extension))
	{
		return AUDIO_FILE;
	}
	else if(handler->compare_video_extension(tmp_extension))
	{
		return VIDEO_FILE;
	}
	else if((tmp_extension.find(".mp3") != npos) || (tmp_extension.find(".mpc") != npos) || (tmp_extension.find(".ogg") != npos))
	{
		return AUDIO_FILE;
	}
	else if((tmp_extension.find(".avi") != npos) || (tmp_extension.find(".mpg") != npos) || 
			(tmp_extension.find(".mpeg") != npos) || (tmp_extension.find(".mp4")))
	{
		return VIDEO_FILE;
	}
	return UNKNOWN_FILE;
};

int file_handlers::get_filetype(std::string data)
{
	std::string temp_extension;
	size_t pos = data.rfind(".");
#if (DEBUG == 1)
cout << __FUNCTION__ << endl;
#endif

	if(pos != npos)
	{
		temp_extension = &data[pos];
		if(temp_extension.size() == data.size())
		{	
			temp_extension.clear();
		}
		if((temp_extension.size() <= 0) || (temp_extension.size() > 5))
		{
			return UNKNOWN_FILE;
		}
	}

	if(handler->compare_skip_extension(temp_extension)) {
		return SKIP_FILE;
	}
	else if(handler->compare_audio_extension(temp_extension))
	{
		return AUDIO_FILE;
	}
	else if(handler->compare_video_extension(temp_extension))
	{
		return VIDEO_FILE;
	}
	else if((temp_extension.find(".mp3") != npos) || (temp_extension.find(".mpc") != npos) || (temp_extension.find(".ogg") != npos))
	{
		return AUDIO_FILE;
	}
	else if((temp_extension.find(".avi") != npos) || (temp_extension.find(".mpg") != npos) || 
			(temp_extension.find(".mpeg") != npos) || (temp_extension.find(".mp4")))
	{
		return VIDEO_FILE;
	}
	return UNKNOWN_FILE;
};

std::string file_handlers::get_next_dir_name(std::string path)
{
	struct dirent *namelist;
	DIR *curr_directory;
	std::string temp = path;
	int r_type;

#if (DEBUG == 1)
cout << __FUNCTION__ << endl;
#endif
	if((curr_directory = opendir(path.c_str())) == NULL)
	{
		return path;
	}

	do
	{
		if((namelist = readdir(curr_directory)) != NULL)
		{
			if((r_type = check_dir_or_file(path + DIR_MARK + namelist->d_name)) == IS_DIRECTORY)
			{
				temp = path + DIR_MARK + namelist->d_name;
				break;
			}
			else if(r_type == IS_FILE)
			{
				break;
			}
		}
	}
	while(namelist != NULL);
	closedir(curr_directory);

	return temp;
}
