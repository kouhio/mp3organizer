#ifndef __DATA_OUTPUTS_H__
#define __DATA_OUTPUTS_H__

#include <string>
#include <cstdio>

class file_options
{
	public:
		void add_unknown_file_data(std::string data);
		void add_unknown_file_extension_data(std::string data);
		void add_changed_data(std::string source, std::string target, int type);
		void setup(void);
		int parse_setup_data(std::string data);
		char get_reorder(void);
		char get_romans(void);
		char remove_id(void);
		char add_id(void);
		int close_file(void);
		int make_m3u_file(std::string data);
		void close_m3u(void);
	private:
		char print_output;
		char print_unknown;
		char reorder_cd_folders;
		char upper_romans;
		char output_open;
		char mem_count;
		char remove_id3;
		char add_id3;
		char make_m3u;
		char m3u_open;
		FILE *fd;
		FILE *m3u;
};

#endif


