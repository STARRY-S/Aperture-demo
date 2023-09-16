#include <aperture/ap_utils.h>
#include <aperture/ap_render.h>
#include <aperture/ap_model.h>
#include <aperture/ap_camera.h>
#include <aperture/ap_cvector.h>
#include <aperture/ap_shader.h>
#include <aperture/ap_audio.h>
#include <aperture/ap_physic.h>
#include <aperture/ap_math.h>
#include <aperture/ap_sqlite.h>
#include <cglm/cglm.h>

#include "utils.h"
#include "light.h"
#include "database.h"
#include "config.h"

#ifndef MODEL_FILE_NAME
#define MODEL_FILE_NAME "/mc/mc-game.obj"
#endif

unsigned int model_id = 0;
unsigned int camera_ids[AP_DEMO_CAMERA_NUMBER] = { 0 };
unsigned int camera_use_id = 0;

bool spot_light_enabled = false;
bool draw_light_cubes = false;
int material_number = 0;

vec3 light_positions[DEMO_POINT_LIGHT_NUM] = {
        {0.0f, 4.0f, 0.0f},
        {-11.0f, 3.0f, -23.0f},
        {-1.0f, 6.0f, -17.0f},
        {-16.0f, 6.0f, -17.0f},
        {29.0f, 3.0f, 46.0f},
        {36.0f, 3.0f, 46.0f},
        {23.0f, 4.0f, 18.0f},
        {42.0f, 3.0f, 8.0f},
        {28.0f, 3.0f, -6.0f},
        {23.0f, 3.0f, -24.0f},
        {10.0f, 2.0f, -35.0f},
        {10.0f, 2.0f, -30.0f}
};

vec3 ortho_cube_pos = { 0, 0, 0 };

unsigned int cube_shader = 0;
unsigned int light_texture = 0;
unsigned int light_cube_VAO = 0;

static char buffer[AP_DEFAULT_BUFFER_SIZE] = { 0 };

struct AP_PCreature *player = NULL;
unsigned int creature_id = 0;
struct AP_Vector barrier_id_vector = { 0, 0, 0, 0 };


