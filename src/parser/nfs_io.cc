#include <cstdint>
#include "nfs_io.h"
#include <cstdio>

namespace nfs {

u_int32_t fstream::get_uint32() {
	char buffer[4];
	_fstream->seekg(position, std::ios_base::beg);
	_fstream->read(buffer, sizeof(buffer));
	position += 4;
	return (unsigned char)buffer[0] + ((unsigned char)buffer[1]<<8) + ((unsigned char)buffer[2]<<16) + ((unsigned char)buffer[3]<<24);
}

u_int16_t fstream::get_uint16() {
	char buffer[2];
	_fstream->seekg(position, std::ios_base::beg);
	_fstream->read(buffer, sizeof(buffer));
	position += 2;
	return (unsigned char)buffer[0] + ((unsigned char)buffer[1]<<8);
}

u_int8_t fstream::get_uint8() {
	char buffer[1];
	_fstream->seekg(position, std::ios_base::beg);
	_fstream->read(buffer, sizeof(buffer));
	position ++;
	return (u_int8_t)buffer[0];
}

std::string fstream::get_str(size_t len) {
	char buffer[len+1];
	printf("this=%p _fstream=%p\n", this, _fstream);
	buffer[len] = '\0';
	printf("_position=%lx\n", position);
	//std::fstream *_fcopy = new std::fstream(_path,_mode);
	//_fcopy->seekg(position, std::ios_base::beg);
	//_fcopy->read(buffer, len);
	_fstream->seekg(position, std::ios_base::beg);
	_fstream->read(buffer, len);
	position += len;
	printf("path=%s: _position=%lx len=%lx %u %u %u %u\n", _path.c_str(), position, len, buffer[0], buffer[1], buffer[2], buffer[3]);
	return std::string(buffer);
}
void fstream::read_bytes(char **buffer, size_t len)
{
	*buffer = new char[len];
	_fstream->read(*buffer, len);
}

void fstream::close() 
{
	_fstream->close();
}

fstream *fstream::clone() 
{
	nfs::fstream *dup = new nfs::fstream(_path, _mode);
	printf("clone(): dup=%p path=%s stream=%p\n", dup, dup->path().c_str(), dup->get_stream());
	dup->position = this->position;
	return dup;
}

size_t fstream::get_size() 
{
	size_t save_position = _fstream->tellg();
	_fstream->seekg(0, std::ios_base::end);
	size_t  file_sz = _fstream->tellg();
	_fstream->seekg(save_position, std::ios_base::beg);
	return file_sz;
}

}
