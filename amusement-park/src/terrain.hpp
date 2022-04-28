#pragma once

#include "cgp/cgp.hpp"

float evaluate_terrain_height(float x, float y);

cgp::mesh create_terrain_mesh(int N, float length);