#ifndef _OPENNFS_LEVEL_H_
#define _OPENNFS_LEVEL_H_

#include <string>
#include <opennfs/config_util.h>
#include <opennfs/scene.h>

namespace game {
	namespace level {
		class Initializer {
			public:
				Initializer(visualizer::Scene *scene, std::string level_name, WorldTime *world_time) : _scene(scene), _level_name(level_name), _world_time(world_time) {
					parse();
				}
				
			private:
				void parse();
				visualizer::Scene *_scene;
				std::string _level_name;
				WorldTime *_world_time;
		};
	}
}


#endif /* _OPENNFS_LEVEL_H_ */
