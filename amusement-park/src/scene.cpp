#include "scene.hpp"
#include "terrain.hpp"

using namespace cgp;

void scene_structure::initialize()
{
	// Basic set-up
	// ***************************************** //

	global_frame.initialize(mesh_primitive_frame(), "Frame");
	environment.camera.axis = camera_spherical_coordinates_axis::z;
	environment.camera.look_at({ 15.0f,6.0f,6.0f }, { 0,0,0 });

	// Creating terrain mesh
	int N = 200;
	float terrain_length = 100;
	mesh const terrain_mesh = create_terrain_mesh(N, terrain_length);
	terrain.initialize(terrain_mesh, "terrain");

	// Altering terrain
	terrain.shading.color = { 0.6f,0.85f,0.5f };
	terrain.shading.phong.specular = 0.0f; // non-specular terrain material
}

void scene_structure::display()
{
	environment.light = environment.camera.position();
	if (gui.display_frame)
		draw(global_frame, environment);

	draw(terrain, environment);

	if (gui.display_wireframe)
		draw_wireframe(terrain, environment);
}

void scene_structure::display_gui()
{
	ImGui::Checkbox("Frame", &gui.display_frame);
	ImGui::Checkbox("Wireframe", &gui.display_wireframe);
}