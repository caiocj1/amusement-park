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

	// The camera moves forward all the time
	//   We consider in this example a constant velocity, so the displacement is: velocity * dt * front-camera-vector
	float const dt = timer.update();
	vec3 forward_displacement = gui.speed * 30 * dt * environment.camera.front();
	vec3 upward_displacement = gui.speed * 30 * dt * vec3{0,0,1};

	vec3 side_displacement = cross(forward_displacement, environment.camera.up());
	if (keyboard.w)
		environment.camera.center_of_rotation += forward_displacement;
	if(keyboard.s)
		environment.camera.center_of_rotation -= forward_displacement;
	if (keyboard.a)
		environment.camera.center_of_rotation -= side_displacement;
	if (keyboard.d)
		environment.camera.center_of_rotation += side_displacement;
	if (keyboard.space)
		environment.camera.center_of_rotation += upward_displacement;
	if (keyboard.shift)
		environment.camera.center_of_rotation -= upward_displacement;

	float phi = -(inputs.mouse.position.previous - inputs.mouse.position.current)[0];
	float theta = (inputs.mouse.position.previous - inputs.mouse.position.current)[1];

	if (fabs(phi) > 0.01 || fabs(theta) > 0.01)
		environment.camera.manipulator_rotate_spherical_coordinates(phi, theta);
}


void scene_structure::initialize()
{
	GLuint const shader_lights = opengl_load_shader("shaders/mesh_lights/vert.glsl", "shaders/mesh_lights/frag.glsl");
	mesh_drawable::default_shader = shader_lights;   // set this shader as the default one for all new shapes declared after this line 


	// Basic set-up
	// ***************************************** //

	global_frame.initialize(mesh_primitive_frame(), "Frame");

	// Physical models
	{

		mesh terrain_mesh = mesh_load_file_obj("obj/final_.obj");
		//terrain_mesh.uv *= 1;
		GLuint const texture_image_id_terrain = opengl_load_texture_image("tex/tex_.jpg", GL_REPEAT, GL_REPEAT);
		terrain.initialize(terrain_mesh, "terrain", shader_lights, texture_image_id_terrain);
		terrain.transform.translation = { 0,0,-45 };
		terrain.shading.phong = { 1,1,0,1 };

		water_mesh = create_terrain_mesh();
		water_mesh.uv *= 2;

		GLuint const texture_image_id_water = opengl_load_texture_image("tex/water_4.jpg", GL_REPEAT, GL_REPEAT);
		water.initialize(water_mesh, "water", shader_lights, texture_image_id_water);
		water.transform.scaling = 200.0f;
		water.transform.translation = { 0,0,-50};
		update_terrain(water_mesh, water, parameters);

		water_mesh_init = water_mesh;

		mesh lighthouse_metal_mesh = mesh_load_file_obj("obj/light_house_metal.obj");
		lighthouse_metal_mesh.uv *= 1;
		GLuint const texture_image_id_metal = opengl_load_texture_image("tex/metal.jpg", GL_REPEAT, GL_REPEAT);
		metal.initialize(lighthouse_metal_mesh, "metal", shader_lights, texture_image_id_metal);
		metal.transform.scaling = 3.0f;
		metal.transform.translation = { 41.1f,50.0f,-22.5f };

		mesh lighthouse_mesh = mesh_load_file_obj("obj/light_house.obj");
		lighthouse_mesh.uv *= 1;
		GLuint const texture_image_id_lighthouse = opengl_load_texture_image("tex/lighthouse_5.jpg", GL_REPEAT, GL_REPEAT);
		lighthouse.initialize(lighthouse_mesh, "lighthouse", shader_lights, texture_image_id_lighthouse);
		lighthouse.transform.scaling = 1.5f;
		lighthouse.transform.translation = { 30.0f, 50.0f, -40.0f };

		mesh cone_mesh = mesh_load_file_obj("obj/cone.obj");
		cone_mesh.uv *= 1;
		GLuint const texture_image_id_cone = opengl_load_texture_image("tex/cone.jpg", GL_REPEAT, GL_REPEAT);
		cone.initialize(cone_mesh, "cone", shader_lights, texture_image_id_cone);
		cone.transform.scaling = 2.0f;
		cone.transform.translation = { 41.0f,50.0,-13.5f };
		cone.shading.phong = { 1,0,0,1 };


		hierarchy.add(lighthouse);
		hierarchy.add(metal, "lighthouse");
		hierarchy.add(cone, "metal");

		mesh castle_mesh = mesh_load_file_obj("obj/castle_ruin.obj");
		castle_mesh.uv *= 1;
		GLuint const texture_image_id_castle = opengl_load_texture_image("tex/ruined_castle.png", GL_REPEAT, GL_REPEAT);
		castle.initialize(castle_mesh, "castle", shader_lights, texture_image_id_castle);
		castle.transform.scaling = 1.0f;
		castle.transform.translation = { 20.0f, 86.0f, -9.0f };
		castle.shading.phong = { 1,1,0,1 };
		castle.shading.color = { 0.6f,0.6f,0.6f };

	}


	// The lights displayed as spheres using this helper initializer (*)-optionnal
	light_drawable.initialize(shader_lights);

	// Light from lighthouse
	{
		mesh_drawable light_cone_head;
		light_cone_head.initialize(mesh_primitive_sphere(0.01, { 0, 0, 0 }, 40, 20), "light_cone_head");

		mesh out_light_cone_mesh = mesh_primitive_cone(40, 600);

		out_light_cone1.initialize(out_light_cone_mesh, "out_light_cone1");
		out_light_cone1.transform.rotation *= rotation_transform::between_vector({ 0, 0, 1 }, { 0, 1, 0 });
		out_light_cone1.shading.color = {1.0, 0.9, 0.1};
		out_light_cone1.shading.alpha = 0.1;

		out_light_cone2.initialize(out_light_cone_mesh, "out_light_cone2");
		out_light_cone2.transform.rotation *= rotation_transform::between_vector({ 0, 0, 1 }, { 0, -1, 0 });
		out_light_cone2.shading.color = { 1.0, 0.9, 0.1 };
		out_light_cone2.shading.alpha = 0.1;

		mesh mid_light_cone_mesh = mesh_primitive_cone(35, 600);

		mid_light_cone1.initialize(mid_light_cone_mesh, "mid_light_cone1");
		mid_light_cone1.transform.rotation *= rotation_transform::between_vector({ 0, 0, 1 }, { 0, 1, 0 });
		mid_light_cone1.shading.color = { 1.0, 0.9, 0.1 };
		mid_light_cone1.shading.alpha = 0.2f;

		mid_light_cone2.initialize(mid_light_cone_mesh, "mid_light_cone2");
		mid_light_cone2.transform.rotation *= rotation_transform::between_vector({ 0, 0, 1 }, { 0, -1, 0 });
		mid_light_cone2.shading.color = { 1.0, 0.9, 0.1 };
		mid_light_cone2.shading.alpha = 0.2f;

		mesh in_light_cone_mesh = mesh_primitive_cone(30, 600);

		in_light_cone1.initialize(in_light_cone_mesh, "in_light_cone1");
		in_light_cone1.transform.rotation *= rotation_transform::between_vector({ 0, 0, 1 }, { 0, 1, 0 });
		in_light_cone1.shading.color = { 1.0, 0.9, 0.1 };
		in_light_cone1.shading.alpha = 0.25f;

		in_light_cone2.initialize(in_light_cone_mesh, "in_light_cone2");
		in_light_cone2.transform.rotation *= rotation_transform::between_vector({ 0, 0, 1 }, { 0, -1, 0 });
		in_light_cone2.shading.color = { 1.0, 0.9, 0.1 };
		in_light_cone2.shading.alpha = 0.25;

		hierarchy.add(light_cone_head);
		hierarchy.add(out_light_cone1, "light_cone_head", {0, -600, 0});
		hierarchy.add(out_light_cone2, "light_cone_head", { 0, 600, 0 });
		hierarchy.add(mid_light_cone1, "light_cone_head", { 0, -600, 0 });
		hierarchy.add(mid_light_cone2, "light_cone_head", { 0, 600, 0 });
		hierarchy.add(in_light_cone1, "light_cone_head", { 0, -600, 0 });
		hierarchy.add(in_light_cone2, "light_cone_head", { 0, 600, 0 });
		
		hierarchy["light_cone_head"].transform.translation = { 41.1f, 50.0f, -16.0f };
	}

	//environment.projection = camera_projection::perspective(50.0f * Pi / 180, 1.0f, 0.1f, 500.0f);
	environment.camera.axis = camera_spherical_coordinates_axis::z;
	environment.camera.look_at({ -1, 0, 0 }, { 0,0,0 });
	environment.camera.manipulator_rotate_spherical_coordinates(-Pi/2, 0);

	skybox.initialize("tex/skybox/");
}



