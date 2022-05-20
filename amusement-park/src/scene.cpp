#include "scene.hpp"
#include "terrain.hpp"
#include "stb_image.h"


using namespace cgp;


void scene_structure::update_camera()
{
	inputs_keyboard_parameters const& keyboard = inputs.keyboard;
	camera_head& camera = environment.camera;

	// The camera moves forward all the time
	//   We consider in this example a constant velocity, so the displacement is: velocity * dt * front-camera-vector
	float const dt = timer.update();
	vec3 forward_displacement = gui.speed * 10 * dt * camera.front();
	vec3 side_displacement = cross(forward_displacement, camera.up());
	if (keyboard.w)
		camera.position_camera += forward_displacement;
	if(keyboard.s)
		camera.position_camera -= forward_displacement;
	if (keyboard.a)
		camera.position_camera -= side_displacement;
	if (keyboard.d)
		camera.position_camera += side_displacement;

	// The camera rotates if we press on the arrow keys
	//  The rotation is only applied to the roll and pitch degrees of freedom.
	float const pitch = 0.5f; // speed of the pitch
	float const roll = 0.7f; // speed of the roll
	if (keyboard.up)
		camera.manipulator_rotate_roll_pitch_yaw(0, pitch * dt, 0);
	if (keyboard.down)
		camera.manipulator_rotate_roll_pitch_yaw(0, -pitch * dt, 0);
	if (keyboard.right)
		camera.manipulator_rotate_roll_pitch_yaw(0, 0, -roll * dt);
	if (keyboard.left)
		camera.manipulator_rotate_roll_pitch_yaw(0, 0, roll * dt);
}


void scene_structure::initialize()
{
    

	// Basic set-up
	// ***************************************** //

	global_frame.initialize(mesh_primitive_frame(), "Frame");

	environment.camera.position_camera = { 15, 6, 6 };
	environment.camera.manipulator_rotate_roll_pitch_yaw(0, Pi / 2.0f, 0);


	environment.light = { -2.5, -2.5, 5 };
	environment.light_view.look_at(environment.light, { 0, 0, 0 });

	// Creating terrain mesh
	int N = 200;
	float terrain_length = 200;
	mesh const terrain_mesh = create_terrain_mesh(N, terrain_length);
	terrain.initialize(terrain_mesh, "terrain");

	// Altering terrain
	terrain.shading.color = { 0.6f,0.85f,0.5f };
	terrain.shading.phong.specular = 0.0f; // non-specular terrain material


	sphere.initialize(mesh_primitive_sphere(), "Sphere");

	cube.initialize(mesh_primitive_cube(), "Cube");
	cube.transform.translation = { 1,2,0 };


	sphere_light.initialize(mesh_primitive_sphere(0.1f));
	sphere_light.shading.color = { 1,1,0 };
	sphere_light.shading.phong = { 1,0,0,1 };
	sphere_light.transform.translation = environment.light;

	// The shadow map paramters must be initialized before their use
	shadow_map.initialize();

	skybox.initialize("tex/skybox/");
}

void scene_structure::display()
{
	// Deal with animation
	timer.update();
	float t = timer.t;
	if (gui.animated_shapes) {
		sphere.transform.translation = { 2.0f * std::cos(t), 1.0f, -2.0f };
		cube.transform.rotation = rotation_transform::from_axis_angle({ 0,1,0 }, t);
	}	

	// First pass: Draw all shapes that cast shadows
	//   Set the FBO to compute the shadow map
	shadow_map.start_first_pass_shadow_rendering();
	{
		shadow_map.draw_shadow_map(sphere, environment);
		if (gui.display_frame)
			shadow_map.draw_shadow_map(global_frame, environment);
		shadow_map.draw_shadow_map(cube, environment);
	}
	shadow_map.end_first_pass_shadow_rendering(inputs.window.width, inputs.window.height); // after this line, the output takes place on the screen

	draw(skybox, environment);

	// Second pass: Draw all shapes that receives shadows
	{
		if (gui.display_frame)
			shadow_map.draw_with_shadow(global_frame, environment);
		shadow_map.draw_with_shadow(terrain, environment);
		shadow_map.draw_with_shadow(sphere, environment);
		shadow_map.draw_with_shadow(cube, environment);
	}

	// The shape can still be displayed using the default shaders (without shadow effect)
	if (gui.display_wireframe) {
		draw_wireframe(terrain, environment);
		draw_wireframe(sphere, environment);
		draw_wireframe(cube, environment);
	}
	draw(sphere_light, environment); // note that the sphere_light should not cast shadow itself
}

void scene_structure::display_gui()
{
	ImGui::Checkbox("Frame", &gui.display_frame);
	ImGui::Checkbox("Wireframe", &gui.display_wireframe);
}