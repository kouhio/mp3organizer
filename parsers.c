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
#include <conio.h>
#endif

#include <dirent.h>
#include <sys/stat.h>

#ifdef __cplusplus
}
#endif

#include "libraries.h"

int get_value_from_string(std::string data)
{
	unsigned int i = 0, found = 0;
	std::string temp;

#if (DEBUG == 1)
cout << __FUNCTION__ << " " << data << endl;
#endif

	if(!isdigit(data[0]))
	{
		return -1;
	}

	while(i < data.length())
	{
		if(found && (!isdigit(data[i])))
		{
			break;
		}
		else if(isdigit(data[i]))
		{
			temp += data[i];
			found++;
		}
		i++;
	}
	return atoi(temp.c_str());
}

int get_first_value_from_string(std::string data)
{
	size_t found = 0, pos = 0;
	std::string temp;

#if (DEBUG == 1)
cout << __FUNCTION__ << " " << data << endl;
#endif

	while((!isdigit(data[pos])) && (pos < data.size()))
	{
		pos++;
	}
	if(pos >= data.size())
	{
		return -1;
	}
	temp.clear();

	while(pos < data.length())
	{
		if(found && (!isdigit(data[pos])))
		{
			break;
		}
		else if(isdigit(data[pos]))
		{
			temp += data[pos];
			found++;
		}
		pos++;
	}
	return atoi(temp.c_str());
}

std::string correct_file_numbering(std::string data, int track_count)
{
	int track_value = get_value_from_string(data), filetype;
	filetype = handler->file.get_current_filetype();

#if (DEBUG == 1)
cout << __FUNCTION__ << endl;
#endif

	switch(filetype)
	{
		case AUDIO_FILE:
			if(track_count > 99) {
				return removeFileValue(data);
			}
			else if(track_value < 10)
			{
				if((isdigit(data[0])) && (!isdigit(data[1])))
				{
					data.insert(0, "0");
				}
			}
			if((isdigit(data[0])) && (isdigit(data[1])) && (!isdigit(data[2])) && (data[3] != '-') && (data[2] != '-'))
			{
				data.insert(2, " - ");
			}
		break;
		
		case VIDEO_FILE:
			if((data[0] == 's') && isdigit(data[1]) && isdigit(data[2]) && (data[3] == 'e'))
			{
				data.erase(3, 1);
				data.erase(0, 1);
				if(data[0] == '0')
				{
					data.erase(0, 1);
				}
			}
			else if((data[0] == 's') && isdigit(data[1]) && (data[2] == 'e'))
			{
				data.erase(2, 1);
				data.erase(0, 1);
			}
			else if(isdigit(data[0]) && data[1] == 'x' && isdigit(data[2]))
			{
				data.erase(1, 1);
			}
			else if(isdigit(data[1]) && data[2] == 'x' && isdigit(data[3]))
			{
				data.erase(2, 1);
			}
		break;
	}
	return data;
}

std::string upper_all_necessary_data(std::string data)
{
	size_t pos = 0, end = 0, i = 0;
	std::string temp;

#if (DEBUG == 1)
cout << __FUNCTION__ << endl;
#endif
	for(i=0;i<data.size();i++)
	{
		temp += char_tolower(data[i]);
	}

	pos = 0;
	while(((pos = temp.find("mr", pos)) != npos) && (pos < temp.size()))
	{
		if((pos == 0) || (isblank(temp[pos-1])))
		{
			if(isblank(temp[pos+2]))
			{
				temp.insert(pos+2, ".");
			}
		}
		pos++;
	}
	pos = 0;
	while(((pos = temp.find("vs", pos)) != npos) && (pos < temp.size()))
	{
		if((isblank(temp[pos-1])) && (isblank(temp[pos+1])))
		{
			if(isblank(temp[pos+2]))
			{
				temp.insert(pos+2, ".");
			}
		}
		pos++;
	}
	pos = 0;
	while(((pos = temp.find("dr", pos)) != npos) && (pos < temp.size()))
	{
		if((pos == 0) || (isblank(temp[pos-1])))
		{
			if(isblank(temp[pos+2]))
			{
				temp.insert(pos+2, ".");
			}
		}
		pos++;
	}
	pos = 0;
	while(((pos = temp.find("ep", pos)) != npos) && (pos < temp.size()))
	{
		if((temp[pos-1] == ' ') || (temp[pos-1] == '-'))
		{
			if(pos+2 >= data.size())
			{
				end = data.size();
			}
			else if(isblank(data[pos+2]))
			{
				end = pos + 2;
			}
			while(pos < end)
			{
				data[pos] = char_toupper(data[pos]);
				pos++;
			}
			end = 0;
		}
		pos++;
	}
	pos = 0;
	while(((pos = temp.find("cd", pos)) != npos) && (pos < temp.size()))
	{
		if((temp[pos-1] == ' ') || (temp[pos-1] == '-'))
		{
			if(pos+4 >= data.size())
			{
				end = data.size();
			}
			else if((isblank(data[pos+3])) || (isdigit(data[pos+3])))
			{
				end = pos + 3;
			}
			while(pos < end)
			{
				data[pos] = char_toupper(data[pos]);
				pos++;
			}
			end = 0;
		}
		pos++;
	}
	if(!temp.compare(0, 2, "cd"))
	{
		if((isblank(data[2])) || (isdigit(data[2])))
		{
			pos = 0;
			while((isblank(data[pos])) || (isdigit(data[pos])))
			{
				data[pos] = char_toupper(data[pos]);
				pos++;
			}
		}
	}

	return data;
}

