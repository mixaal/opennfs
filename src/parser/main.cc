#include "nfs_path.h"
#include "fam.h"
#include <iostream>
#include <vector>
#include "bitmap.h"

int main(int argc, char *argv[])
{
	if(2==argc) {
		//nfs::GameConfig path_config(argv[1]);
		//std::cout << path_config.to_string() << std::endl;
		nfs::fam::parser parser(argv[1]);
		parser.start();
	}
	else if (3==argc) {
#if 1
		nfs::fam::parser parser(argv[2]); /* road texture data */
		nfs::fam::wwww_chunk *wwww = parser.start();
		nfs::fam::wwww_chunk *first_one = wwww->get_wwww_chunk_at(0);
		std::vector<nfs::fam::bitmap*> bmaps = parser.get_bitmaps(first_one);
		for(std::vector<nfs::fam::bitmap*>::iterator it=bmaps.begin(); it!=bmaps.end(); ++it) {
			nfs::fam::bitmap *b = *it;
			printf("GOT bitmap: %s \n", b->name().c_str());
		}
		nfs::fam::bitmap *all_textures = nfs::fam::bitmap::create_final_bitmap(bmaps);
#endif
		nfs::fam::trkd_chunk *track = new nfs::fam::trkd_chunk(
			new nfs::fstream(argv[1]),  /* road 3D data */
			first_one /* scenery bitmaps */
		);
		//track->skip_bytes(0x1b000);
		track->parse();
		track->serialize();
	}
	else {
		exit(-1);
	}
 	return 0;
}
