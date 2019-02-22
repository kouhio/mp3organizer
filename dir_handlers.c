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
#endif

#include <dirent.h>
#include <sys/stat.h>

#ifdef __cplusplus
}
#endif

#include "libraries.h"

void makedir(std::string path)
{
#if (DEBUG == 1)
cout << __FUNCTION__ << endl;
#endif
#if (OS_LINUX == 0)
	mkdir(path.c_str(), S_IRWXG | S_IROTH | S_IXOTH);
#else
	mkdir(path.c_str(), 0777);
#endif
}

int check_dir_or_file(std::string path)
{
	DIR *dir;
	//FILE *fd;
	int size = 0;
	std::string comparer = parse_filename(path);

	size = comparer.size();
	
	if(size < 3)
	{
		if((strncmp(comparer.c_str(), ".", 1) == 0) || (strncmp(comparer.c_str(), "..", 2) == 0))
		{
//cout << path << " " << 1 << " " << comparer << endl;
			return 0;
		}
	}
#if (OS_LINUX == 1)
	else if(strncmp(comparer.c_str(), "./.", 3) == 0)
	{
cout << path << " " << 2 << " " << comparer << endl;
		return 0;
	}
#else
	else if(strncmp(comparer.c_str(), ".\\.", 3) == 0)
	{
		return 0;
	}
#endif

	if((dir = opendir(path.c_str())) != NULL)
	{
		closedir(dir);
		return IS_DIRECTORY;
	}
	/*else if((fd = fopen(path.c_str(),"rb")) != NULL)
	{
		fclose(fd);
		return IS_FILE;
	}
	else if((fd = fopen(path.c_str(),"rt")) != NULL)
	{
		fclose(fd);
		return IS_TEXT;
	}*/
	return IS_FILE;
}

int move_directory(std::string source, std::string target)
{
#if (DEBUG == 1)
cout << __FUNCTION__ << endl;
#endif
	if(source == target)
	{
		return -1;
	}

	return rename(source.c_str(), target.c_str());
}

int deltree(std::string path)
{
	struct dirent *namelist;
	DIR *directory;
	int type = 0;
	std::string new_path, temp;

#if (DEBUG == 1)
cout << __FUNCTION__ << endl;
#endif
	if((directory = opendir(path.c_str())) != NULL)
	{
		do
		{
			if((namelist = readdir(directory)) != NULL)
			{
				temp = path + DIR_MARK + namelist->d_name;
				if(!(type = check_dir_or_file(temp)))
				{
					continue;
				}
				new_path = path + DIR_MARK + namelist->d_name;
				if(type == IS_DIRECTORY)
				{
					if(!handler->file.get_data_count(new_path, IS_BOTH))
					{
						remove(new_path.c_str());
					}
					else
					{
						deltree(new_path);
					}
				}
				else if(type == IS_FILE)
				{
					remove(new_path.c_str());
				}
			}
		}
		while(namelist != NULL);
		closedir(directory);
		remove(path.c_str());
	}
	return 0;
}

std::string check_for_multiple_cds(std::string dir, std::string path)
{
	int found = 0;

#if (DEBUG == 1)
cout << __FUNCTION__ << endl;
#endif
	if(!dir.compare(0, 2, "cd"))
	{
		if((dir[2] == ' ') || (isdigit(dir[2])))
		{
			found++;
		}
	}
	else if(!dir.compare(0, 5, "disc "))
	{
		found++;
	}
	else if((dir[0] == '#') && (isdigit(dir[1])))
	{
		found++;
	}

	if(found)
	{
		return parse_directories_down(path, 2) + DIR_MARK + parse_current_file_dir(parse_directories_down(path, 1));
	}
	return dir;
}


