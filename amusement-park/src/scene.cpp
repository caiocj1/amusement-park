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
		// Terrain *********************
		mesh terrain_mesh = mesh_load_file_obj("obj/final_.obj");
		//terrain_mesh.uv *= 1;
		GLuint const texture_image_id_terrain = opengl_load_texture_image("tex/tex_.jpg", GL_REPEAT, GL_REPEAT);
		terrain.initialize(terrain_mesh, "terrain", shader_lights, texture_image_id_terrain);
		terrain.transform.translation = { 0,0,-45 };
		terrain.shading.phong = { 1,1,0,1 };

		// Water **********************
		initialize_waves();

		water_mesh = create_terrain_mesh();
		water_mesh.uv *= 20;

		GLuint const texture_image_id_water = opengl_load_texture_image("tex/water.jpg", GL_REPEAT, GL_REPEAT);
		water.initialize(water_mesh, "water");
		water.transform.translation = { 0,0,-41.5 };
		water.shading.alpha = 0.8f;

		water_mesh_init = water_mesh;

		// Lighthouse ******************
		mesh lighthouse_mesh = mesh_load_file_obj("obj/light_house.obj");
		lighthouse_mesh.uv *= 1;
		GLuint const texture_image_id_lighthouse = opengl_load_texture_image("tex/lighthouse_5.jpg", GL_REPEAT, GL_REPEAT);
		lighthouse.initialize(lighthouse_mesh, "lighthouse", shader_lights, texture_image_id_lighthouse);
		lighthouse.transform.scaling = 0.9f;
		lighthouse.transform.translation = { 14.0f, 33.0f, -21.5f };

		mesh lighthouse_metal_mesh = mesh_load_file_obj("obj/light_house_metal.obj");
		lighthouse_metal_mesh.uv *= 1;
		GLuint const texture_image_id_metal = opengl_load_texture_image("tex/metal.jpg", GL_REPEAT, GL_REPEAT);
		metal.initialize(lighthouse_metal_mesh, "metal", shader_lights, texture_image_id_metal);
		metal.transform.scaling = 1.8f;
		metal.transform.translation = { 20.7f,33.0f,-10.3f };

		mesh cone_mesh = mesh_load_file_obj("obj/cone.obj");
		cone_mesh.uv *= 1;
		GLuint const texture_image_id_cone = opengl_load_texture_image("tex/cone.jpg", GL_REPEAT, GL_REPEAT);
		cone.initialize(cone_mesh, "cone", shader_lights, texture_image_id_cone);
		cone.transform.scaling = 1.2f;
		cone.transform.translation = { 20.7f,33.0f,-5.0f };
		//cone.shading.phong = { 1,0,0,1 };

		hierarchy.add(lighthouse);
		hierarchy.add(metal, "lighthouse");
		hierarchy.add(cone, "metal");

		// Castle ******************
		mesh castle_mesh = mesh_load_file_obj("obj/castle_ruin.obj");
		castle_mesh.uv *= 1;
		GLuint const texture_image_id_castle = opengl_load_texture_image("tex/ruined_castle.png", GL_REPEAT, GL_REPEAT);
		castle.initialize(castle_mesh, "castle", shader_lights, texture_image_id_castle);
		castle.transform.scaling = 1.0f;
		castle.transform.translation = {75.0f, -155.0f, -14.0f };
		castle.transform.rotation *= rotation_transform::between_vector({ 0, -1, 0 }, {0, 1, 0});
		castle.shading.phong = {0.3f,0.05f,0,1 };
		castle.shading.color = { 0.6f,0.6f,0.6f };

		// Boat ******************
		mesh boat_mesh = mesh_load_file_obj("obj/boat.obj");
		GLuint const texture_image_id_boat = opengl_load_texture_image("tex/rust.jpg", GL_REPEAT, GL_REPEAT);
		boat.initialize(boat_mesh, "boat", shader_lights, texture_image_id_boat);
		boat.transform.scaling = 2.0f;
		boat.transform.rotation *= rotation_transform::between_vector({ 0, 0, 1 }, { std::sqrt(0.05f), -std::sqrt(0.9f), -std::sqrt(0.05f)});
		boat.transform.translation = {-83.5f, -8.0f, -42.0f };
		boat.shading.alpha = 1.0f;

		// Flag ******************
		flag_mesh = create_flag_mesh();
		GLuint const texture_image_id_flag = opengl_load_texture_image("tex/pirate.png", GL_REPEAT, GL_REPEAT);
		flag.initialize(flag_mesh, "flag", shader_lights, texture_image_id_flag);
		flag.transform.scaling = 2.0f;
		flag_mesh_init = flag_mesh;
		flag.transform.translation = {25.0f, 57.0f, -20.0f };
		flag.shading.phong = {0.2f,0.05f,0,0.2f};

		pole_mesh = mesh_primitive_cylinder(0.05f, { 0,0,0 }, { 0,0,15}, 10, 20, true);
		pole.initialize(pole_mesh, "pole", shader_lights);
		pole.transform.translation = {25.0f, 57.0f, -25.0f };

	}


	// The lights displayed as spheres using this helper initializer (*)-optionnal
	light_drawable.initialize(shader_lights);

	// Light from lighthouse
	{
		mesh_drawable light_cone_head;
		light_cone_head.initialize(mesh_primitive_sphere(0.01, { 0, 0, 0 }, 40, 20), "light_cone_head");
		hierarchy.add(light_cone_head);

		// Use another shader?

		mesh* light_cone_meshes = new mesh[20];

		for(int i = 0; i < 20; i++)
		{
			light_cone_meshes[i] = mesh_primitive_cone(15.0f * sin((i + 1) * Pi / (40.0f)), 103.0f + 15.0f * cos((i + 1) * Pi / (40.0f)));
			light_cones[i].initialize(light_cone_meshes[i], "cone" + std::to_string(i));
			light_cones[i].transform.rotation *= rotation_transform::between_vector({ 0, 0, 1 }, { 0, 1, 0 });
			light_cones[i].shading.color = { 1.0, 0.9, 0.55 };
			light_cones[i].shading.alpha = 0.2f * (20 - i) / 20;

			hierarchy.add(light_cones[i], "light_cone_head", { 0, -(103.0f + 15.0f * cos((i + 1) * Pi / (40.0f))), 0 });
		}

		delete[] light_cone_meshes;

		hierarchy["light_cone_head"].transform.translation = { 20.7f, 33.0f, -6.8f };
	}

	// THUNDER - BOOOOOOM
	lightning.initialize(shader_lights);

	// Light from LIGHTNING
	{
		mesh lightn = mesh_load_file_obj("obj/eclair1.obj");
		lightn.uv *= 1;
		GLuint const lightning_light = opengl_load_shader("shaders/light_cone/vert.glsl", "shaders/light_cone/frag.glsl");
		GLuint const texture_image_id_lightning = opengl_load_texture_image("tex/lightning_.png", GL_REPEAT, GL_REPEAT);

		lightning_draw.initialize(lightn, "lightning", lightning_light);
		lightning_draw.transform.scaling = 50.0f;
		lightning_draw.shading.phong = { 1,1,1,1 };
		lightning_draw.shading.color = { 1,1,1 };
	}

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

	draw(metal, environment);
	draw(lighthouse, environment);
	draw(cone, environment);
	draw(castle, environment);
	//draw(flag, environment);
	draw(pole, environment);

	if (cooldown == 0) {
		cooldown = 50+20*((double)rand() / (RAND_MAX));
		lightning_life = 10;
		lightning_draw.transform.translation = { 100*(((double)rand() / (RAND_MAX))-0.5f), 100*(((double)rand() / (RAND_MAX)) - 0.5f), -45.0f};
	}

	if(lightning_life>0){
		draw(lightning_draw, environment);
		lightning_life--;
	}

	cooldown--;


	// Update the position and color of the lights
	compute_light_position(t, environment);
	draw(light_drawable, environment); // this is a helper function from multiple_lights (display all the spotlights as spheres) (*)-optionnal

	hierarchy["light_cone_head"].transform.rotation *= rotation_transform::from_axis_angle({ 0, 0, 1 }, 300 * dt);
	hierarchy.update_local_to_global_coordinates();

	display_semi_transparent(t);

	update_flag(flag_mesh, flag_mesh_init, flag, t);
	gerstner_waves(water_mesh, water_mesh_init, water, t);
}

void scene_structure::display_semi_transparent(float t)
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

	draw(water, environment);
	draw(hierarchy, environment);

	boat.shading.alpha = 0.6f + 0.4f*std::sin(3.0f*t);
	draw(boat, environment);

	draw(flag, environment);

	// Don't forget to re-activate the depth-buffer write
	glDepthMask(true);
	glDisable(GL_BLEND);
}
	

void scene_structure::display_gui()
{
	ImGui::Checkbox("Frame", &gui.display_frame);
	ImGui::Checkbox("Wireframe", &gui.display_wireframe);
}