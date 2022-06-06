
#include "terrain.hpp"

using namespace cgp;

// Wave parameters

vec2 wave_vecs[10];
float ampls[10];

void initialize_waves()
{
	for (int i = 0; i < 10; i++)
	{
		wave_vecs[i] = { rand_interval(-0.4, 0.4), rand_interval(-0.4, 0.4)};
		ampls[i] = rand_interval(0.1, 0.5);
	}
}

mesh create_terrain_mesh()
{
	int const terrain_sample = 80;
	mesh terrain = mesh_primitive_grid({ -200,-200,0 }, { 200,-200,0 }, { 200,200,0 }, { -200,200,0 }, terrain_sample, terrain_sample);
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

void gerstner_waves(mesh& terrain, mesh& terrain_init, mesh_drawable& terrain_visual, float t)
{
	int const N = std::sqrt(terrain.position.size());


	for (int ku = 0; ku < N; ++ku) {
		for (int kv = 0; kv < N; ++kv) {
			const float u = ku / (N - 1.0f);
			const float v = kv / (N - 1.0f);

			int const idx = ku * N + kv;

			/*terrain.position[idx].z = terrain_init.position[idx].z +
				ampl1 * cos(dot(k1, { terrain_init.position[idx].x, terrain_init.position[idx].y }) - sqrt(9.8 * norm(k1)) * t) +
				ampl2 * cos(dot(k2, { terrain_init.position[idx].x, terrain_init.position[idx].y }) - sqrt(9.8 * norm(k2)) * t) +
				ampl3 * cos(dot(k3, { terrain_init.position[idx].x, terrain_init.position[idx].y }) - sqrt(9.8 * norm(k3)) * t) +
				ampl4 * cos(dot(k4, { terrain_init.position[idx].x, terrain_init.position[idx].y }) - sqrt(9.8 * norm(k4)) * t);*/
			terrain.position[idx].z = terrain_init.position[idx].z;
			for (int i = 0; i < 10; i++)
				terrain.position[idx].z += ampls[i] *
				cos(dot(wave_vecs[i], { terrain_init.position[idx].x, terrain_init.position[idx].y }) - sqrt(9.8 * norm(wave_vecs[i])) * t);


			/*terrain.position[idx].x = terrain_init.position[idx].x -
				((k1[0] / norm(k1)) * ampl1 * sin(dot(k1, {terrain_init.position[idx].x, terrain_init.position[idx].y}) - sqrt(9.8 * norm(k1)) * t) +
				(k2[0] / norm(k2)) * ampl2 * sin(dot(k2, { terrain_init.position[idx].x, terrain_init.position[idx].y }) - sqrt(9.8 * norm(k2)) * t) +
				(k3[0] / norm(k3)) * ampl3 * sin(dot(k3, { terrain_init.position[idx].x, terrain_init.position[idx].y }) - sqrt(9.8 * norm(k3)) * t) +
				(k4[0] / norm(k4)) * ampl4 * sin(dot(k4, { terrain_init.position[idx].x, terrain_init.position[idx].y }) - sqrt(9.8 * norm(k4)) * t));*/
			terrain.position[idx].x = terrain_init.position[idx].x;
			for (int i = 0; i < 10; i++)
				terrain.position[idx].x -= (wave_vecs[i][0] / norm(wave_vecs[i])) * ampls[i] *
				cos(dot(wave_vecs[i], { terrain_init.position[idx].x, terrain_init.position[idx].y }) - sqrt(9.8 * norm(wave_vecs[i])) * t);

			/*terrain.position[idx].y = terrain_init.position[idx].y -
				((k1[1] / norm(k1)) * ampl1 * sin(dot(k1, {terrain_init.position[idx].x, terrain_init.position[idx].y}) - sqrt(9.8 * norm(k1)) * t) +
				(k2[1] / norm(k2)) * ampl2 * sin(dot(k2, { terrain_init.position[idx].x, terrain_init.position[idx].y }) - sqrt(9.8 * norm(k2)) * t) +
				(k3[1] / norm(k3)) * ampl3 * sin(dot(k3, { terrain_init.position[idx].x, terrain_init.position[idx].y }) - sqrt(9.8 * norm(k3)) * t) +
				(k4[1] / norm(k4)) * ampl4 * sin(dot(k4, { terrain_init.position[idx].x, terrain_init.position[idx].y }) - sqrt(9.8 * norm(k4)) * t));*/
			terrain.position[idx].y = terrain_init.position[idx].y;
			for (int i = 0; i < 10; i++)
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
