#include <aperture/glad/glad.h>
#include <GLFW/glfw3.h>

#include <aperture/ap_config.h>
#include <aperture/ap_utils.h>
#include <aperture/ap_render.h>
#include <aperture/ap_camera.h>
#include <aperture/ap_physic.h>
#include <stdio.h>
#include <stdlib.h>

#include "main.h"
#include "utils.h"
#include "config.h"

void key_callback(GLFWwindow *win, int key, int s, int action, int mods);
void framebuffer_size_callback(GLFWwindow *win, int width, int height);
void mouse_callback(GLFWwindow *win, double x_pos, double y_pos);
void mouse_cursor_callback(GLFWwindow *win, double x_pos, double y_pos);
void mouse_button_callback(GLFWwindow *win, int button, int action, int mods);
void scroll_callback(GLFWwindow *win, double x_offset, double y_offset);
void process_input(GLFWwindow *win);

GLFWwindow* window = NULL;
const GLFWvidmode* mode;

float last_x = 400, last_y = 300;
bool first_mouse = true;
bool full_screen_mode = false;
bool left_button_pressed = false;

int main(int argc, char **argv)
{
        if (argc == 2 && !strcmp(argv[1], "-h")) {
                printf("%s -f -- Full Screen Mode\n\n", argv[0]);
                printf("\tW S A D -- Camea movement\n");
                printf("\tLCtrl   -- Speedup movement\n");
                printf("\tC       -- Change camera\n");
                printf("\tT       -- Change texture\n");
                printf("\tL       -- Turn ON/OFF spot light\n");
                printf("\tESC / Q -- Exit\n");

                return 0;
        }

        if (argc == 2 && !strcmp(argv[1], "-f")) {
                full_screen_mode = true;
        }

        LOGI("Using %s version - %s",
                AP_PROJECT_NAME, AP_VERSION);
        LOGI("Demo version %s", DEMO_VERSION)

        glfwInit();
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

        #ifdef __APPLE__
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
        #endif

        mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
        glfwWindowHint(GLFW_RED_BITS, mode->redBits);
        glfwWindowHint(GLFW_GREEN_BITS, mode->greenBits);
        glfwWindowHint(GLFW_BLUE_BITS, mode->blueBits);
        glfwWindowHint(GLFW_REFRESH_RATE, mode->refreshRate);

        glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
        if (full_screen_mode) {
                window = glfwCreateWindow(mode->width, mode->height,
                        WINDOW_TITLE, glfwGetPrimaryMonitor(), NULL);
        } else {
                window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT,
                        WINDOW_TITLE, NULL, NULL);
        }

        if (window == NULL)
        {
                LOGE("Failed to create GLFW window.");
                glfwTerminate();
                return -1;
        }

        glfwSetWindowPos(window, (mode->width - SCREEN_WIDTH) / 2,
                        (mode->height - SCREEN_HEIGHT) / 2 );
        glfwMakeContextCurrent(window);
        ap_set_context_ptr(window);
        // init game engine
        ap_render_general_initialize();

        demo_init();
        if (full_screen_mode) {
                ap_render_resize_buffer(mode->width, mode->height);
        } else {
                ap_render_resize_buffer(SCREEN_WIDTH, SCREEN_HEIGHT);
        }

        glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
        glfwSetCursorPosCallback(window, mouse_callback);
        glfwSetScrollCallback(window, scroll_callback);
        glfwSetMouseButtonCallback(window, mouse_button_callback);
        glfwSetKeyCallback(window, key_callback);
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

        glfwShowWindow(window);

        while (!glfwWindowShouldClose(window))
        {
                // input
                process_input(window);

                // GameEngine main renderer
                demo_render();

                // refresh
                glfwSwapBuffers(window);
                glfwPollEvents();
        }

        demo_finished();
        ap_render_finish();
        glfwTerminate();
        return 0;
}

void key_callback(GLFWwindow *window, int key, int s, int action, int mods)
{
        if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
                glfwSetWindowShouldClose(window, GL_TRUE);
        }

        if (key == GLFW_KEY_Q && action == GLFW_PRESS) {
                glfwSetWindowShouldClose(window, GL_TRUE);
        }

        if (key == GLFW_KEY_L && action == GLFW_PRESS) {
                spot_light_enabled = !spot_light_enabled;
                ap_render_set_spot_light_enabled(spot_light_enabled);
        }

        if (key == GLFW_KEY_T && action == GLFW_PRESS) {
                if (++material_number > 1) {
                        material_number = 0;
                }
                ap_render_set_material_num(material_number);
        }

        if (key == GLFW_KEY_C && action == GLFW_PRESS) {
        }

        if (key == GLFW_KEY_0 && action == GLFW_PRESS) {
                draw_light_cubes = !draw_light_cubes;
        }
}

void process_input(GLFWwindow *window)
{
        float speed = 1.0f;
        if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) {
                speed *= 2.0f;
        }

        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
                ap_creature_process_move(AP_DIRECTION_FORWARD, speed);
        }
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
                ap_creature_process_move(AP_DIRECTION_BACKWORD, speed);
        }
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
                ap_creature_process_move(AP_DIRECTION_LEFT, speed);
        }
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
                ap_creature_process_move(AP_DIRECTION_RIGHT, speed);
        }
        if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
                ap_creature_process_move(AP_DIRECTION_DOWN, speed);
        }
        if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
                // ap_creature_process_move(AP_DIRECTION_UP, speed);
                ap_creature_jump();
        }
}

void mouse_callback(GLFWwindow *window, double x_pos, double y_pos)
{
        // Don't rotate viewport when mouse left button isn't pressed
        // if (!left_button_pressed) {
        //         if (!first_mouse) {
        //                 first_mouse = true;
        //         }
        //         return;
        // }

        if(first_mouse) {
                last_x = x_pos;
                last_y = y_pos;
                first_mouse = false;
        }

        float x_offset = x_pos - last_x;
        // it's oppisite (negative) here,
        // because y increases from bottom to top
        float y_offset = -(y_pos - last_y);
        last_x = x_pos;
        last_y = y_pos;

        ap_camera_process_mouse_move(x_offset, y_offset, true);
}

void mouse_button_callback(GLFWwindow *win, int button, int action, int mods)
{
        if (button == GLFW_MOUSE_BUTTON_LEFT) {
                if (action == GLFW_PRESS) {
                        left_button_pressed = true;
                } else if (action == GLFW_RELEASE) {
                        left_button_pressed = false;
                }
        }
}

void scroll_callback(GLFWwindow* window, double x_offset, double y_offset)
{
        ap_camera_process_scroll(y_offset);
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
        ap_render_resize_buffer(width, height);
}
