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
#include <sys/ioctl.h>
#include <unistd.h>

#ifdef __cplusplus
}
#endif

#include "libraries.h"

#if (OS_LINUX == 0)
union REGS regs;
#else
int gotoxy(int x, int y)
{
	char essq[100], xstr[100], ystr[100];

#if (DEBUG == 1)
	cout << __FUNCTION__ << endl;
	return 0;
#endif
	sprintf(xstr, "%d", x);
	sprintf(ystr, "%d", y);

	essq[0] = '\0';
	strcat(essq, "\033[");
	strcat(essq, ystr);
	strcat(essq, "d");

	strcat(essq, "\033[");
	strcat(essq, xstr);
	strcat(essq, "G");

	printf("%s", essq);

	return 0;
}
#endif

int display_actions::getRows() {
	struct winsize w;
	ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
	return w.ws_row;
}

int display_actions::getCols() {
	struct winsize w;
	ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
	return w.ws_col;
}

void display_actions::update_dimensions() {
	unsigned int w = getCols();
	unsigned int h = getRows();
	if((h != height) || (w != width)) {
		height = h;
		width = w;
		curr_row = 0;
		filler.clear();
		filler.resize(w, ' ');
	}
}

void display_actions::set_row(int row) 
{
#if(OS_LINUX==0)
	int clear = 0, i = 0;
#endif

#if (DEBUG == 1)
cout << __FUNCTION__ << endl;
	return;
#endif
	curr_row = ((unsigned int)row > height)? height: row;

#if(OS_LINUX==0)
	clear = (curr_row == 6)? 3: (curr_row == 19)? 8: 2;
	for(i=0;i<clear;i++)
	{	
		clear_row(curr_row + i);
	}
	
	regs.h.ah = 2;
	regs.h.bh = 0;

	regs.h.dh = curr_row;
	regs.h.dl = 0;
	int86(0x10,&regs,&regs);
#else
	clear_row(curr_row);
	gotoxy(0, curr_row);
#endif
}

void display_actions::clear_screen(void)
{
	unsigned int i = 1;

#if (DEBUG == 1)
cout << __FUNCTION__ << endl;
#endif
	
	while(i < height)
	{
		set_row(i);
		if(i == 1)
		{
			cout << "File organizer extraordinary. Compiled " << __DATE__; // << endl;
		}
		else if(i == 2)
		{
			cout << "Copyright Lari Koskinen."; // << endl;
		}
		else if(i == 3)
		{
			cout << "Brainsmash uLtd 1991 - 2013."; // << endl;
		}
#if (ID3 == 1)
		else if(i == 4)
		{
			cout << "Abusing ID3LIB 3.8.3. libraries. Used under LGPL. http://id3lib.sourceforge.net/"; // << endl;
		}
#endif
		else if(i == 14)
		{
			cout << "Latest Procedures:"; // << endl;
		}
		//else if(i == 14)
		//{
			//cout << "Total Changes Made:"; // << endl;
		//}
		i++;
	}
}

void display_actions::clear_row(int row)
{
#if (DEBUG == 1)
cout << __FUNCTION__ << endl;
	return;
#endif
#if(OS_LINUX==0)
	regs.h.ah = 2;
	regs.h.bh = 0;

	regs.h.dh = row;
	regs.h.dl = 0;
	int86(0x10,&regs,&regs);
#else
	gotoxy(0, row);
#endif
	cout << filler << endl;
}

void display_actions::set_dimension(int w, int h)
{
#if (DEBUG == 1)
cout << __FUNCTION__ << endl;
#endif
	width = w;
	height = h;
	curr_row = 0;
	filler.clear();
	filler.resize(w, ' ');
}

void display_actions::update_information(void)
{
#if (DEBUG == 1)
cout << __FUNCTION__ << endl;
#endif
	update_dimensions();
	set_row(19);
	cout << "Total Changes Made (time: " << (handler->timer.get_time_passed() / 1000) << " seconds):" << endl;
	cout << "Directories :";
	handler->directories.print_results();
	cout << "Files       :";
	handler->files.print_results();
}

std::string display_actions::fitToScreen(std::string data) {
	if(data.length() >= width) {
		data[width - 1] = 0;
	}
	return data;
}

void display_actions::update_directory(std::string data)
{
#if (DEBUG == 1)
cout << __FUNCTION__ << endl;
#endif
	set_row(7);
	//cout << setfill(' ');
	//cout << "Processing Directory: " << data << setw(width - 22 - data.length()) << " " << endl;
	cout << fitToScreen("Processing Directory: " + data) << endl;
}

void display_actions::update_subdirectory(std::string data)
{
#if (DEBUG == 1)
cout << __FUNCTION__ << endl;
#endif
	set_row(9);
	//cout << setfill(' ');
	//cout << "SUBDIRECTORY: " << data << setw(width - 12 - data.length()) << " " << endl;
	cout << fitToScreen("SUBDIRECTORY: " + data) << endl;
}

void display_actions::update_file(std::string data)
{
#if (DEBUG == 1)
cout << __FUNCTION__ << endl;
#endif
	set_row(11);
	//cout << setfill(' ');
	//cout << "Processing File: " << data << setw(width - 15 - data.length()) << " " << endl;
	cout << fitToScreen("Processing File: " + data) << endl;
}

void display_actions::update_last_procedure(std::string data)
{
#if (DEBUG == 1)
cout << __FUNCTION__ << endl;
#endif
	set_row(15);
	cout << data << endl;
	
	if(!last_procedure.empty())
	{
		set_row(17);
		cout << fitToScreen(last_procedure) << endl;
	}
	last_procedure = data;
}

void display_actions::debug_row(void)
{
	static int k = 0;

#if (DEBUG == 1)
cout << __FUNCTION__ << endl;
#endif
	set_row(26 + k++);
}

void display_actions::move_to_last_row(void)
{
#if (DEBUG == 1)
cout << __FUNCTION__ << endl;
#endif
	set_row(height);
}

void display_actions::change_row(void)
{
#if (DEBUG == 1)
cout << __FUNCTION__ << endl;
#endif
	set_row(curr_row+1);
}


