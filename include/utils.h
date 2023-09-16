#ifndef DEMO_UTILS_H
#define DEMO_UTILS_H

#include <aperture/ap_utils.h>
#include "config.h"

#ifndef DEMO_FONT_PATH
#define DEMO_FONT_PATH "/fonts/roboto-remix.ttf"
#endif

#if AP_PLATFORM_ANDROID
#define AP_DEMO_CAMERA_NUMBER 1
#else // AP_PLATFORM_ANDROID
#define AP_DEMO_CAMERA_NUMBER 3
#endif // AP_PLATFORM_ANDROID

extern unsigned int cube_shader, ortho_shader;
extern unsigned int light_texture, light_cube_VAO;
extern unsigned int model_id, camera_use_id, camera_ids[];
extern float light_positions[][3];
extern bool spot_light_enabled, draw_light_cubes;
extern int material_number;

int demo_init();
int demo_render();
int demo_finished();

#endif