int demo_init()
{
        ap_render_general_initialize();
        ap_audio_init();
        demo_setup_light();
        demo_setup_database();
        ap_render_init_font(DEMO_DATA_DIR DEMO_FONT_PATH, 42);
        // init model
        AP_CHECK(ap_model_generate(DEMO_DATA_DIR MODEL_FILE_NAME, &model_id));
        ap_model_use(model_id);

        // Player (creature)
        vec3 tmp;
        ap_v3_set(tmp, 0.8f, 1.8f, 0.8f);
        AP_CHECK( ap_physic_generate_creature(&creature_id, tmp) );
        ap_creature_use(creature_id);
        ap_v3_set(tmp, 0.f, 1.f, 0.f);
        ap_creature_set_camera_offset(tmp);

#if !AP_PLATFORM_ANDROID
        ap_creature_set_pos(db_restore_creature_pos);
        ap_camera_set_yaw(db_restore_creature_euler[0]);
        ap_camera_set_pitch(db_restore_creature_euler[1]);
#else
        ap_v3_set(tmp, 0.f, 0.f, 0.f);
        ap_creature_set_pos(tmp);
#endif

        ap_physic_get_creature_ptr(creature_id, &player);
        if (player == NULL || creature_id == 0) {
                LOGE("FATAL: failed to generate player");
                exit(-1);
        }

        // Setup barriers
        ap_vector_init(&barrier_id_vector, AP_VECTOR_UINT);
        unsigned int barrier_id = 0;
        // generate a ground barrier
        ap_physic_generate_barrier(&barrier_id, AP_BARRIER_TYPE_BOX);
        // ground size (100, 1, 100)
        ap_v3_set(tmp, 100.f, 1.f, 100.f);
        ap_barrier_set_size(barrier_id, tmp);
        // ground position (0, 0, 0)
        ap_v3_set(tmp, 0.f, -0.5f, 0.f);
        ap_barrier_set_pos(barrier_id, tmp);
        ap_vector_push_back(&barrier_id_vector, (char*) &barrier_id);

        // generate four pillar barriers
        vec3 pillar_poss[] = {
                {   0.f, 2.f,  -5.f },
                {   0.f, 2.f,   5.f },
                {   5.f, 2.f,   0.f },
                {  -5.f, 2.f,   0.f },
                { -28.f, 2.f,  -6.f },
                { -33.f, 2.f, -10.f },
                { -29.f, 2.f, -15.f },
                { -28.f, 2.f, -20.f },
                { -28.f, 2.f, -25.f },
                { -28.f, 2.f, -28.f },
                { -28.f, 2.f, -32.f },
                { -28.f, 2.f, -38.f },
                { -27.f, 2.f, -41.f },
                {  13.f, 2.f, -40.f },
                {  23.f, 2.f, -41.f },
                {  39.f, 2.f, -32.f },
                {  20.f, 2.f, -28.f },
                {  33.f, 2.f, -16.f },
                {  40.f, 2.f,   3.f },
                {  15.f, 2.f,  12.f },
                {  29.f, 2.f,  46.f },
                {  36.f, 2.f,  46.f },
                { -30.f, 2.f, -10.f },
                {  33.f, 2.f,  19.f }
        };
        for (int i = 0; i < (sizeof(pillar_poss) / VEC3_SIZE); ++i) {
                ap_physic_generate_barrier(&barrier_id, AP_BARRIER_TYPE_BOX);
                ap_v3_set(tmp, 1.f, 4.f, 1.f);
                ap_barrier_set_size(barrier_id, tmp);
                ap_barrier_set_pos(barrier_id, pillar_poss[i]);
                ap_vector_push_back(&barrier_id_vector, (char*) &barrier_id);
        }

        ap_physic_generate_barrier(&barrier_id, AP_BARRIER_TYPE_BOX);
        ap_v3_set(tmp, 3.0f, 1.0f, 3.0f);
        ap_barrier_set_size(barrier_id, tmp);
        ap_v3_set(tmp, 11.f, 0.5f, -2.f);
        ap_barrier_set_pos(barrier_id, tmp);
        ap_vector_push_back(&barrier_id_vector, (char*) &barrier_id);

        // house left wall
        ap_physic_generate_barrier(&barrier_id, AP_BARRIER_TYPE_BOX);
        ap_v3_set(tmp, 13.0f, 4.0f, 1.0f);
        ap_barrier_set_size(barrier_id, tmp);
        ap_v3_set(tmp, -8.f, 2.f, -32.f);
        ap_barrier_set_pos(barrier_id, tmp);
        ap_vector_push_back(&barrier_id_vector, (char*) &barrier_id);

        // house back wall
        ap_physic_generate_barrier(&barrier_id, AP_BARRIER_TYPE_BOX);
        ap_v3_set(tmp, 1.0f, 4.0f, 16.0f);
        ap_barrier_set_size(barrier_id, tmp);
        ap_v3_set(tmp, -16.f, 2.f, -24.f);
        ap_barrier_set_pos(barrier_id, tmp);
        ap_vector_push_back(&barrier_id_vector, (char*) &barrier_id);

        // house right wall
        ap_physic_generate_barrier(&barrier_id, AP_BARRIER_TYPE_BOX);
        ap_v3_set(tmp, 1.0f, 4.0f, 16.0f);
        ap_barrier_set_size(barrier_id, tmp);
        ap_v3_set(tmp, -1.f, 2.f, -24.5f);
        ap_barrier_set_pos(barrier_id, tmp);
        ap_vector_push_back(&barrier_id_vector, (char*) &barrier_id);

        // house front wall
        ap_physic_generate_barrier(&barrier_id, AP_BARRIER_TYPE_BOX);
        ap_v3_set(tmp, 11.0f, 4.0f, 1.0f);
        ap_barrier_set_size(barrier_id, tmp);
        ap_v3_set(tmp, -7.f, 2.f, -17.f);
        ap_barrier_set_pos(barrier_id, tmp);
        ap_vector_push_back(&barrier_id_vector, (char*) &barrier_id);

        vec3 blocks[] = {
                { -7.f, 0.5f, -30.f },
                { -7.f, 0.5f, -31.f },
                { -6.f, 1.5f, -30.f },
                { -6.f, 1.5f, -31.f },
                { -5.f, 2.5f, -30.f },
                { -5.f, 2.5f, -31.f },
                { -4.f, 3.5f, -30.f },
                { -4.f, 3.5f, -31.f },
                { -3.f, 4.5f, -30.f },
                { -3.f, 4.5f, -31.f },
                { -2.f, 4.5f, -30.f },
                { -2.f, 4.5f, -31.f }
        };
        for (int i = 0; i < (sizeof(blocks) / VEC3_SIZE); ++i) {
                ap_physic_generate_barrier(&barrier_id, AP_BARRIER_TYPE_BOX);
                ap_v3_set(tmp, 1.f, 1.f, 1.f);
                ap_barrier_set_size(barrier_id, tmp);
                ap_barrier_set_pos(barrier_id, blocks[i]);
                ap_vector_push_back(&barrier_id_vector, (char*) &barrier_id);
        }

        vec4 aim_color = { 1.0f, 1.0f, 1.0f, 0.5f };
        ap_render_set_aim_cross(40, 2, aim_color);
        ap_v4_set(aim_color, 1.0f, 1.0f, 1.0f, 1.0f);
        ap_render_set_aim_dot(4, aim_color);

        int view_distance = 16 * 10;
#if AP_PLATFORM_ANDROID
        // bool enable_mobile_type = false;
        // int iMobileType = ap_get_mobile_type(ap_get_mobile_name());
        // switch (iMobileType) {
        //         case AP_MOBILE_GOOGLE:
        //         case AP_MOBILE_X86:
        //         enable_mobile_type = false;
        //         break;
        //         default:
        //         enable_mobile_type = true;
        // }
        view_distance = 16 * 6;
        // Do not calculate point lights in Android platform
        // FIXME: point calculate consumes a huge amount of performance
        ap_render_set_point_light_enabled(false);
        ap_render_set_env_light_enabled(true);
#else
        ap_render_set_point_light_enabled(true);
        ap_render_set_env_light_enabled(true);
#endif
        ap_render_set_optimize_zconflict(true);
        ap_render_set_view_distance(view_distance);

        unsigned int audio_id = 0;
        ap_audio_load_MP3(
                DEMO_DATA_DIR "/sound/c418-haggstorm.mp3", &audio_id);
        if (audio_id > 0) {
                ap_audio_play(audio_id, NULL);
        }

        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LESS);

        return 0;
}