char *remove_excessive_data_from_stream(char *data)
{
	size_t len = 0, len2 = (strlen(data) - 1);

#if (DEBUG == 1)
cout << __FUNCTION__ << endl;
#endif
	while(((data[len2] == ' ') || (data[len2] == '-')) && (len2 > 0))
	{
		data[len2] = '\0';
		len2--;
	}
	while(((data[len] == ' ') || (data[len] == '-')) && (len < len2))
	{
		data[len] = '\0';
		len++;
	}
	return &data[len];
}

int get_true_or_false(std::string data)
{
#if (DEBUG == 1)
cout << __FUNCTION__ << endl;
#endif
	if((!data.compare(0, 4, "true")) || (!data.compare(0, 1, "1")) || (!data.compare(0, 7, "enabled")))
	{
		return true;
	}
	return false;
}

std::string remove_underscores(std::string data)
{
	size_t pos = 0;

#if (DEBUG == 1)
cout << __FUNCTION__ << endl;
#endif
	while(((pos = data.find("_")) != npos) && (pos < data.size()))
	{
		data[pos] = ' ';
	}
	return data;
}

std::string removeDoubleValues(std::string data) {
	size_t pos = 3;
	char temp[8];
	memset(temp, 0, sizeof(temp));

	if(data.copy(temp, 4, 0) > 0) {
		if(data[3] == '-') {
			while((pos = data.find(temp, pos)) != npos) {
				data.erase(0, 4);
			}
		}
	}

	return data;
}

std::string removeEmptyParenthesis(std::string data) {
	size_t pos = 0;

	while((pos = data.find("( )", pos)) != npos) {
		data.erase(pos, 3);
	}
	while((pos = data.find("[ ]", pos)) != npos) {
		data.erase(pos, 3);
	}
	while((pos = data.find("{ }", pos)) != npos) {
		data.erase(pos, 3);
	}
	while((pos = data.find("()", pos)) != npos) {
		data.erase(pos, 2);
	}
	while((pos = data.find("[]", pos)) != npos) {
		data.erase(pos, 2);
	}
	while((pos = data.find("{}", pos)) != npos) {
		data.erase(pos, 2);
	}
	pos = 0;
	return data;
}

std::string removeFileValue(std::string data) {
	if(data[3] == '-') {
		data.erase(0, 4);
	}
	return data;
}

