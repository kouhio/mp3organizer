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

size_t npos = string::npos;
comparison_list *handler = new comparison_list;

int main(void)
{
	string koe;

#if (DEBUG == 1)
cout << __FUNCTION__ << endl;
#endif
	handler->timer.reset_timers();
	remove("organizer.unknown.txt");
	remove("organizer.changelog.txt");
	handler->files.reset_counters();
	handler->directories.reset_counters();
	handler->file.get_current_path();
	handler->options.setup();
#if (OS_LINUX == 0)
	handler->display.set_dimension(80, 23);
#else
	//handler->display.set_dimension(142, 56);
	handler->display.update_dimensions();
#endif
	handler->file.get_current_path();

	handler->parse_configuration_ini();
	handler->display.clear_screen();
	handler->file.handle_files_and_directories(handler->file.get_current_directory());

	handler->display.update_information();
	handler->options.close_file();
	handler->options.close_m3u();
	handler->display.move_to_last_row();
	
#if (OS_LINUX == 0)
	cout << "Done! Press any key to continue." << endl;
	getch();
#endif

	delete handler;

	return 0;
}

