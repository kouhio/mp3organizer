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

directory_tracklist::directory_tracklist()
{
#if (DEBUG == 1)
cout << __FUNCTION__ << endl;
#endif
	item_count = 0;
	last_value_returned = 0;
	last_intermediate_returned = 0;
	clear_item_list();
}

directory_tracklist::~directory_tracklist()
{
#if (DEBUG == 1)
cout << __FUNCTION__ << endl;
#endif
	item_count = 0;
	last_value_returned = 0;
	last_intermediate_returned = 0;
	clear_item_list();
}

void directory_tracklist::add_item_to_list(std::string data)
{
#if (DEBUG == 1)
cout << __FUNCTION__ << endl;
#endif
	item.push_back(data);
	item_count++;
}

void directory_tracklist::clear_item_list(void)
{
#if (DEBUG == 1)
cout << __FUNCTION__ << endl;
#endif
	item.clear();
	values.clear();
	item_count = 0;
	last_value_returned = 0;
	last_intermediate_returned = 0;
}

int directory_tracklist::find_first_value_not_used(void)
{
	int i = 0, value = 1, found = 0;
	
#if (DEBUG == 1)
cout << __FUNCTION__ << endl;
#endif
	for(i=0;i<item_count;i++)
	{
		found = 0;
		for(ipos = item.begin(); ipos < item.end(); ipos++)
		{
			if(value == get_value_from_string(*ipos))
			{
				found = 1;
				break;
			}
		}
		if(found)
		{
			value++;
		}
		else if(value > last_intermediate_returned)
		{
			last_intermediate_returned = value;
			return value;
		}
	}
	if(!last_value_returned)
	{
		if(!last_intermediate_returned)
		{
			last_value_returned = item_count;
		}
		else
		{
			last_value_returned = last_intermediate_returned + 1;
		}
	}
	return last_value_returned;
}

int directory_tracklist::get_first_value(void)
{
	int value = item_count, compare = 0;

#if (DEBUG == 1)
cout << __FUNCTION__ << endl;
#endif
	for(ipos = item.begin(); ipos < item.end(); ipos++)
	{
		compare = get_value_from_string(*ipos);
		value = (compare < value)? compare: value;
	}
	return value;
}

int directory_tracklist::get_last_value(void)
{
	int value = 0, compare = item_count;

#if (DEBUG == 1)
cout << __FUNCTION__ << endl;
#endif
	for(ipos = item.begin(); ipos < item.end(); ipos++)
	{
		compare = get_value_from_string(*ipos);
		value = (compare > value)? compare: value;
	}
	return value;
}

std::string directory_tracklist::check_for_value_multiplies(std::string data)
{
	int value = 0, found = 0;
	size_t	i = 0;
	std::string temp = int_to_string(find_first_value_not_used());
	std::vector<std::string>::iterator replace = item.end();

#if (DEBUG == 1)
cout << __FUNCTION__ << endl;
#endif

	if(get_value_from_string(temp) < 10)
	{
		temp.insert(0, "0");
	}

	if((value = get_value_from_string(data)) <= 0)
	{
		return data;
	}

	for(ipos = item.begin(); ipos < item.end(); ipos++)
	{
		if(data == *ipos)
		{
			replace = ipos;
		}
		if(value == get_value_from_string(*ipos))
		{
			found++;
		}
	}
	if(found > 1)
	{
		i=0;
		while((isdigit(data[i])) && (i < data.size()))
		{
			i++;
		}
		data.replace(0, i, temp);
		if(replace != item.end())
		{
			*replace = data;
		}
	}

	return data;
}

std::string directory_tracklist::check_for_minus_after_trackvalue(std::string data)
{
	size_t pos = 0;

#if (DEBUG == 1)
cout << __FUNCTION__ << endl;
#endif
	if(!isdigit(data[0]))
	{
		return data;
	}
	if((pos = data.find(" - ")) != npos)
	{
		if(pos != 2)
		{
			data.insert(2, " - ");
		}
	}
	else
	{
		if(pos != 2)
		{
			data.insert(2, " - ");
		}
	}

	return data;
}