std::string remove_excess_dots(std::string data, std::string extension)
{
	size_t pos = 0, pos2 = 0;

#if (DEBUG == 1)
cout << __FUNCTION__ << endl;
#endif
	pos = 0;
	while((pos = data.find("..", pos)) != npos)
	{
		if(!data.compare(pos, 3, "..."))
		{
			pos+=3;
		}
		else if(!data.compare(pos, 2, ".."))
		{
			data.erase(pos, 1);
		}
		else
		{
			pos++;
		}
	}
	
	pos2 = data.rfind(extension);
	
	/*pos = 0;
	while(((pos = data.find(".", pos)) != npos) && (pos < data.size()) && (pos < pos2))
	{
		if((isalpha(data[pos+1])) && (isalpha(data[pos+2])))
		{
			data.insert(pos+1, " ");
		} 
		pos++;
	}*/

	pos = 0;
	while(((pos = data.find(".", pos)) != npos) && (pos < data.size()) && (pos < pos2))
	{
		if(!isblank(data[pos+1]))
		{
			if((data[pos + 2] != '.') && (data[pos - 2] != '.') && (data[pos+1] != ' '))
			{
				if(data.compare(pos, extension.size(), extension))
				{
					data[pos] = ' ';
				}
			}
		}
		pos++;
	}
	while((pos = data.find(".-.")) != npos)
	{
		if(data.compare(pos+2, extension.size(), extension))
		{
			data.replace(pos, 3, " - ");
		}
		else
		{
			data.erase(pos, 2);
		}
	}
	while((pos = data.find(" .")) != npos)
	{
		if(data.compare(pos+1, extension.size(), extension))
		{
			data.replace(pos, 2, " ");
		}
		else
		{
			data.erase(pos, 1);
		}
	}
	while((pos = data.find("- .")) != npos)
	{
		if(data.compare(pos+2, extension.size(), extension))
		{
			data.replace(pos, 3, "- ");
		}
		else
		{
			data.erase(pos, 2);
		}
	}
	while((pos = data.find("-.")) != npos)
	{
		if(data.compare(pos+1, extension.size(), extension))
		{
			data.replace(pos, 2, "-");
		}
		else
		{
			data.erase(pos, 1);
		}
	}
	pos = 0;
	while((pos = data.find(".", pos)) != npos)
	{
		if(!isblank(data[pos+1]))
		{
			if(data.compare(pos, extension.size(), extension))
			{
				if((isalpha(data[pos-1])) && (isalpha(data[pos-2])) && (isalpha(data[pos+1])) && (data[pos+2] == '.') && (isalpha(data[pos+3])))
				{
					data[pos] = ' ';
					data[pos+2] = ' ';
				}
			}
		} 
		pos++;
	}
	pos = 0;
	while((pos = data.find(".", pos)) != npos)
	{
		if(data.compare(pos, extension.size(), extension))
		{
			if((isalpha(data[pos+1])) && (data[pos+2] == '-'))
			{
				data[pos] = ' ';
			}
		}
		pos++;
	}
	pos = 0;
	while((pos = data.find(".", pos)) != npos)
	{
		if(!isblank(data[pos+1]))
		{
			if(data.compare(pos, extension.size(), extension))
			{
				if((isalpha(data[pos-1])) && (!isalpha(data[pos+1])))
				{
					data[pos] = ' ';
				}
				else if((!isalpha(data[pos-1])) && (isalpha(data[pos+1])))
				{
					data[pos] = ' ';
				}
				else if((isalpha(data[pos-2])) || (isalpha(data[pos+2])))
				{
					data[pos] = ' ';
				}
			}
		}
		else if(isdigit(data[pos-1]))
		{
			data[pos] = ' ';
		}
		pos++;
	}
	return data;
}

std::string remove_excess_space(std::string data)
{
	size_t pos = 0;
	std::string ext = handler->file.get_current_extension();
#if (DEBUG == 1)
cout << __FUNCTION__ << endl;
#endif
	if((ext[0] != '.') && (ext.size() > 0))
	{
		ext.insert(0, "."); 
	}
	while(((pos = data.find("  ")) != npos) && (pos < data.size()))
	{
		data.replace(pos, 2, " ");
	}
	for(pos=0;pos<data.size();pos++)
	{
		if((data[pos] == ' ') && (isalpha(data[pos+1])) && (data[pos+2] == ' ') && (data[pos+4] == ' ') && (data[pos+1] != '-') && (data[pos-1] != '-') && (!isalpha(data[pos-2])))
		{
			while((data[pos] == ' ') && (isalpha(data[pos+1])) && (data[pos+2] == ' ') && (pos < data.size()) && (data[pos+1] != '-') && (data[pos-1] != '-'))
			{
				data[pos] = '.';
				pos += 2;
			}
		} 
	}
	if((pos = data.rfind(ext)) != npos)
	{
		if(pos > 0)
		{
			while(data[pos-1] == ' ')
			{
				data.erase(pos-1, 1);
			}
		}
	}

	if(data[0] == ' ')
	{
		while(data[0] == ' ')
		{
			data.erase(0, 1);
		}
	}
	
	pos = 0;
	while((pos = data.find(" ", pos)) != npos)
	{
		if((isalpha(data[pos-1])) && (isalpha(data[pos-3])) && (isblank(data[pos-2])) && (isblank(data[pos+2])) && (data[pos+1] != '-')) 
		{
			data[pos-2] = '.';
		}
		pos++;
	}
	
	pos = 0;
	while((pos = data.find(" ", pos)) != npos)
	{
		if((data[pos-2] == '.') && (data[pos-4] == '.'))
		{
			data.insert(pos, ".");
		}
		pos++;
	}
	if(((pos = data.rfind(ext)) != npos) && (ext.size() > 0) && (ext.size() < 5))
	{
		if(data[pos-2] == '.')
		{
			data.insert(pos, ".");
		}
	}
	
	return data;
}

