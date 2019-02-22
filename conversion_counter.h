#ifndef __CONVERSION_COUNTER_H__
#define __CONVERSION_COUNTER_H__

class conversion_counter
{
	public:
		void add_renamed(void);
		void add_deleted(void);
		void add_moved(void);
		void add_id3_removed(void);
		void add_id3_added(void);
		void add_unknown(void);
		void print_results(void);
		void reset_counters(void);
		void add_checked(void);
		void add_read_only(void);
	private:
		int renamed;
		int deleted;
		int moved;
		int id3_rem;
		int id3_add;
		int read_only;
		int unknown;
		int checked;
};

#endif 

