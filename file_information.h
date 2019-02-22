#ifndef __FILE_INFORMATION_H__
#define __FILE_INFORMATION_H__

#include <string>

#include "comparison_list.h"
#include "conversion_counter.h"

class file_information
{
	public:
		file_information();
		~file_information();

		void parse_album_information(std::string data);
		void parse_track_information(std::string data);
		void parseSingleTrackInformation(std::string data);
		void parseSingleMiscInformation(std::string data);
		void reset_information(void);

		std::string get_band_name(void);
		std::string get_album_name(void);
		std::string get_year(void);

		int get_track_number(void);
		int get_track_count(void);
		std::string get_track_name(void);
		void setOriginal(void);
		void resetOriginal(void);

	private:
		std::string band_name;
		std::string album_name;
		std::string year;

		std::string track_name; 

		std::string tmp_band_name;
		std::string tmp_album_name;
		std::string tmp_track_name;

		int track_number;
		int track_count;
};

#endif

