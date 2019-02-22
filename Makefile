OBJECTS = main.o comparison_list.o conversion_counter.o file_handlers.o parsers.o display.o file_options.o file_information.o \
	  dir_handlers.o track_list.o time_functions.o id3.o

CPPFLAGS = -Wall

TARGET=linux
#TARGET=dos

ifeq ($(TARGET), linux)
	CC = g++
	REMOVE=rm
	APPLICATION_NAME=organizer
	CPPFLAGS+=-DOS_LINUX=1 -DDEBUG=0 -DID3=1
	EXTRALIBS=lib/libid3.a
else
	CC = gpp
	REMOVE=del
	APPLICATION_NAME=organizer.exe
	CPPFLAGS+=-DOS_LINUX=0 -DID3LIB_LINKOPTION=3 -DDEBUG=0
	#EXTRALIBS=--dllname lib/id3lib.dll
	#EXTRALIBS=-L./lib -lid3lib
endif

all:$(OBJECTS)
	$(CC) $(CPPFLAGS) $(INCLUDES) $(OBJECTS) -o $(APPLICATION_NAME) $(EXTRALIBS)
	@echo Compiled $(APPLICATION_NAME) for $(TARGET)

clean:
	$(REMOVE) *.o 
	$(REMOVE) $(APPLICATION_NAME)

