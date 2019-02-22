#ifndef __DIR_HANDLERS_H__
#define __DIR_HANDLERS_H__

enum
{
	IS_DIRECTORY = 1,
	IS_FILE,
	IS_TEXT,
	IS_BOTH,
};

int check_dir_or_file(std::string path);
void makedir(std::string path);
int move_directory(std::string source, std::string target);
int deltree(std::string path);
std::string check_for_multiple_cds(std::string dir, std::string path);

#endif