int demo_render()
{
        ap_render_flush();
        // render
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // depth test
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        // cull face
        glEnable(GL_CULL_FACE);

        // update player
        ap_physic_update_creature();
        // render the model
        ap_model_use(model_id);
        AP_CHECK( ap_model_draw() );

        // render the lamp cube
        ap_shader_use(cube_shader);
        int view_distance = 0;
        ap_render_get_view_distance(&view_distance);
        ap_shader_set_float(
                cube_shader,
                "view_distance",
                (float) view_distance
        );
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, light_texture);
        float *projection = NULL;
        ap_render_get_persp_matrix(&projection);
        ap_shader_set_mat4(cube_shader, "projection", projection);
        float *view = NULL;
        ap_render_get_view_matrix(&view);
        ap_shader_set_mat4(cube_shader, "view", view);

        mat4 mat_model;
        for (int i = 0; draw_light_cubes && i < DEMO_POINT_LIGHT_NUM; ++i) {
                glm_mat4_identity(mat_model);
                glm_translate(mat_model, light_positions[i]);
                ap_shader_set_mat4(cube_shader, "model", mat_model[0]);
                glBindVertexArray(light_cube_VAO);
                glDrawArrays(GL_TRIANGLES, 0, 36);
        }

        vec4 color = {0.9, 0.9, 0.9, 1.0};
        int screen_height = ap_get_buffer_height();
        float fps = 0;
        ap_render_get_fps(&fps);
        vec3 cam_position = { 0.0f, 0.0f, 0.0f };
        vec3 cam_direction = { 0.0f, 0.0f, 0.0f };
        ap_camera_get_position(cam_position);
        ap_camera_get_front(cam_direction);
        sprintf(buffer, "(%.1f, %.1f, %.1f) (%.1f, %.1f, %.1f) %4.1ffps",
                player->box.pos[0],
                player->box.pos[1] - (player->box.size[1] / 2),
                player->box.pos[2],
                cam_direction[0], cam_direction[1], cam_direction[2], fps);
        ap_render_text_line(buffer, 10.0, screen_height - 30.0, 1.0, color);
        sprintf(buffer, "float: %d (%.1f, %.1f, %.1f)",
                player->floating, player->move.speed[0], player->move.speed[1],
                player->move.speed[2]);
        ap_render_text_line(buffer, 10.0, screen_height - 60.0, 1.0, color);
        ap_render_aim_dot();
        ap_render_aim_cross();

        glBindVertexArray(0);
        ap_shader_use(0);
        glDisable(GL_CULL_FACE);
        glDisable(GL_BLEND);

        return EXIT_SUCCESS;
}

int demo_finished()
{
        vec3 pos = {0}, front = {0};
        ap_v3_set(pos, player->box.pos[0], player->box.pos[1],
                player->box.pos[2]);
        ap_camera_use(player->camera_id);
        ap_camera_get_front(front);
        vec2 euler = {0};
        ap_camera_get_yaw(&euler[0]);
        ap_camera_get_pitch(&euler[1]);
        demo_save_database(pos, euler);
        glDeleteVertexArrays(1, &light_cube_VAO);

        return 0;
}
