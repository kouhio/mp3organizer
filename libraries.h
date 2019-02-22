#ifndef __LIBRARIES_H__
#define __LIBRARIES_H__

#include "conversion_counter.h"
#include "comparison_list.h"
#include "file_handlers.h"
#include "parsers.h"
#include "display.h"
#include "file_options.h"
#include "dir_handlers.h"
#include "track_list.h"
#include "time_functions.h"

#if (ID3 == 1)
#include "id3.h"
#endif

#if (OS_LINUX == 1)
#define DIR_MARK '/'
#define DIR_REPLACE '\\'
#define DIR_REPLACEMENT "/"
#else
#define DIR_MARK '\\'
#define DIR_REPLACE '/'
#define DIR_REPLACEMENT "\\"
#endif

using namespace std;

extern size_t npos;
extern comparison_list *handler;

#endif 

