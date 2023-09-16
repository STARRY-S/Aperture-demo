#include "light.h"
#include "utils.h"
#include "config.h"

#include <aperture/ap_utils.h>
#include <aperture/ap_shader.h>
#include <aperture/ap_texture.h>
#include <aperture/ap_light.h>
#include <cglm/cglm.h>

vec3 point_light_ambient = { 0.45f, 0.4f, 0.4f };
vec3 point_light_diffuse = { 0.5f, 0.5f, 0.5f };
vec3 point_light_specular = { 0.2f, 0.2f, 0.2f };

vec3 dir_light_direction = { -1.0f, -1.0f, -0.0f };
#if AP_PLATFORM_ANDROID
vec3 dir_light_ambient   = { 0.53f, 0.5f,  0.52f };
#else
vec3 dir_light_ambient   = { 0.23f, 0.2f,  0.22f };
#endif
vec3 dir_light_diffuse   = { 0.4f,  0.4f,  0.4f  };
vec3 dir_light_specular  = { 0.3f,  0.3f,  0.3f  };

vec3 spot_light_ambient = {0.0f, 0.0f, 0.0f};
vec3 spot_light_diffuse = {1.0f, 1.0f, 1.0f};
vec3 spot_light_specular = {0.5f, 0.5f, 0.5f};

static int demo_setup_point_lights()
{
        // constant linear quadratic
        float param[AP_LIGHT_PARAM_NUM] = { 1.0f, 0.09f, 0.032f };
        for (int i = 0; i < DEMO_POINT_LIGHT_NUM; ++i) {
                unsigned id = 0;
                ap_light_generate_point(
                        &id,
                        light_positions[i],
                        point_light_ambient,
                        point_light_diffuse,
                        point_light_specular,
                        param
                );
        }

        return 0;
}

static int demo_setup_directional_light()
{
        ap_light_setup_directional(
                dir_light_direction,
                dir_light_ambient,
                dir_light_diffuse,
                dir_light_specular
        );
        return 0;
}

static int demo_setup_spot_light()
{
        float param[AP_LIGHT_PARAM_NUM] = {
                1.0f, 0.09f, 0.032f,
                (float) cos(glm_rad(30.0f)),    // cut_off
                (float) cos(glm_rad(35.0f))     // outer_cut_off
        };

        ap_light_setup_spot(
                spot_light_ambient,
                spot_light_diffuse,
                spot_light_specular,
                param
        );

        return 0;
}

int demo_setup_light()
{
        ap_shader_generate(
                DEMO_DATA_DIR "/glsl/cube_light.vs.glsl",
                DEMO_DATA_DIR "/glsl/cube_light.fs.glsl",
                &cube_shader
        );

        ap_texture_generate(&light_texture,
                AP_TEXTURE_TYPE_DIFFUSE,
                "redstone_lamp_on.png",
                DEMO_DATA_DIR "/mc/tex/minecraft/block/",
                false
        );
        ap_shader_set_int(cube_shader, "texture_diffuse0", 0);

        // light cube initialize
        const float* cube_vertices = ap_get_default_cube_vertices();
        unsigned int VBO = 0;
        glGenBuffers(1, &VBO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(
                GL_ARRAY_BUFFER,
                ap_get_default_cube_vertices_length(),
                cube_vertices,
                GL_STATIC_DRAW
        );

        glGenVertexArrays(1, &light_cube_VAO);
        glBindVertexArray(light_cube_VAO);
        glVertexAttribPointer(
                0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0
        );
        glEnableVertexAttribArray(0);
        // texture coord attribute
        glVertexAttribPointer(
                2, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float),
                (void*)(3 * sizeof(float))
        );
        glEnableVertexAttribArray(2);

        demo_update_light();

        return 0;
}

int demo_update_light()
{
        demo_setup_directional_light();
        demo_setup_point_lights();
        demo_setup_spot_light();
        ap_light_set_material_shininess(16.0f);
        ap_light_send_data();

        return 0;
}