std::string check_for_commas(std::string data)
{
	size_t pos = 0;

#if (DEBUG == 1)
cout << __FUNCTION__ << endl;
#endif
	while(((pos = data.find(",", pos)) != npos) && (pos < data.size()))
	{
		if(data[pos+1] != ' ')
		{
			data.insert(pos+1, " ");
		}
		if((data[pos-1] == ' ') && (pos > 0))
		{
			data.erase(pos-1, 1);
		}
		pos++;
	}
	return data;
}

std::string check_for_minus_spacing(std::string data)
{
	size_t pos = 0;
	char p1, p2, p3, p4;

#if (DEBUG == 1)
cout << __FUNCTION__ << endl;
#endif
	if(data[0] == '-')
	{
		while(data[0] == '-')
		{
			data.erase(0, 1);
		}
	}

	while(((pos = data.find("-", pos)) != npos) && (pos < data.size()))
	{
		p1 = data[pos-1];
		p2 = data[pos+1];
		p3 = data[pos-2];
		p4 = data[pos+2];

		if((isblank(p1)) && (isdigit(p3)) && (isblank(data[pos-3])) && (pos > 4))
		{
			data.replace(pos-1, 3, "-");
		}
		else if((isalpha(p3)) && (isdigit(p4)) && (!isdigit(data[pos+4])) && (isblank(p1)) && (isblank(p2)) && (data[pos+5] != '-'))
		{
			data.replace(pos-1, 3, "-");
		}
		else if((!isblank(p1)) && (isblank(p2)))
		{
			data.insert(pos, " ");
			pos++;
		}
		else if((isblank(p1)) && (!isblank(p2)))
		{
			data.insert(pos+1, " ");
		}
		else if((isdigit(p1)) && (!isdigit(p2)))
		{
			data.replace(pos, 1, " - ");
		}
		else if((!isdigit(p1)) && (isdigit(p2)))
		{
			data.replace(pos, 1, " - ");
		}
		else if((isalpha(p3)) && (isalpha(p4)) && (isblank(p1)) && (isblank(p2)))
		{
			data.replace(pos-1, 3, "-");
		}
		pos++;
	}
	while((pos = data.find("- -")) != npos)
	{
		data.erase(pos, 2);
	}
	while((pos = data.find("--")) != npos)
	{
		data.erase(pos, 1);
	}
	while((pos = data.find(".-.")) != npos)
	{
		data.replace(pos, 3, " - ");
	}
	pos = data.size() - 1;
	while((data[pos] == '-') || (data[pos] == ' '))
	{
		data.erase(pos, 1);
		pos = data.size() - 1;
	} 
	return data;
}

std::string parse_directories_down(std::string path, int count)
{
	size_t pos = path.size();
	int compare = 0;
	std::string new_path = path;

#if (DEBUG == 1)
cout << __FUNCTION__ << endl;
#endif
	while((pos = new_path.rfind(DIR_MARK, pos)) != npos)
	{
		if(++compare >= count)
		{
			new_path.erase(pos, (new_path.size() - pos));
			return new_path;
		}
		pos--;
	}

	return path;
}

std::string parse_current_file_dir(std::string path)
{
	size_t pos = 0;

#if (DEBUG == 1)
cout << __FUNCTION__ << endl;
#endif
	if((pos = path.rfind(DIR_MARK)) != npos)
	{
		return &path[pos+1];
	}
	return path;
}

std::string parse_filename(std::string path)
{
	size_t pos = 0;

	if((pos = path.rfind(DIR_MARK)) != npos)
	{
		return &path[pos+1];
	}
	return path;
}

