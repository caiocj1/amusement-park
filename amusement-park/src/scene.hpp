#pragma once

#include "cgp/cgp.hpp"
#include "shadow_map.hpp"
#include "terrain.hpp"

struct gui_parameters {
	bool display_frame = true;
	bool display_wireframe = false;
	bool animated_shapes = true;
	float speed = 1.0f;
};

struct scene_structure {

	cgp::mesh_drawable global_frame;          // The standard global frame
	scene_environment_shadow_map environment; // Standard environment controler
	cgp::inputs_interaction_parameters inputs; // Storage for inputs status (mouse, keyboard, window dimension)

	shadow_map_structure shadow_map;

	cgp::timer_basic timer;

	gui_parameters gui;                       // Standard GUI element storage

	cgp::skybox_drawable skybox;

	cgp::mesh water_mesh;

	cgp::mesh_drawable terrain;
	cgp::mesh_drawable water;
	cgp::mesh_drawable sphere_light; //used to display the position of the light

	perlin_noise_parameters parameters;

	float last_wait=0;

	void update_camera();

	void initialize();  // Standard initialization to be called before the animation loop
	void display();     // The frame display to be called within the animation loop
	void display_gui(); // The display of the GUI, also called within the animation loop
	void reset_mouse(GLFWwindow* window);

};