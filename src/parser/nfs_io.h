#ifndef _NFS_IO_H_
#define _NFS_IO_H_ 

#include <iostream>
#include <fstream>
#include <stdexcept>
#include <algorithm>
#include <cstdint>

namespace nfs {

class fstream {

public:
	/**
	 * @brief ignorecase version of fstream
	 * @param path filename to open
	 * @param mode in mode
	 */
	fstream(
		std::string path,  
		std::ios_base::openmode mode = std::ios::in | std::ios::binary
	) : _path(path), _mode(mode) {
		std::replace( path.begin(), path.end(), '\\', '/');
		_fstream = new std::fstream(path, mode);
		if( !_fstream->is_open() ) {
			std::cerr << "Error opening file: " << path << std::endl;
			throw std::runtime_error("can't open file!");
		}
		printf("new: _fstream=%p\n", _fstream);
		position = 0;
	}

	fstream() { }  // need for null_fstream
	
	u_int32_t get_uint32();
	u_int16_t get_uint16();
	u_int8_t get_uint8();
	std::string get_str(size_t len);
	/**
	 * @brief aka C# position for streams
	 * @param position set the position in the stream
	 */
	size_t position;
	void close();
	void read_bytes(char **buffer, size_t len);
	virtual fstream *clone();
	std::fstream *get_stream() { return _fstream; }
	std::string path() {
		return _path;
	}

	std::ios_base::openmode mode() {
		return _mode;
	}
	size_t get_size();
	
private:
	std::fstream *_fstream=NULL;
	std::string _path="";
	std::ios_base::openmode _mode;

};

class null_fstream : public fstream {
	public:
		null_fstream() { printf("NULL STREAM : %p!\n", this); }
		fstream *clone() {
			printf("ALWAYS NULL!\n");
			return this;
		}
		
};


}


#endif /* _NFS_IO_H_ */
