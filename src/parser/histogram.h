#ifndef _HISTOGRAM_H_
#define _HISTOGRAM_H_ 1

namespace nfs {
namespace adt {

#include <vector>
#include <string>

class histogram_element {
	public:
		histogram_element() : _occurence(0), _cdf(0), _scaled(0) { }
		size_t cdf() { return _cdf; }
		size_t occurence() { return _occurence; }
		size_t scaled()    { return _scaled; }
		void increase_occurence() {
			++_occurence;
		}
		void calc_cdf(size_t last_cdf) {
			_cdf = last_cdf + _occurence;
		}
		void equalize(size_t pixels, size_t cdf_min, size_t max_value) {
			printf("histogram_element::equalize(): pixels=%lu _cdf=%lu cdf_min=%lu max_value=%lu\n", pixels, _cdf, cdf_min, max_value);
			_scaled = (size_t)floor(max_value * (float)(_cdf - cdf_min)/(pixels-cdf_min));	
			printf("histogram_element::equalize(): scaled=%lu\n", _scaled);
		}
		std::string to_string() {
			std::string x=std::to_string(_occurence) + ":";
			for(size_t idx=0; idx<_occurence/170; idx++) {
				x+="*";
			}
			if(_occurence >0 && _occurence<170) x+="*";
			return x;
		}

		std::string cdf_string() {
			std::string x=std::to_string(_cdf) + ":";
			for(size_t idx=0; idx<_cdf/500; idx++) {
				x+="*";
			}
			if(_cdf > 0 && _cdf<500) x+="*";
			return x;
		}
		std::string eq_string() {
			std::string x=std::to_string(_scaled) + ":";
			for(size_t idx=0; idx<_scaled/4; idx++) {
				x+="*";
			}
			if(_scaled>0 && _scaled<4) x+="*";
			return x;
		}


	private:

		size_t _occurence;
		size_t _cdf;
		size_t _scaled;
};

class channel_histogram {
public:
	channel_histogram(size_t bits=8) {
		_max_value = (1<<bits) - 1;
		_cdf_min = 0;
		for (size_t i=0; i<=_max_value; i++) {
			_hist.push_back(new histogram_element());
		}
	}
	void increase_occurence(size_t value) {
		_hist.at(value)->increase_occurence();
	}
	void cdf() {
		size_t cdf = 0;
		for(std::vector<histogram_element*>::iterator it = _hist.begin(); it != _hist.end(); ++it) {
			histogram_element *he = *it;
			he->calc_cdf(cdf);
			cdf = he->cdf();
			printf("cdf=%lu\n", cdf);
			if(_cdf_min == 0) {
				_cdf_min = cdf;
			}
			else {
				_cdf_min = std::min(_cdf_min, cdf);
			}
		}
		printf("cdf_min=%lu\n", _cdf_min);
	}

	void equalize(size_t pixels) {
		size_t prev=0;
		for(std::vector<histogram_element*>::iterator it = _hist.begin(); it != _hist.end(); ++it) {
			histogram_element *he = *it;
			he->equalize(pixels, _cdf_min, _max_value);
			printf("prev=%lu eq=%lu _cdf_min=%lu _max_value=%lu\n", prev++, he->scaled(), _cdf_min, _max_value);
		}
	}

	size_t equalized_value_for(size_t idx) {
		printf("EQV: %lu --->  %lu\n", idx, _hist.at(idx)->scaled());
		return _hist.at(idx)->scaled();
	}

	std::string to_string() {
		std::string out="";
		size_t idx=0;
		for(std::vector<histogram_element*>::iterator it = _hist.begin(); it != _hist.end(); ++it) {
			histogram_element *he = *it;
			out+=std::string("\n")+std::to_string(idx)+std::string(":") + he->to_string();
			idx++;
		}
		return out;
		
	}

	std::string cdf_string() {
		std::string out="";
		size_t idx=0;
		for(std::vector<histogram_element*>::iterator it = _hist.begin(); it != _hist.end(); ++it) {
			histogram_element *he = *it;
			out+=std::string("\n")+std::to_string(idx)+std::string(":") + he->cdf_string();
			idx++;
		}
		return out;
		
	}

	std::string eq_string() {
		std::string out="";
		size_t idx=0;
		for(std::vector<histogram_element*>::iterator it = _hist.begin(); it != _hist.end(); ++it) {
			histogram_element *he = *it;
			out+=std::string("\n")+std::to_string(idx)+std::string(":") + he->eq_string();
			idx++;
		}
		return out;
		
	}


private:
	std::vector<histogram_element*> _hist;
	size_t _cdf_min=0;
	size_t _max_value;
	

};

}
}


#endif /* _HISTOGRAM_H_ */
