#ifndef __DISPLAY_H__
#define __DISPLAY_H__

class display_actions
{
	public:
		void set_row(int row);
		void clear_screen(void);
		void clear_row(int row);
		void set_dimension(int w, int h);
		void update_information(void);
		void update_directory(std::string data);
		void update_subdirectory(std::string data);
		void update_file(std::string data);
		void update_last_procedure(std::string data);
		void debug_row(void);
		void debug_row(std::string data);
		void move_to_last_row(void);
		void change_row(void);
		void update_dimensions();
	private:
		unsigned int height;
		unsigned int width;
		unsigned int curr_row;
		std::string filler;
		std::string last_procedure;

		int getRows();
		int getCols();
		std::string fitToScreen(std::string data);
};


#endif

