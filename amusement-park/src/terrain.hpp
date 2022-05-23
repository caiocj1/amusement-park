#pragma once

#include "cgp/cgp.hpp"

struct perlin_noise_parameters
{
	float persistency = 0.1f;
	float frequency_gain = 0.02f;
	int octave = 6;
	float terrain_height = 0.1f;
};


// Initialize the mesh of the terrain
cgp::mesh create_terrain_mesh();

// Recompute the vertices of the terrain everytime a parameter is modified
//  and update the mesh_drawable accordingly
void update_terrain(cgp::mesh& terrain, cgp::mesh_drawable& terrain_visual, perlin_noise_parameters const& parameters);
void update_terrain(cgp::mesh& terrain, cgp::mesh_drawable& terrain_visual, float t);
