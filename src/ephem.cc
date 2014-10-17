/****************************************************************************
 * Copyright (C) 2014 Michal Conos 
 * 
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */
#include <opennfs/ephem.h>
#include <math.h>
#include <stdio.h>
namespace astronomy {

	// update the time and geographical position
	void Ephemerides::update(float _latitude, float _longitude, int _year, int _month, int _day, int _hour, int _minute, int _sec)
	{
		latitude = _latitude;
		longitude = _longitude;
		year = _year;
		month = _month;
		day = _day;
		hour = _hour;
		minute = _minute;
		sec = _sec;
		jd = julian_date(year, month, day);
		lst = local_siderial_time(jd, longitude, hour+minute/60.0f+sec/3600.0f);
	}

	float Ephemerides::julian_date(int year, int month, int day) {
		int y=year, m=month, D=day;
		if ( m < 2 ) {
			y--;
			m += 12;			
		}

		float a = floor (y/100);
		float b = 2 - a + floor(a/4);

		float jd1 = floor (365.25f*y) + floor(30.6001f*(m+1)) + D + 1720994.5f;
		float jd2 = jd1 + b;

		if ( ( year == 1582 && month >= 10 && day >= 15 ) || ( year >= 1582 ) ) {
			return jd2;
		} else {
			return jd1;
		}

	}

	// julian_date - Julian date obtained by julian_date() method call
	// longitude - longitude in hours here [0-24]
	// t - is UTC time in hours and fractions of hour, i.e.: t= h+m/60.0f+s/3600.0f;
	float Ephemerides::local_siderial_time(float julian_date, float longitude, float t)
	{
		double T3, S0, S, s;

		T3 = (julian_date - 2451545.0)/36525;
		//System.out.println ("T3="+T3);
		S0 = 6.697374558 + 2400.05133691*T3 + 0.0000258622*T3*T3 - 0.0000000017*T3*T3*T3  ;

		//System.out.println ("S0="+S0);
		S0 -= floor (S0 / 24)*24;
		//System.out.println ("S0="+S0);
		if (S0 < 0) S0+=24;
		//System.out.println ("S0="+S0);

		//System.out.println( "t="+t);

		S = S0 + 1.0027379093*t;
		if (S > 24) S-=24;
		s = S + longitude;

		return s;

	}

	// ascension and declination to the observer's azimuth and elevation
	void Ephemerides::eq_to_observer(double ascension, double declination, double *az, double *elevation)
	{
		float local_time = -ascension + lst * 15 * M_PI / 180.0f;
		transform(declination, M_PI_2 - local_time, (latitude - 90) * M_PI / 180, elevation, az);
	}

	void Ephemerides::transform(double D, double L, double e, double *d, double *l)
	{
		double cos_e = cos(e);
		double sin_e = sin(e);
		double sin_D = sin(D);
		double cos_D = cos(D);
		double cos_L = cos(L);
		double cos_D_sin_L = cos_D * sin(L);

		double y  = cos_D * cos_L;
		double x  =  sin_e * sin_D + cos_e * cos_D_sin_L;
		double sin_d = cos_e * sin_D - sin_e * cos_D_sin_L;
		double tan_l = y / x;

		*d = asin (sin_d);
		*l = atan2 ( y, x );
		//if ( x < 0 ) l += M_PI;

	}

	// Compute sun altitude (eleveation above horizon) and azimuth (both in degrees)
	// Original pseudocode from:
	//    http://aa.usno.navy.mil/faq/docs/SunApprox.php
	void Ephemerides::sun_coords(double *alt, double *az) {
		//printf("!!! [%d %d %d %d %d %d]\n", year, month, day, hour, minute, sec);
		float JD = julian_date(year, month, day);
		float D = JD - 2451545.0f;
		//Mean anomaly of the Sun (in degrees)
		double g = 357.529 + 0.98560028 * D;
		g=fmod(g, 360.0);
		//Mean longitude of the Sun: (in degrees)
		double q = 280.459 + 0.98564736 * D;
		q=fmod(q, 360.0);
		g *= M_PI/180.0f;
		//Geocentric apparent ecliptic longitude of the Sun (adjusted for aberration) (in degrees)
		double L = q + 1.915 * sin(g) + 0.020 * sin (2*g);
		L=fmod(L, 360.0);
		q *= M_PI/180.0f;
		L *= M_PI/180.0f;
		// mean obliquity of the ecliptic, in degrees
		double e = 23.439 - 0.00000036 * D;
		e *= M_PI/180.0f;
		//ascension and declination
		double d = asin(sin(e)* sin(L));
		double RA = atan2( cos (e) * sin (L),  cos (L));

		//printf("[RA=%f d=%f]\n", RA, d);

		// local sidereal time
		float t = hour + minute/60.0f +sec/3600.0f;
		float lst = local_siderial_time(JD, longitude/24.0f, t);
		float local_time = -RA + lst * 15 * M_PI / 180.0f;
		//printf("local_time = %f\n", local_time);

		// transform ascension and declination into observer's azimuth and elevation (altitude)
		transform(d, M_PI_2 - local_time, (latitude - 90) * M_PI / 180, alt, az);
	}



}
