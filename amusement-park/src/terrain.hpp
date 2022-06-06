#pragma once

#include "cgp/cgp.hpp"

struct perlin_noise_parameters
{
	float persistency = 0.01f;
	float frequency_gain = 0.3f;
	int octave = 6;
	float terrain_height = 0.1f;
};

// Initialize the mesh of the terrain
cgp::mesh create_terrain_mesh();

cgp::mesh create_flag_mesh();

// Recompute the vertices of the terrain everytime a parameter is modified
//  and update the mesh_drawable accordingly
void update_terrain(cgp::mesh& terrain, cgp::mesh_drawable& terrain_visual, perlin_noise_parameters const& parameters);
<<<<<<< HEAD
void update_flag(cgp::mesh& flag, cgp::mesh& flag_init, cgp::mesh_drawable& flag_visual, float t);
=======
void update_terrain(cgp::mesh& terrain, cgp::mesh& terrain_init, cgp::mesh_drawable& terrain_visual, float t);

void initialize_waves();
>>>>>>> 97d70ef8bdb39743f919f63e5b024f1050206471
void gerstner_waves(cgp::mesh& terrain, cgp::mesh& terrain_init, cgp::mesh_drawable& terrain_visual, float t);
