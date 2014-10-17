#ifndef _EPHEM_H_
#define _EPHEM_H_

namespace astronomy {

#ifndef M_PI
# define M_PI		3.14159265358979323846	/* pi */
#endif
#ifndef M_PI_2
# define M_PI_2		1.57079632679489661923	/* pi/2 */
#endif


	class Ephemerides {
		public:
			Ephemerides(float latitude, float longitude, int year, int month, int day, int hour, int minute, int sec) 
				: latitude(latitude), longitude(longitude), year(year), month(month), day(day), hour(hour), minute(minute), sec(sec), 
				jd(julian_date(year, month, day)), 
				lst(local_siderial_time(julian_date(year, month, day), longitude, hour+minute/60.0f+sec/3600.0f)) {
				}  

			// compute sun alt and azimuth (degrees)
			void sun_coords(double *alt, double *az);
			// ascension and declination to the observer's azimuth and elevation
			void eq_to_observer(double ascension, double declination, double *azimuth, double *elevation);
			// update the time and geographical position
			void update(float latitude, float longitude, int year, int month, int day, int hour, int minute, int sec);
		private:
			// compute the julian date
			float julian_date(int year, int month, int day);
			// local sideral time from julian_date, longitude [hrs] and UTC time [hrs]
			float local_siderial_time(float julian_date, float longitude, float t);
			// trasnformation between different systems
			void transform(double D, double L, double e, double *d, double *l);
			float latitude, longitude;
			int year, month, day;
			int hour, minute, sec;
			float jd;
			float lst;
	};

}


#endif /* _EPHEM_H_ */