void scene_structure::display()
{
	// Deal with animation
	float dt = timer.update();
	float t = timer.t;

	draw(skybox, environment);
	draw(global_frame, environment);

	draw(terrain, environment);
	draw(water, environment);
	draw(metal, environment);
	draw(lighthouse, environment);
	draw(cone, environment);
	draw(castle, environment);

	// Update the position and color of the lights
	compute_light_position(t, environment);
	draw(light_drawable, environment); // this is a helper function from multiple_lights (display all the spotlights as spheres) (*)-optionnal


	hierarchy["light_cone_head"].transform.rotation *= rotation_transform::from_axis_angle({ 0, 0, 1 }, 300 * dt);
	hierarchy.update_local_to_global_coordinates();

	display_semi_transparent();
	display_gui();

	update_terrain(water_mesh, water_mesh_init, water, t);
}

void scene_structure::display_semi_transparent()
{
	// Enable use of alpha component as color blending for transparent elements
	//  alpha = current_color.alpha
	//  new color = previous_color * alpha + current_color * (1-alpha)
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// Disable depth buffer writing
	//  - Transparent elements cannot use depth buffer
	//  - They are supposed to be display from furest to nearest elements
	glDepthMask(false);

	draw(hierarchy, environment);

	// Don't forget to re-activate the depth-buffer write
	glDepthMask(true);
	glDisable(GL_BLEND);
}
	

void scene_structure::display_gui()
{
	ImGui::Checkbox("Frame", &gui.display_frame);
	ImGui::Checkbox("Wireframe", &gui.display_wireframe);
}