std::string int_to_string(int value)
{
#if (DEBUG == 1)
cout << __FUNCTION__ << endl;
#endif
	std::stringstream stream;
	std::string temp;

	stream << value;
	stream >> temp;

	return temp;
}

std::string to_lowercase(std::string data)
{
	size_t i = 0;
#if (DEBUG == 1)
cout << __FUNCTION__ << endl;
#endif

	for(i=0;i<data.size();i++)
	{
		data[i] = char_tolower(data[i]);
	}
	return data;
}

#if (OS_LINUX == 0)
int isblank(int c)
{
#if (DEBUG == 1)
cout << __FUNCTION__ << endl;
#endif
	if((c == ' ') || (c == '\t'))
	{
		return 1;
	} 
	return 0;
}
#endif

std::string check_character_spacing(std::string data)
{
	size_t pos = 0, pos2;

#if (DEBUG == 1)
cout << __FUNCTION__ << endl;
#endif

	while(((pos = data.find("(", pos)) != npos) && (pos < data.size()))
	{
		if((data[pos-1] != ' ') && (pos > 0))
		{
			data.insert(pos, " ");
			pos++;
		}
		pos++;
	} 

	pos = 0;
	if((pos2 = data.rfind(handler->file.get_current_extension())) == npos)
	{
		pos2 = data.size();
	}
	pos2--;
	while(((pos = data.find("[", pos)) != npos) && (pos < data.size()) && (pos < pos2))
	{
		if((data[pos-1] != ' ') && (pos > 0))
		{
			data.insert(pos, " ");
		}
		pos++;
	} 

	pos = 0;
	if((pos2 = data.rfind(handler->file.get_current_extension())) == npos)
	{
		pos2 = data.size();
	}
	pos2--;
	while(((pos = data.find(")", pos)) != npos) && (pos < data.size()) && (pos < pos2))
	{
		if(data[pos+1] != ' ')
		{
			data.insert(pos+1, " ");
		}
		if(data[pos-1] == ' ')
		{
			data.erase(pos-1, 1);
		}
		pos++;
	}

	pos = 0;
	if((pos2 = data.rfind(handler->file.get_current_extension())) == npos)
	{
		pos2 = data.size();
	}
	pos2--;
	while(((pos = data.find("]", pos)) != npos) && (pos < data.size()) && (pos < pos2))
	{
		if(data[pos+1] != ' ')
		{
			data.insert(pos+1, " ");
		}
		if(data[pos-1] == ' ')
		{
			data.erase(pos-1, 1);
		}
		pos++;
	} 

	return data;
}

std::string check_for_thes(std::string data)
{
	size_t pos = 0; //, size = 0;
	std::string comp = handler->file.current_file.get_band_name();

#if (DEBUG == 1)
cout << __FUNCTION__ << endl;
#endif
	
	if(!data.compare(0, 4, "the "))
	{
		if(comp.size() < data.size())
		{
			pos = comp.size();
			//pos = data.find(comp);
			//size = comp.size() - 4;
		}
		else
		{
			pos = data.find("-");
			//size = pos - 1;
		}
		if(pos != npos)
		{
			data.insert(pos, ", the ");
			data.erase(0, 4);
		}
	}

	return data;
}

char char_toupper(char data)
{
	if((data >= 97) && (data <= 122))
	{
		//data -= 32;
		data = toupper(data);
	}
	else if(data == -122) // Å
	{
		data = -113;
	}
	else if(data == -108) // Ö
	{
		data = -103;
	}
	else if(data == -124) // Ä
	{
		data = -114;
	}
	return data;
}

char char_tolower(char data)
{
	if((data >= 65) && (data <= 90))
	{
		//data += 32;
		data = tolower(data);
	}
	else if(data == -113)
	{
		data = -122;
	}
	else if(data == -103)
	{
		data = -108;
	}
	else if(data == -114)
	{
		data = -124;
	}
	return data;

}

std::string remove_excess_characters(std::string data)
{
	unsigned int i;
#if (DEBUG == 1)
cout << __FUNCTION__ << endl;
#endif

	for(i=0;i<data.size();i++)
	{
		if((data[i] == -60) || (data[i] == '?'))
		{
			data.erase(i, 1);
		}
	}

	return data;
}

