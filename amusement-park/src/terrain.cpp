
#include "terrain.hpp"

using namespace cgp;

mesh create_terrain_mesh()
{
	int const terrain_sample = 100;
	mesh terrain = mesh_primitive_grid({ -1,-1,0 }, { 1,-1,0 }, { 1,1,0 }, { -1,1,0 }, terrain_sample, terrain_sample);
	return terrain;
}

void update_terrain(mesh& terrain, mesh_drawable& terrain_visual, perlin_noise_parameters const& parameters)
{
	// Number of samples in each direction (assuming a square grid)
	int const N = std::sqrt(terrain.position.size());

	// Recompute the new vertices
	for (int ku = 0; ku < N; ++ku) {
		for (int kv = 0; kv < N; ++kv) {

			// Compute local parametric coordinates (u,v) \in [0,1]
			const float u = ku / (N - 1.0f);
			const float v = kv / (N - 1.0f);

			int const idx = ku * N + kv;

			// Compute the Perlin noise
			float const noise = noise_perlin({ u, v }, parameters.octave, parameters.persistency, parameters.frequency_gain);

			// use the noise as height value
			terrain.position[idx].z = parameters.terrain_height * noise;

			// use also the noise as color value
			terrain.color[idx] = 0.3f * vec3(0, 0.5f, 0) + 0.7f * noise * vec3(1, 1, 1);
		}
	}

	// Update the normal of the mesh structure
	terrain.compute_normal();

	// Update step: Allows to update a mesh_drawable without creating a new one
	terrain_visual.update_position(terrain.position);
	terrain_visual.update_normal(terrain.normal);
	terrain_visual.update_color(terrain.color);
}

void update_terrain(mesh& terrain, mesh& terrain_init, mesh_drawable& terrain_visual, float t)
{
	// Number of samples in each direction (assuming a square grid)
	int const N = std::sqrt(terrain.position.size());

	// Recompute the new vertices
	for (int ku = 0; ku < N; ++ku) {
		for (int kv = 0; kv < N; ++kv) {

			// Compute local parametric coordinates (u,v) \in [0,1]
			const float u = ku / (N - 1.0f);
			const float v = kv / (N - 1.0f);

			int const idx = ku * N + kv;

			// Compute the Perlin noise

			// use the noise as height value
			//std::cout << terrain_init.position[idx].z + 0.001f * std::sin(1.0f * terrain.position[idx].x + 1.0f * terrain.position[idx].y + 1.0f * t) + 0.015f * std::sin(0.7f * terrain.position[idx].x - 0.7f * terrain.position[idx].y + 2.0f * t) << " ";
			terrain.position[idx].z = std::max(terrain_init.position[idx].z + 0.001f*std::sin(1.0f * terrain.position[idx].x + 1.0f * terrain.position[idx].y + 1.0f * t) + 0.015f * std::sin(0.7f * terrain.position[idx].x - 0.7f * terrain.position[idx].y + 2.0f * t), 0.0f);
			terrain.uv[idx].x = terrain_init.uv[idx].x + 0.00f*t+ 0.01f * std::sin(1.0f * t);;
			terrain.uv[idx].y = terrain_init.uv[idx].y + 0.00f*t + 0.01f * std::sin(1.0f * t);;

			// use also the noise as color value
			//terrain.color[idx] = 0.3f * vec3(0, 0.5f, 0) + 0.7f * noise * vec3(1, 1, 1);
		}
	}

	// Update the normal of the mesh structure
	terrain.compute_normal();

	// Update step: Allows to update a mesh_drawable without creating a new one
	terrain_visual.update_position(terrain.position);
	terrain_visual.update_normal(terrain.normal);
	terrain_visual.update_color(terrain.color);
	terrain_visual.update_uv(terrain.uv);

}
