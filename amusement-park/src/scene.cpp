#include "scene.hpp"
#include "terrain.hpp"
#include "stb_image.h"


using namespace cgp;

void scene_structure::reset_mouse(GLFWwindow* window) {
	glfwSetCursorPos(window, inputs.window.width / 2, inputs.window.height / 2);
}


void scene_structure::update_camera()
{
	inputs_keyboard_parameters const& keyboard = inputs.keyboard;
	camera_head& camera = environment.camera;

	// The camera moves forward all the time
	//   We consider in this example a constant velocity, so the displacement is: velocity * dt * front-camera-vector
	float const dt = timer.update();
	vec3 forward_displacement = gui.speed * 10 * dt * camera.front();
	vec3 upward_displacement = gui.speed * 10 * dt * vec3{0,0,1};

	vec3 side_displacement = cross(forward_displacement, camera.up());
	if (keyboard.w)
		camera.position_camera += forward_displacement;
	if(keyboard.s)
		camera.position_camera -= forward_displacement;
	if (keyboard.a)
		camera.position_camera -= side_displacement;
	if (keyboard.d)
		camera.position_camera += side_displacement;
	if (keyboard.ctrl)
		camera.position_camera += upward_displacement;
	if (keyboard.shift)
		camera.position_camera -= upward_displacement;


	// The camera rotates if we press on the arrow keys
	//  The rotation is only applied to the roll and pitch degrees of freedom.
	//float const pitch = 0.5f; // speed of the pitch
	//float const roll = 0.7f; // speed of the roll
	//if (keyboard.up)
		//camera.manipulator_rotate_roll_pitch_yaw(0, pitch * dt, 0);
	//if (keyboard.down)
		//camera.manipulator_rotate_roll_pitch_yaw(0, -pitch * dt, 0);
	//if (keyboard.right)
		//camera.manipulator_rotate_roll_pitch_yaw(0, 0, -roll * dt);
	//if (keyboard.left)
		//camera.manipulator_rotate_roll_pitch_yaw(0, 0, roll * dt);

	camera.manipulator_rotate_trackball({0,0}, inputs.mouse.position.previous-inputs.mouse.position.current);
}


void scene_structure::initialize()
{
    

	// Basic set-up
	// ***************************************** //

	global_frame.initialize(mesh_primitive_frame(), "Frame");

	environment.camera.position_camera = { 15, 6, 6 };
	environment.camera.manipulator_rotate_roll_pitch_yaw(0, Pi / 2.0f, 0);


	environment.light = { 0, 0, 50};
	environment.light_view.look_at(environment.light, { 0, 0, 0 });

	mesh terrain_mesh = mesh_load_file_obj("obj/terrain_tilt.obj");
	terrain_mesh.uv *= 7;
	GLuint const texture_image_id_terrain = opengl_load_texture_image("tex/rock_2.jpg", GL_REPEAT, GL_REPEAT);
	terrain.initialize(terrain_mesh, "terrain", NULL, texture_image_id_terrain);

	water_mesh = create_terrain_mesh();
	water_mesh.uv *= 2;

	GLuint const texture_image_id_water = opengl_load_texture_image("tex/water_4.jpg", GL_REPEAT, GL_REPEAT);
	water.initialize(water_mesh, "water", NULL, texture_image_id_water);
	water.transform.scaling = 100.0f;
	water.transform.translation = {0,0,-45};
	update_terrain(water_mesh, water, parameters);

	water_mesh_init = water_mesh;

	mesh lighthouse_metal_mesh = mesh_load_file_obj("obj/light_house_metal.obj");
	lighthouse_metal_mesh.uv *= 1;
	GLuint const texture_image_id_metal = opengl_load_texture_image("tex/metal.jpg", GL_REPEAT, GL_REPEAT);
	metal.initialize(lighthouse_metal_mesh, "metal", NULL, texture_image_id_metal);
	metal.transform.scaling = 3.0f;
	metal.transform.translation = {41.1f,50.0f,-22.5f};

	mesh lighthouse_mesh = mesh_load_file_obj("obj/light_house.obj");
	lighthouse_mesh.uv *= 1;
	GLuint const texture_image_id_lighthouse = opengl_load_texture_image("tex/lighthouse_5.jpg", GL_REPEAT, GL_REPEAT);
	lighthouse.initialize(lighthouse_mesh, "lighthouse", NULL, texture_image_id_lighthouse);
	lighthouse.transform.scaling = 1.5f;
	lighthouse.transform.translation = { 30.0f, 50.0f, -40.0f };

	mesh cone_mesh = mesh_load_file_obj("obj/cone.obj");
	cone_mesh.uv *= 1;
	GLuint const texture_image_id_cone = opengl_load_texture_image("tex/cone.jpg", GL_REPEAT, GL_REPEAT);
	cone.initialize(cone_mesh, "cone", NULL, texture_image_id_cone);
	cone.transform.scaling = 2.0f;
	cone.transform.translation = {41.0f,50.0,-13.5f};
	cone.shading.phong = { 1,0,0,1 };


	hierarchy.add(lighthouse);
	hierarchy.add(metal, "lighthouse");
	hierarchy.add(cone, "metal");

	mesh castle_mesh = mesh_load_file_obj("obj/castle_ruin.obj");
	castle_mesh.uv *= 1;
	GLuint const texture_image_id_castle = opengl_load_texture_image("tex/ruined_castle.png", GL_REPEAT, GL_REPEAT);
	castle.initialize(castle_mesh, "castle", NULL, texture_image_id_castle);
	castle.transform.scaling = 1.0f;
	castle.transform.translation = { 20.0f, 86.0f, -9.0f };
	castle.shading.phong = { 1,1,0,1 };
	castle.shading.color = { 0.6f,0.6f,0.6f };


	// Altering terrain
	//terrain.shading.color = { 0.6f,0.85f,0.5f };
	terrain.shading.phong.specular = 0.5f; // non-specular terrain material

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
	}

	// First pass: Draw all shapes that cast shadows
	//   Set the FBO to compute the shadow map
	shadow_map.start_first_pass_shadow_rendering();
	{
		if (gui.display_frame)
			shadow_map.draw_shadow_map(global_frame, environment);
	}
	shadow_map.end_first_pass_shadow_rendering(inputs.window.width, inputs.window.height); // after this line, the output takes place on the screen

	draw(skybox, environment);

	// Second pass: Draw all shapes that receives shadows
	{
		if (gui.display_frame)
			shadow_map.draw_with_shadow(global_frame, environment);
		shadow_map.draw_with_shadow(terrain, environment);
		shadow_map.draw_with_shadow(water, environment);
		shadow_map.draw_with_shadow(metal, environment);
		shadow_map.draw_with_shadow(lighthouse, environment);
		shadow_map.draw_with_shadow(cone, environment);
		shadow_map.draw_with_shadow(castle, environment);
	}

	// The shape can still be displayed using the default shaders (without shadow effect)
	if (gui.display_wireframe) {
		draw_wireframe(terrain, environment);
	}
	draw(sphere_light, environment); // note that the sphere_light should not cast shadow itself

	update_terrain(water_mesh, water_mesh_init, water, t);
}
	

void scene_structure::display_gui()
{
	ImGui::Checkbox("Frame", &gui.display_frame);
	ImGui::Checkbox("Wireframe", &gui.display_wireframe);
}