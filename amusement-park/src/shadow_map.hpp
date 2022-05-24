#pragma once

#include "cgp/cgp.hpp"


struct scene_environment_shadow_map {
	// Color of the background of the scene
	cgp::vec3 background_color;

	// The position/orientation of a camera that can rotates freely around a specific position
	cgp::camera_head camera;

	// A projection structure (perspective or orthogonal projection)
	cgp::camera_projection projection;

	// The position of a light
	cgp::vec3 light; // position of the light (provided for compatibility with default shaders)
	cgp::camera_spherical_coordinates light_view;
	cgp::camera_projection light_projection;

	// ---------------- Testing multiple lights + fog

	// Store position for 5 lights
	std::array<cgp::vec3, 5> spotlight_position;
	// The (r,g,b) color of the 5 lights
	std::array<cgp::vec3, 5> spotlight_color;

	// The characteristic attenuation of the light
	float spotlight_falloff = 0.5f;

	// The characteristic attenuation due to the fog
	float fog_falloff = 0.005f;

	// ------------------------------------------------

	scene_environment_shadow_map();
};
void opengl_uniform(GLuint shader, scene_environment_shadow_map const& current_scene);
void opengl_uniform_shadow(GLuint shader, scene_environment_shadow_map const& current_scene);


struct shadow_map_structure
{
	GLuint shader_shadow_map;    // Shader used to compute the shadow - depth map from the light viewpoint
	GLuint shader_shadow_render; // Shader used to render the 3D shapes with shadows once the shadow map is computed

	GLuint fbo;                // FBO used to render the shadow map as a texture
	GLuint texture_shadow_map; // Texture of the shadow map
	int width_shadow_map;
	int height_shadow_map;


	void initialize(); // Initialize shaders and FBO - must be called in the initialization step before any rendering
	void start_first_pass_shadow_rendering();  // Render objects in the shadow map after this line using draw_shadow_map()
	void end_first_pass_shadow_rendering(int window_width, int window_height);    // Render objects receiving shadows after this line using draw_with_shadow()

	void draw_shadow_map(cgp::mesh_drawable const& drawable, scene_environment_shadow_map const& scene);
	void draw_with_shadow(cgp::mesh_drawable const& drawable, scene_environment_shadow_map const& current_scene);
};

// ---------------- Testing multiple lights + fog

// Function to call in the display function of your scene
//  Compute a new position and color of the lights procedurally.
//  This function can be modified at your will.
void compute_light_position(float t, scene_environment_shadow_map& environment);

/** A structure to help drawing the light as sphere
* The spotlights are simply displayed as spheres */
struct light_shape_drawable
{
	cgp::mesh_drawable spotlight_sphere;              // a mesh_drawable of a sphere
	void initialize(GLuint shader_multiple_lights);   // Initialise the spotlight_sphere to a sphere
};
// Function to call in your scene display to draw all the spotlights
void draw(light_shape_drawable lights, scene_environment_shadow_map const& environment);

// Function to call in your display_gui to add the sliders modifying the spotlight and fog falloff
void display_gui_falloff(scene_environment_shadow_map& environment);