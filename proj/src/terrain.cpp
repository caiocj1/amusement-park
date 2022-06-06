
#include "terrain.hpp"

using namespace cgp;

// Wave parameters

vec2 wave_vecs[5];
float ampls[5];

void initialize_waves()
{
	for (int i = 0; i < 5; i++)
	{
		wave_vecs[i] = { rand_interval(-0.8, 0.8), rand_interval(-0.8, 0.8)};
		ampls[i] = rand_interval(0.1, 0.3);
	}
}

mesh create_terrain_mesh()
{
	int const terrain_sample = 100;
	mesh terrain = mesh_primitive_grid({ -200,-200,0 }, { 200,-200,0 }, { 200,200,0 }, { -200,200,0 }, terrain_sample, terrain_sample);
	return terrain;
}

mesh create_flag_mesh()
{
	int const flag_sample = 100;
	mesh flag = mesh_primitive_grid({ 0,0,0 }, { 0,5,-1}, { 0,5,4}, { 0,0,5}, flag_sample, flag_sample);
	return flag;
}

mesh create_thunder()
{
	int const flag_sample = 10;
	mesh flag = mesh_primitive_grid({ 0,0,0 }, { 0,5,0 }, { 0,5,3 }, { 0,0,3 }, flag_sample, flag_sample);
	return flag;
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

void update_flag(mesh& flag, mesh& flag_init, mesh_drawable& flag_visual, float t)
{
	// Number of samples in each direction (assuming a square grid)
	int const N = std::sqrt(flag.position.size());

	// Recompute the new vertices
	for (int ku = 0; ku < N; ++ku) {
		for (int kv = 0; kv < N; ++kv) {

			// Compute local parametric coordinates (u,v) \in [0,1]
			const float u = ku / (N - 1.0f);
			const float v = kv / (N - 1.0f);

			int const idx = ku * N + kv;

			flag.position[idx].x = flag_init.position[idx].x + 1.0f * (0.05f + 0.3f*flag.position[idx].y) * std::sin(2.0f * flag.position[idx].y + 4.0f * t);
		}
	}

	// Update the normal of the mesh structure
	flag.compute_normal();

	// Update step: Allows to update a mesh_drawable without creating a new one
	flag_visual.update_position(flag.position);
	flag_visual.update_normal(flag.normal);
	flag_visual.update_color(flag.color);
	flag_visual.update_uv(flag.uv);
}

void gerstner_waves(mesh& terrain, mesh& terrain_init, mesh_drawable& terrain_visual, float t)
{
	int const N = std::sqrt(terrain.position.size());

	for (int ku = 0; ku < N; ++ku) {
		for (int kv = 0; kv < N; ++kv) {
			const float u = ku / (N - 1.0f);
			const float v = kv / (N - 1.0f);

			int const idx = ku * N + kv;

			terrain.position[idx].z = terrain_init.position[idx].z;
			for (int i = 0; i < 5; i++)
				terrain.position[idx].z += ampls[i] *
				cos(dot(wave_vecs[i], { terrain_init.position[idx].x, terrain_init.position[idx].y }) - sqrt(9.8 * norm(wave_vecs[i])) * t);

			terrain.position[idx].x = terrain_init.position[idx].x;
			for (int i = 0; i < 5; i++)
				terrain.position[idx].x -= (wave_vecs[i][0] / norm(wave_vecs[i])) * ampls[i] *
				cos(dot(wave_vecs[i], { terrain_init.position[idx].x, terrain_init.position[idx].y }) - sqrt(9.8 * norm(wave_vecs[i])) * t);

			terrain.position[idx].y = terrain_init.position[idx].y;
			for (int i = 0; i < 5; i++)
				terrain.position[idx].y -= (wave_vecs[i][1] / norm(wave_vecs[i])) * ampls[i] *
				cos(dot(wave_vecs[i], { terrain_init.position[idx].x, terrain_init.position[idx].y }) - sqrt(9.8 * norm(wave_vecs[i])) * t);

			terrain.color[idx] = vec3(0.01, 0.15, 0.28) + vec3(0.8, 0.7, 0.5) * (terrain.position[idx].z - terrain_init.position[idx].z) / 20;
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
