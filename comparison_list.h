#ifndef __COMPARISON_LIST_H__
#define __COMPARISON_LIST_H__

#include "conversion_counter.h"
#include "comparison_list.h"
#include "file_handlers.h"
#include "file_options.h"
#include "display.h"
#include "track_list.h"
#include "time_functions.h"

#include <string>
#include <vector>

enum {
	REMOVE_STRING = 1,
	REPLACE_STRING,
	REMOVE_FILE,
	ACCEPT_FILE,
	MOVE_FILE,
	REMOVE_FILE_WITH_STRING,
	ID3_ADDED,
	ID3_REMOVED,
	FILE_READ_ONLY,
	FILE_AUDIO,
	FILE_VIDEO,
	RENAME_FAILURE,
	MOVE_FAILURE,
	DIR_OPEN_FAILURE,
	UNPROCESSABLE_CHARACTER,
	SKIP_A_FILE,
	SKIP_DIRECTORY,
};

class comparison_list
{
	public:
		comparison_list();
		~comparison_list();

		int parse_configuration_ini(void);
		int add_item_to_list(std::string read_string);
		
		std::string check_for_string_removal(std::string data);
		std::string check_for_string_replacement(std::string data);
		int compare_extension(std::string data);
		int check_for_file_removal(std::string data);
		std::string get_target_directory(std::string data);
		int check_if_moveable(std::string data);

		int compare_audio_extension(std::string data);
		int compare_video_extension(std::string data);
		int compare_skip_extension(std::string data);
		int compare_skip_directory(std::string data);

		conversion_counter files;
		conversion_counter directories;
		file_options options;
		file_handlers file;
		display_actions display;
		directory_tracklist tracks;
		timer_functions timer;
		
	private:
		std::vector<std::string> remove_string;
		std::vector<std::string> replace_compare;
		std::vector<std::string> replace_replacement;
		std::vector<std::string> delete_file;
		std::vector<std::string> accept_extension;
		std::vector<std::string> move_extension;
		std::vector<std::string> move_path;
		std::vector<std::string> delete_file_with_string;
		std::vector<std::string> audio_file;
		std::vector<std::string> video_file;
		std::vector<std::string> skip_list;
		std::vector<std::string> skip_directory_list;
};

#endif