std::string directory_tracklist::check_filename_against_filelist(std::string data)
{
	int track_value = get_value_from_string(data), compare_value = 0, found = 0, new_value = 0;
	size_t i = 0;
	std::string temp;
	
#if (DEBUG == 1)
cout << __FUNCTION__ << endl;
#endif

	if((new_value = get_value_for_item(data)) > 0)
	{
		temp = int_to_string(new_value); 
	}
	else
	{
		temp = int_to_string(find_first_value_not_used());
		last_value_returned++;
	}

	if(track_value > 0)
	{
		for(ipos = item.begin(); ipos < item.end(); ipos++)
		{
			compare_value = get_value_from_string(*ipos);
			if(compare_value == track_value)
			{
				found++;
				break;
			}
		}
	}
	data = check_for_value_multiplies(data);
	track_value = get_value_from_string(data);
	if(get_value_from_string(temp) < 10)
	{
		temp.insert(0, "0");
	}

	if((track_value < 10) && (track_value > 0))
	{
		if(!isdigit(data[0]))
		{
			data.insert(0, "0");
		}
	}
	while((track_value >= 100) && (handler->options.get_reorder()))
	{
		data.erase(0, 1);
		track_value = get_value_from_string(data);
	}
	if(track_value <= 0)
	{
		if(!handler->file.get_data_count(handler->file.get_current_directory(), IS_DIRECTORY))
		{
			data.insert(0, temp + " ");
		}
	}
	else
	{
		i = 0;
		while((isdigit(data[i])) && (i < data.size()))
		{
			i++;
		}
		data.replace(0, i, temp);
	}
	data = check_for_minus_after_trackvalue(data);
	if(ipos < item.end())
	{
		*ipos = data;
	}

	return data;
}

void directory_tracklist::add_items_from_path(std::string path)
{
	struct dirent *namelist;
	DIR *directory;
	std::string check;

#if (DEBUG == 1)
cout << __FUNCTION__ << endl;
#endif
	if((handler->file.get_data_count(path, IS_DIRECTORY)) || (!handler->file.get_data_count(path, IS_FILE)))
	{
		return;
	}
	clear_item_list();

	if((directory = opendir(path.c_str())) != NULL)
	{
		do
		{
			if((namelist = readdir(directory)) != NULL)
			{
				check = path + DIR_MARK + namelist->d_name;
				if(check_dir_or_file(check) == IS_FILE)
				{
					if(handler->file.get_filetype(check) == AUDIO_FILE)
					{
						add_item_to_list(namelist->d_name);
					}				
				}
			}
		}
		while(namelist != NULL);
		closedir(directory);
	}
	organize_track_values();
}

int directory_tracklist::get_value_for_item(std::string data)
{
	std::vector<int>::iterator vpos = values.begin();

#if (DEBUG == 1)
cout << __FUNCTION__ << endl;
#endif
	for(ipos = item.begin(); ipos < item.end(); ipos++)
	{
		if(*ipos == data)
		{
			return *vpos;
		}
		vpos++;
	}
	return -1;
}

void directory_tracklist::organize_track_values(void)
{
	int value = get_last_value(), compare = 0, i = 0, last = 0, set = 1;
	std::vector<int>::iterator vpos;
#if (DEBUG == 1)
cout << __FUNCTION__ << endl;
#endif
	values.clear();
	for(ipos = item.begin(); ipos < item.end(); ipos++)
	{
		values.push_back(0);
	}
	for(i=0;i<item_count;i++)
	{
		for(ipos = item.begin(); ipos < item.end(); ipos++)
		{
			compare = get_first_value_from_string(*ipos);
			if(compare >= 0)
			{
				value = ((compare < value) && (compare > last))? compare: value;
			}
		}
		vpos = values.begin();
		for(ipos = item.begin(); ipos < item.end(); ipos++)
		{
			if((compare = get_first_value_from_string(*ipos)) == value)
			{
				*vpos = set++;
				break;
			}
			vpos++;
		}
		last = value;
		value = get_last_value() + 1;
	}
	for(vpos = values.begin(); vpos < values.end(); vpos++)
	{
		if(*vpos <= 0)
		{
			*vpos = set++;
		}
	}
}

std::vector<std::string>::iterator directory_tracklist::get_corresponting_iterator(std::string data)
{
#if (DEBUG == 1)
cout << __FUNCTION__ << endl;
#endif
	for(ipos = item.begin(); ipos < item.end(); ipos++)
	{
		if(*ipos == data)
		{
			return ipos;
		}
	}
	return item.end();
}

std::vector<std::string>::iterator directory_tracklist::get_last_comparison(void)
{
#if (DEBUG == 1)
cout << __FUNCTION__ << endl;
#endif
	return item.end();
}

