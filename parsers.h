#ifndef __PARSERS_H__
#define __PARSERS_H__

#include <string>

int get_value_from_string(std::string data);
int get_first_value_from_string(std::string data);
std::string correct_file_numbering(std::string data, int track_count);
std::string upper_all_necessary_data(std::string data);
char *remove_excessive_data_from_stream(char *data);
int get_true_or_false(std::string data);

std::string remove_underscores(std::string data);
std::string remove_excess_dots(std::string data, std::string extension);
std::string remove_excess_space(std::string data);
std::string check_for_commas(std::string data);
std::string check_for_minus_spacing(std::string data);
std::string parse_directories_down(std::string path, int count);
std::string parse_current_file_dir(std::string path);
std::string parse_filename(std::string path);
std::string int_to_string(int value);
std::string to_lowercase(std::string data);
std::string check_character_spacing(std::string data);
std::string check_for_thes(std::string data);
char char_toupper(char data);
char char_tolower(char data);
std::string remove_excess_characters(std::string data);

std::string removeFileValue(std::string data);
std::string removeEmptyParenthesis(std::string data);
std::string removeDoubleValues(std::string data);

#if (OS_LINUX == 0)
int isblank(int c);
#endif

#endif

