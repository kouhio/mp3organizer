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
#endif

#include <dirent.h>
#include <sys/stat.h>

#ifdef __cplusplus
}
#endif

#include "libraries.h"

void conversion_counter::add_renamed(void)
{
#if (DEBUG == 1)
cout << __FUNCTION__ << endl;
#endif
	renamed++;
}

void conversion_counter::add_deleted(void)
{
#if (DEBUG == 1)
cout << __FUNCTION__ << endl;
#endif
	deleted++;
}

void conversion_counter::add_moved(void)
{
#if (DEBUG == 1)
cout << __FUNCTION__ << endl;
#endif
	moved++;
}

void conversion_counter::add_id3_removed(void)
{
#if (DEBUG == 1)
cout << __FUNCTION__ << endl;
#endif
	id3_rem++;
}

void conversion_counter::add_id3_added(void)
{
#if (DEBUG == 1)
cout << __FUNCTION__ << endl;
#endif
	id3_add++;
}

void conversion_counter::add_read_only(void)
{
#if (DEBUG == 1)
cout << __FUNCTION__ << endl;
#endif
	read_only++;
}

void conversion_counter::add_unknown(void)
{
#if (DEBUG == 1)
cout << __FUNCTION__ << endl;
#endif
	unknown++;
}

void conversion_counter::print_results(void)
{
#if (DEBUG == 1)
cout << __FUNCTION__ << endl;
#endif
	cout << setfill('0');
	cout << " Total:" << setw(6) << checked;
	cout << " | Renamed:" << setw(6) << renamed;
	cout << " | Deleted:" << setw(6) << deleted;
	cout << " | Moved:" << setw(6) << moved;

	if(unknown + read_only) {
		cout << endl;
		cout << "            :";
	}
	if(unknown) {
		cout << " Unknown files found:" << setw(6) << unknown;
	}
	if(read_only) {
		if(read_only) {
			cout << " |";
		}
		cout << " Read only files (failed):" << setw(6) << read_only;
	}

	if(id3_rem + id3_add + read_only) {
		cout << endl;
		cout << "ID3         :";
	}
	if(id3_rem) {
		cout << " ID3's removed:" << setw(6) << id3_rem;
	}
	if(id3_add) {
		if(id3_rem) {
			cout << " |";
		}
		cout << " ID3's added:" << setw(6) << id3_add;
	}
	cout << endl;
}

void conversion_counter::reset_counters(void)
{
#if (DEBUG == 1)
cout << __FUNCTION__ << endl;
#endif
	renamed = 0;
	deleted = 0;
	moved = 0;
	id3_rem = 0;
	id3_add = 0;
	unknown = 0;
	checked = 0;
	read_only = 0;
}

void conversion_counter::add_checked(void)
{
	checked++;
}


