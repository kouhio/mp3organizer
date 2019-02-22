#ifndef __FILE_HANDLERS_H__
#define __FILE_HANDLERS_H__

#include <string>

#include "comparison_list.h"
#include "conversion_counter.h"
#include "file_information.h"

#if (OS_LINUX == 1)
#include "id3.h"
#endif

#define UNKNOWN_FILE 0
#define AUDIO_FILE 1
#define VIDEO_FILE 0
#define SKIP_FILE 2

class file_handlers
{
	public:
		file_handlers();
		~file_handlers();

		void get_current_path(void);
		int handle_files_and_directories(std::string path);
		std::string get_current_extension(std::string data);
		std::string get_current_extension(void);
		std::string get_current_directory(void);
		std::string get_current_filename(void);

		std::string handle_directory(std::string path);
		std::string handle_file(std::string file);

		int delete_current_file(void);
		int rename_current_file(std::string new_filename);
		int rename_current_directory(std::string new_dirname);
		int move_current_file(std::string path);

		int move_and_rename_file(std::string file);
		std::string correct_string_char_sizes(std::string data);
		std::string upper_romans(std::string data);
		void set_target_directory(std::string data);

		int get_data_count(std::string path, int type);
		int get_current_filetype(void);
		int get_filetype(std::string data);
		
		int get_level(void);
		void add_level(void);
		void decrease_level(void);
		
		std::string get_next_dir_name(std::string path);
		std::string get_highest_directory(std::string path);
		
		file_information current_file;

		void handleID3Shit(void);


	private:
#if (ID3 == 1)
		mp3_file mp3;
#endif
		std::string filename;
		std::string directory;
		std::string extension;
		std::string target_dir;
		int move_to_new_folder;
		int current_level;
		int repeat;
};

int move_file(std::string source, std::string target);

#endif

