#include <aperture/ap_utils.h>
#include <aperture/ap_sqlite.h>
#include <aperture/ap_math.h>
#include <aperture/ap_render.h>

#include "database.h"

#include <cglm/cglm.h>
#include <stdbool.h>

float db_restore_creature_pos[3] = {0};
float db_restore_creature_euler[2] = {0};

bool db_has_data = false;

static int demo_init_creature_cb(void *r, int argc, char **argv, char **name)
{
        if (argc == 0) {
                db_has_data = false;
                return 0;
        }

        vec3 pos = {0};
        vec2 euler = {0};
        for (int i = 0; i < argc; ++i) {
                // LOGD("%d: %s = %s", i, name[i], argv[i]);
                if (!argv[i]) {
                        continue;
                }
                if (strcmp(name[i], "POS_X") == 0) {
                        pos[0] = (float) ap_c2d(argv[i]);
                } else if (strcmp(name[i], "POS_Y") == 0) {
                        pos[1] = (float) ap_c2d(argv[i]);
                } else if (strcmp(name[i], "POS_Z") == 0) {
                        pos[2] = (float) ap_c2d(argv[i]);
                } else if (strcmp(name[i], "YAW") == 0) {
                        euler[0] = (float) ap_c2d(argv[i]);
                } else if (strcmp(name[i], "PITCH") == 0) {
                        euler[1] = (float) ap_c2d(argv[i]);
                }
        }

        LOGD("restore position (%.3f, %.3f, %.3f)", pos[0], pos[1], pos[2]);
        LOGD("restore Euler    YAW = %.3f, PITCH = %.3f",
                euler[0], euler[1]);
        memcpy(db_restore_creature_pos, pos, VEC3_SIZE);
        memcpy(db_restore_creature_euler, euler, VEC2_SIZE);
        db_has_data = true;

        return 0;
}

int demo_setup_database()
{
        // create table if not exists
        char *sql = "CREATE TABLE IF NOT EXISTS ap_creatures ( "
                " ID INTEGER PRIMARY KEY AUTOINCREMENT, "
                " POS_X TEXT, "
                " POS_Y TEXT, "
                " POS_Z TEXT, "
                " YAW TEXT, "
                " PITCH TEXT "
                " ); ";
        ap_sqlite_execute(sql, NULL);

        ap_sqlite_execute(
                "select POS_X,POS_Y,POS_Z,YAW,PITCH "
                " from ap_creatures where ID=1;",
                demo_init_creature_cb
        );

        if (!db_has_data) {
                 // if there is no data in database, insert a new data first
                char *sql = "INSERT INTO ap_creatures "
                        " ( POS_X,POS_Y,POS_Z,YAW,PITCH) VALUES "
                        " ('0.0','0.0','0.0','0.0','0.0');";
                ap_sqlite_execute(sql, NULL);
        }

        return 0;
}

int demo_save_database(vec3 pos, vec2 euler)
{
        char *buffer = AP_MALLOC(1024 * 10);
        sprintf(buffer, "update ap_creatures set POS_X='%.3f', POS_Y='%.3f', "
                "POS_Z='%.3f',YAW='%.3f',PITCH='%.3f' "
                " where ID=1",
                pos[0], pos[1], pos[2], euler[0], euler[1]
        );
        // LOGD("buffer: %s", buffer);
        ap_sqlite_execute(buffer, NULL);
        AP_FREE(buffer);
        return 0;
}