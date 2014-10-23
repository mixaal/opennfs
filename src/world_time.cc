
#include <stdio.h>
#include <opennfs/world_time.h>

namespace game {

	void WorldTime::update() 
	{
		_second += _time_vector;
		_day += _time_vector / 86400;
		_hour += _time_vector / 3600;
		_minute += _time_vector / 60;
		_hour %= 24;
		_minute %= 60;
		_second %= 60;
		//printf("%04d/%02d/%02d T %02d:%02d:%02d\n", _year, _month, _day, _hour, _minute, _second);
	}

	void WorldTime::set_time_vector(int time_vector)
	{
		_time_vector = time_vector;
	}	
	
	void WorldTime::freeze()
	{
		set_time_vector(0);
	}

	void WorldTime::forward() 
	{
		set_time_vector(60);
	}	

	void WorldTime::backward()
	{
		set_time_vector(-60);
	}

	int WorldTime::get_year() 
	{
		return _year;
	}

	int WorldTime::get_month()
	{
		return _month;
	}

	int WorldTime::get_day()
	{
		return _day;
	}

	int WorldTime::get_hour()
	{
		return _hour;
	}

	int WorldTime::get_minute()
	{
		return _minute;
	}

	int WorldTime::get_seconds()
	{
		return _second;
	}

}
