#ifndef DEMO_DATABASE_H
#define DEMO_DATABASE_H

#include <cglm/cglm.h>

extern float db_restore_creature_pos[3];
extern float db_restore_creature_euler[2];

int demo_setup_database();
int demo_save_database(vec3 pos, vec2 euler);

#endif  // DEMO_DATABASE_H
