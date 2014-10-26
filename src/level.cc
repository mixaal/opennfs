#include <opennfs/level.h>
#include <opennfs/cloud.h>
#include <opennfs/skydome.h>

namespace game {

	namespace level {
		void Initializer::parse() 
		{
			configuration::CloudProperties cloud_properties(_level_name);
			configuration::SkyDomeProperties sky_properties(_level_name);
			configuration::types::all_clouds cloud_layers = cloud_properties.get();
			configuration::types::skydome skydome = sky_properties.get();

			//
			// Configure the cloud layers
		        //
			for(int idx=0; idx<cloud_properties.get_cloud_no(); idx++) {
				configuration::types::cloud _cloud = cloud_layers.at(idx);
				visualizer::CloudLayer *_cloud_layer = new visualizer::CloudLayer(
							_cloud.map_name,
							_cloud.scale,
							_cloud.altitude
				);
				_cloud_layer->set_air_humidity(_cloud.air_humidity);
				_cloud_layer->position(_cloud.position.x, _cloud.altitude, _cloud.position.z);
				_scene->add_clouds(_cloud_layer);
			}

			// Atmospehere
			_scene->add_atmosphere(
				new visualizer::SkyDome(
				50.0f, 15.0f,   /* latitude, longitude */
				_world_time->get_year(), 
				_world_time->get_month(), 
				_world_time->get_day(),
				_world_time->get_hour(),
				_world_time->get_minute(),
				_world_time->get_seconds(), 
				skydome.exposure,            /* exposure */
				skydome.turbidity,            /* turbidity */
				skydome.sunlight,
				skydome.radius
				)
			);

		}
	}
}
