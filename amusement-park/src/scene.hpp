#pragma once

#include "cgp/cgp.hpp"
//#include "shadow_map.hpp"
#include "multiple_lights/multiple_lights.hpp"
#include "terrain.hpp"

struct gui_parameters {
	bool display_frame = true;
	bool display_wireframe = true;
	bool animated_shapes = true;
	float speed = 2.0f;
};

struct scene_structure {

	cgp::mesh_drawable global_frame;          // The standard global frame
	scene_environment_with_multiple_lights environment; // Standard environment controler
	cgp::inputs_interaction_parameters inputs; // Storage for inputs status (mouse, keyboard, window dimension)

	//shadow_map_structure shadow_map;

	cgp::timer_basic timer;

	gui_parameters gui;                       // Standard GUI element storage

	cgp::skybox_drawable skybox;

	light_shape_drawable light_drawable;

	// -------------- Meshes
	cgp::mesh_drawable water;
	cgp::mesh water_mesh;
	cgp::mesh water_mesh_init;

	cgp::mesh_drawable terrain;
	
	cgp::mesh_drawable metal;
	cgp::mesh_drawable lighthouse;
	cgp::mesh_drawable cone;

	cgp::mesh_drawable castle;
	cgp::mesh_drawable boat;

	cgp::mesh_drawable flag;
	cgp::mesh flag_mesh;
	cgp::mesh flag_mesh_init;

	cgp::mesh_drawable light_cones[20];

	/*cgp::mesh_drawable out_light_cone1;
	cgp::mesh_drawable out_light_cone2;
	cgp::mesh_drawable mid_light_cone1;
	cgp::mesh_drawable mid_light_cone2;
	cgp::mesh_drawable in_light_cone1;
	cgp::mesh_drawable in_light_cone2;*/

	cgp::hierarchy_mesh_drawable hierarchy;

	perlin_noise_parameters parameters;

	float last_wait=0;

	void update_camera();

	void initialize();  // Standard initialization to be called before the animation loop
	void display();     // The frame display to be called within the animation loop
	void display_semi_transparent();

	void display_gui(); // The display of the GUI, also called within the animation loop
	void reset_mouse(GLFWwindow* window);

};