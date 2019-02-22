#ifndef __TIME_FUNCTIONS_H__
#define __TIME_FUNCTIONS_H__

class timer_functions
{
	public:
		unsigned long long TickCount(void);
		void delay_ms(unsigned long delay_time);
		void reset_timers(void);
		unsigned long long get_time_passed(void);

	private:
		unsigned long long current_time;
		unsigned long long start_time;
};

#endif

