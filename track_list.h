#ifndef __TRACK_LIST_H__
#define __TRACK_LIST_H__

#include <string>
#include <vector>

class directory_tracklist
{
	public:
		directory_tracklist();
		~directory_tracklist();
		void add_item_to_list(std::string data);
		void clear_item_list(void);
		int find_first_value_not_used(void);
		std::string check_filename_against_filelist(std::string data);
		void add_items_from_path(std::string path);
		std::string check_for_value_multiplies(std::string data);
		std::string check_for_minus_after_trackvalue(std::string data);
		int get_first_value(void);
		int get_last_value(void);
		int get_value_for_item(std::string data);
		void organize_track_values(void);
		std::vector<std::string>::iterator get_corresponting_iterator(std::string data);
		std::vector<std::string>::iterator get_last_comparison(void);
	private:
		int item_count;
		std::vector<std::string> item;
		std::vector<int> values;
		std::vector<std::string>::iterator ipos;
		int last_value_returned;
		int last_intermediate_returned;
};

#endif

