#ifndef _OPENNFS_WORLD_TIME_H
#define _OPENNFS_WORLD_TIME_H 

namespace game {

class WorldTime {
	public:
		WorldTime(int year, int month, int day, int hour, int minute, int second)
			: _year(year), _month(month), _hour(hour), _minute(minute), _second(second) {
			_time_vector = 60; // 60seconds forward on update
		}

		void update();
		void set_time_vector(int vector);
		void freeze();
		void forward();
		void backward();

		int get_year();	
		int get_month();
		int get_day();
		int get_hour();
		int get_minute();
		int get_seconds();
	private:
		int _year, _month, _day;
		int _hour, _minute, _second;
		int _time_vector;
};

}





#endif /* _OPENNFS_WORLD_TIME_H */
