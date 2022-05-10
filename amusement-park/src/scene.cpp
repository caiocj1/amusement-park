#include "scene.hpp"
#include "terrain.hpp"
#include "stb_image.h"

using namespace cgp;


void scene_structure::initialize()
{
    skybox.initialize("tex/skybox/");

	// Basic set-up
	// ***************************************** //

	global_frame.initialize(mesh_primitive_frame(), "Frame");
	environment.camera.axis = camera_spherical_coordinates_axis::z;
	environment.camera.look_at({ 15.0f,6.0f,6.0f }, { 0,0,0 });
    environment.light = { 3, 3, 10 };

	// Creating terrain mesh
	int N = 200;
	float terrain_length = 1000;
	mesh const terrain_mesh = create_terrain_mesh(N, terrain_length);
	terrain.initialize(terrain_mesh, "terrain");

    test_cylinder.initialize(mesh_primitive_cylinder(1.0f), "cylinder");

	// Altering terrain
	terrain.shading.color = { 0.6f,0.85f,0.5f };
	terrain.shading.phong.specular = 0.0f; // non-specular terrain material

}

void scene_structure::display()
{
    draw(skybox, environment);

	if (gui.display_frame)
		draw(global_frame, environment);
    
	draw(terrain, environment);
    draw(test_cylinder, environment);

	if (gui.display_wireframe)
		draw_wireframe(terrain, environment);
}

void scene_structure::display_gui()
{
	ImGui::Checkbox("Frame", &gui.display_frame);
	ImGui::Checkbox("Wireframe", &gui.display_wireframe);
}