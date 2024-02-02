#include <stdio.h>
#include <time.h>
#include <math.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

#define MATRIX_IMPLEMENTATION
#include "Matrix.h"

#define VEC2_IMPLEMENTATION
#include "vec2.h"

#define WINDOW_WIDTH 1200
#define WINDOW_HEIGHT 1000
#define FPS 30
#define FRAME_TARGET_TIME (1000 / FPS)
#define MAX_NUMBER_OF_PARTICALES 100
#define MIN(a, b) (((a) < (b)) ? (a) : (b))
#define MAX(a, b) (((a) > (b)) ? (a) : (b))
#define dprintINT(expr) printf(#expr " = %d\n", expr)
#define dprintF(expr) printf(#expr " = %g\n", expr)

#define PI 3.14159265359

typedef struct {
    float v_x;
    float v_y;
    float x;
    float y;
} partical;

int initialize_window(void);
void setup(void);
void process_input(void);
void update(void);
void render(void);
void destroy_window(void);
void fix_framerate(void);
float liniar_map(float s, float min_in, float max_in, float min_out, float max_out);
void generate_vector_field(void);
void render_draw_vector_field_to_texture(SDL_Renderer *renderer, SDL_Texture *texture);
void creat_and_render_to_texture_vector_field(void);
vec2 V_source_p(vec2 v_in);
vec2 V_vortex_p(vec2 v_in);
vec2 V_circle_p(vec2 v_in);
vec2 polar2cartesian(vec2 v_in);
vec2 cartesian2polar(vec2 v_in);
void DrawCircle(SDL_Renderer * renderer, int32_t centreX, int32_t centreY, int32_t radius);
void fill_circle(SDL_Renderer *renderer, int x, int y, int radius, SDL_Color color);
void update_particals(void);

vec2 (*my_field_function)(vec2);

SDL_Window *window = NULL;
SDL_Renderer *renderer = NULL;
SDL_Renderer *background_renderer = NULL;
TTF_Font *font = NULL;
SDL_Surface *text_surface = NULL;
SDL_Texture *text_texture = NULL;
SDL_Texture *vector_field_texture = NULL;
SDL_Rect fps_place;
SDL_Rect vector_field_texture_rect;
SDL_Color white_color;
SDL_Color field_color;
SDL_Color turquoise_color;
int game_is_running = 0;
float delta_time;
float fps = 0;
float scale = 30;
float zoom = 1;
float vector_length_factor = 4;
int rows, cols;
Mat vector_field_theta;
Mat vector_field_size;

float Q = 1000;
float Gamma = 4000;
float U_inf = 10;
float a = 40;

float offset_x = -WINDOW_WIDTH/2;
float original_offset_x;
float offset_y = -WINDOW_HEIGHT/2;
float original_offset_y;

float max_length;
float min_length;
float final_max_length;
float final_min_length;
float max_theta;
float min_theta;
float final_max_theta;
float final_min_theta;

int generated_vector_field = 0;
int left_button_pressed = 0;
int left_mouse_was_pressed = 0;
int right_button_pressed = 0;
vec2 last_mouse_position = {.x = 0, .y = 0};
int to_print = 0;
int hide_center = 0;
partical particals[MAX_NUMBER_OF_PARTICALES];
int num_of_particals = -1;
float partical_velocity_factor = 30;

int main()
{
    srand(time(0));
    game_is_running = !initialize_window();

    setup();

    while (game_is_running) {
        process_input();
        update();
        render();        
    }

    destroy_window();
    return 0;
}

int initialize_window(void)
{
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
        fprintf(stderr, "Error initializing SDL.\n");
        return -1;
    }

    window = SDL_CreateWindow(NULL,
                              SDL_WINDOWPOS_CENTERED,
                              SDL_WINDOWPOS_CENTERED,
                              WINDOW_WIDTH,
                              WINDOW_HEIGHT,
                              0
                              );
    if (!window) {
        fprintf(stderr, "Error creating SDL window.\n");
        return -1;
    }

    renderer = SDL_CreateRenderer(window, -1, 0);
    if (!renderer) {
        fprintf(stderr, "Erorr creating SDL renderer.\n");
        return -1;
    }

    if (TTF_Init() == -1) {
        fprintf(stderr, "Erorr initailizin SDL_ttf.\n");
        return -1;
    }

    font = TTF_OpenFont("./font/Gabriely Black.ttf",32);
    if (!font) {
        fprintf(stderr, "Error loading font.\n");
        return -1;
    }
    
    return 0;
}

void setup(void)
{
    my_field_function = V_source_p;

    original_offset_x = offset_x;
    original_offset_y = offset_y;

    max_length = 0;
    min_length = Q*MAX(WINDOW_HEIGHT,WINDOW_WIDTH);

    white_color.a = 255;
    white_color.b = 255;
    white_color.g = 255;
    white_color.r = 255;

    turquoise_color.a = 255;
    turquoise_color.b = 255;
    turquoise_color.g = 255;
    turquoise_color.r = 0;

    fps_place.x = 10;
    fps_place.y = 10;
    fps_place.w = 400;
    fps_place.h = 25;

    vector_field_texture_rect.x = 0;
    vector_field_texture_rect.y = 0;
    vector_field_texture_rect.w = WINDOW_WIDTH;
    vector_field_texture_rect.h = WINDOW_HEIGHT;

    vector_field_texture = SDL_CreateTexture(renderer,
                                           SDL_PIXELFORMAT_RGBA32,
                                           SDL_TEXTUREACCESS_TARGET,
                                           WINDOW_WIDTH,
                                           WINDOW_HEIGHT);

    creat_and_render_to_texture_vector_field();
}

void process_input(void)
{
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
            case SDL_QUIT:
                game_is_running = 0;
                break;
            case SDL_KEYDOWN:
                if (event.key.keysym.sym == SDLK_ESCAPE) {
                    game_is_running = 0;
                }
                if (event.key.keysym.sym == SDLK_r) {
                    offset_x = original_offset_x;
                    offset_y = original_offset_y;
                    max_length = 0;
                    min_length = 1000;
                    max_theta = 0;
                    min_theta = 1000;
                    scale = 30;
                    zoom = 1;
                    num_of_particals = -1;

                    creat_and_render_to_texture_vector_field();    
                }
                if (event.key.keysym.sym == SDLK_p) {
                    to_print = 1;
                }
                if (event.key.keysym.sym == SDLK_h) {
                    if (!hide_center) {
                        hide_center = 1;

                        creat_and_render_to_texture_vector_field();
                    }
                    else if (hide_center) {
                        hide_center = 0;

                        creat_and_render_to_texture_vector_field();
                    }
                }
                if (event.key.keysym.sym == SDLK_d) {
                    num_of_particals = -1;
                }
                if (event.key.keysym.sym == SDLK_UP) {
                    zoom += 0.1;
                    creat_and_render_to_texture_vector_field();
                }
                if (event.key.keysym.sym == SDLK_DOWN) {
                    zoom -= 0.1;
                    creat_and_render_to_texture_vector_field();
                }
                break;
            case SDL_MOUSEBUTTONDOWN:
                if (event.button.button == SDL_BUTTON_LEFT) {
                    left_button_pressed = 1;
                }
                if (event.button.button == SDL_BUTTON_RIGHT) {
                    if (num_of_particals < 99) {
                        num_of_particals++;
                    }
                    right_button_pressed = 1;
                    
                }
                break;
            case SDL_MOUSEBUTTONUP:
                if (event.button.button == SDL_BUTTON_LEFT) {
                    left_button_pressed = 0;
                    left_mouse_was_pressed = 0;
                }
                if (event.button.button == SDL_BUTTON_RIGHT) {
                    right_button_pressed = 0;
                }
                break;
            case SDL_MOUSEWHEEL:
                scale += event.wheel.y;
                creat_and_render_to_texture_vector_field();
                break;
        }
    }
}

void update(void)
{
    vec2 current_mouse_position;
    vec2 diff;
    int mouse_x, mouse_y;
    float diff_length;

    fix_framerate();

    fps = 1.0f / delta_time;
    // printf("%d\n", (int)fps);

    char fps_count[100];
    sprintf(fps_count, "FPS = %8.4g  |  num of particals = %d/%d", fps, num_of_particals+1, MAX_NUMBER_OF_PARTICALES);
    text_surface = TTF_RenderText_Solid(font, fps_count,white_color);

    text_texture = SDL_CreateTextureFromSurface(renderer,text_surface);
    SDL_FreeSurface(text_surface);

    SDL_GetMouseState(&mouse_x, &mouse_y);
    current_mouse_position = vec2_new((float)mouse_x, (float)mouse_y);
    diff = vec2_sub(&current_mouse_position, &last_mouse_position);
    diff_length = vec2_length(&diff);
        
    if (left_button_pressed) {
        creat_and_render_to_texture_vector_field();

        if (!left_mouse_was_pressed) {
            last_mouse_position = vec2_add(&last_mouse_position, &current_mouse_position);
            left_mouse_was_pressed = 1;
        }
        if (diff_length > 0) {
            offset_x -= diff.x;
            offset_y += diff.y;
        } 
    }
    last_mouse_position = current_mouse_position;

    
    if (right_button_pressed) {
        // if (num_of_particals < 99) {
        //     num_of_particals++;
        // }
        particals[num_of_particals].x = current_mouse_position.x*(1/zoom);
        particals[num_of_particals].y = (WINDOW_HEIGHT - current_mouse_position.y)*(1/zoom);

    }

    update_particals();  
}

void render(void)
{
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    SDL_RenderCopy(renderer, vector_field_texture, NULL, NULL);
    
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    DrawCircle(renderer, (-offset_x)*zoom, WINDOW_HEIGHT+(offset_y)*zoom, a*zoom);
    
    for (int i = 0; i <= num_of_particals; i++) {
        fill_circle(renderer, particals[i].x*zoom, WINDOW_HEIGHT - particals[i].y*zoom, 10, turquoise_color);
    }

    SDL_RenderCopy(renderer, text_texture, NULL, &fps_place);

    SDL_RenderPresent(renderer);
}

void destroy_window(void)
{
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

void fix_framerate(void)
{
    static Uint32 previous_frame_time = 0;
    int time_ellapsed = SDL_GetTicks() - previous_frame_time;
    int time_to_wait = FRAME_TARGET_TIME - time_ellapsed;
    if (time_to_wait > 0 && time_to_wait < FRAME_TARGET_TIME) {
        // SDL_Delay(time_to_wait);
    }
    delta_time = (SDL_GetTicks() - previous_frame_time) / 1000.0f;
    previous_frame_time = SDL_GetTicks();
}

float liniar_map(float s, float min_in, float max_in, float min_out, float max_out)
{
    return (min_out + ((s-min_in)*(max_out-min_out))/(max_in-min_in));
}

void generate_vector_field(void)
{
    vec2 current_vector_c, current_vector_p;
    vec2 current_position_c, current_position_p;
    float length, theta;

    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            current_position_c.x = j*scale + offset_x;
            current_position_c.y = i*scale + offset_y;

            current_position_p = cartesian2polar(current_position_c);

            current_vector_p = my_field_function(current_position_p);
            current_vector_c = polar2cartesian(current_vector_p);
            theta = atan2f(current_vector_c.y, current_vector_c.x);
            length = vec2_length(&current_vector_c);
            
            MAT_AT(vector_field_theta, i, j) = theta;

            MAT_AT(vector_field_size, i, j) = length;
            if (fabsf(length) > 10) {
                continue;
            }
            if (length > max_length) {
                max_length = length;
            }
            if (length < min_length) {
                min_length = length;
            }

            if (theta > max_theta) {
                max_theta = theta;
            }
            if (theta < min_theta) {
                min_theta = theta;
            }
            
        }
    }
    if (!generated_vector_field) {
        final_max_length = max_length;
        final_min_length = min_length;
        final_max_theta = max_theta;
        final_min_theta = min_theta;
        generated_vector_field = 1;
        // dprintF(final_max_length);
        // dprintF(max_theta);
        // dprintF(min_theta);
    }
}

void render_draw_vector_field_to_texture(SDL_Renderer *renderer, SDL_Texture *texture)
{
    float length, theta;
    int in_cirle;
    vec2 origin, second_point, diff_between_points, current_position_c;
    
    SDL_SetRenderTarget(renderer, texture);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            theta = MAT_AT(vector_field_theta, i, j);
            length = MAT_AT(vector_field_size, i, j);

            origin = vec2_new(j*scale, i*scale);

            diff_between_points = vec2_new(
                length * cosf(theta) * vector_length_factor,
                length * sinf(theta) * vector_length_factor
            );

            second_point = vec2_add(&origin, &diff_between_points);

            current_position_c = vec2_new(origin.x + offset_x, origin.y + offset_y);        
            in_cirle = hide_center? vec2_length(&current_position_c) <= a: 0;
            
            theta = liniar_map(theta,min_theta,final_max_theta,0,1);
            length = liniar_map(length,min_length,35,0,1);
            
            SDL_SetRenderDrawColor(renderer,255*length,255*(1-length),0,255);
            // SDL_SetRenderDrawColor(renderer,255*theta,255*(1-theta),0,255);
            // SDL_SetRenderDrawColor(renderer,255,255,255,255);

            if (!in_cirle) {
                SDL_RenderDrawLineF(renderer, origin.x*zoom, WINDOW_HEIGHT - origin.y*zoom, second_point.x*zoom, WINDOW_HEIGHT - second_point.y*zoom);
            }
        }
    }
    SDL_SetRenderTarget(renderer, NULL);
}

vec2 V_source_p(vec2 v_in)
{
    float r = v_in.x;
    vec2 v_out = {
        .x = (Q / (2*PI*r)),
        .y = v_in.y
    };
    // vec2 v_out = {
    //     .x = 1*v_in.x,
    //     .y = 1*v_in.y
    // };

    return v_out;
}

vec2 V_vortex_p(vec2 v_in)
{
    float r = v_in.x;
    float theta = v_in.y;
    float u_theta = ((Gamma)/(2*PI*r)); 
    vec2 v_out = {
        .x = u_theta,
        .y = theta + PI/2
    };

    return v_out;

}

vec2 V_circle_p(vec2 v_in)
{
    vec2 vector_u_r, vector_u_theta, vector_u, v_out;
    float r, theta, u_r, u_theta;

    r = v_in.x;
    theta = v_in.y;
    u_r =  U_inf*cosf(theta)*(1-((a*a)/(r*r)));
    u_theta = -U_inf*sinf(theta)*(1+((a*a)/(r*r))) - ((Gamma)/(2*PI*r));
    vector_u_r = vec2_new(u_r * cosf(theta), u_r *sinf(theta));
    vector_u_theta = vec2_new(- u_theta*sinf(theta), u_theta * cosf(theta));
    vector_u = vec2_add(&vector_u_r, &vector_u_theta);
    v_out = vec2_new(
        vec2_length(&vector_u),
        atan2f(vector_u.y, vector_u.x)
    );

    return v_out;
}

vec2 polar2cartesian(vec2 v_in)
{
    float theta = v_in.y;
    float length = v_in.x;
    vec2 v_out = {
        .x = length*cosf(theta),
        .y = length*sinf(theta)
    };
    return v_out;
}

vec2 cartesian2polar(vec2 v_in)
{
    float theta = atan2f(v_in.y, v_in.x);
    float length = vec2_length(&v_in);
    vec2 v_out = {
        .x = length,
        .y = theta
    };
    return v_out;
}

void DrawCircle(SDL_Renderer * renderer, int32_t centreX, int32_t centreY, int32_t radius)
{
   const int32_t diameter = (radius * 2);

   int32_t x = (radius - 1);
   int32_t y = 0;
   int32_t tx = 1;
   int32_t ty = 1;
   int32_t error = (tx - diameter);

   while (x >= y)
   {
      //  Each of the following renders an octant of the circle
      SDL_RenderDrawPoint(renderer, centreX + x, centreY - y);
      SDL_RenderDrawPoint(renderer, centreX + x, centreY + y);
      SDL_RenderDrawPoint(renderer, centreX - x, centreY - y);
      SDL_RenderDrawPoint(renderer, centreX - x, centreY + y);
      SDL_RenderDrawPoint(renderer, centreX + y, centreY - x);
      SDL_RenderDrawPoint(renderer, centreX + y, centreY + x);
      SDL_RenderDrawPoint(renderer, centreX - y, centreY - x);
      SDL_RenderDrawPoint(renderer, centreX - y, centreY + x);

      if (error <= 0)
      {
         ++y;
         error += ty;
         ty += 2;
      }

      if (error > 0)
      {
         --x;
         tx += 2;
         error += (tx - diameter);
      }
   }
}

void fill_circle(SDL_Renderer *renderer, int x, int y, int radius, SDL_Color color)
{
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
    for (int w = 0; w < radius * 2; w++)
    {
        for (int h = 0; h < radius * 2; h++)
        {
            int dx = radius - w; // horizontal offset
            int dy = radius - h; // vertical offset
            if ((dx*dx + dy*dy) <= (radius * radius))
            {
                SDL_RenderDrawPoint(renderer, x + dx, y + dy);
            }
        }
    }
}

void update_particals(void)
{
    vec2 current_position_c, current_position_p, current_vector_c, current_vector_p;

    for (int i = 0; i <= num_of_particals; i++) {
        current_position_c = vec2_new((particals[i].x + offset_x)*zoom, (particals[i].y + offset_y)*zoom);
        current_position_p = cartesian2polar(current_position_c);
        current_vector_p = my_field_function(current_position_p);
        current_vector_c = polar2cartesian(current_vector_p);

        particals[i].v_x = current_vector_c.x*partical_velocity_factor;
        particals[i].v_y = current_vector_c.y*partical_velocity_factor;
        particals[i].x += particals[i].v_x*delta_time;
        particals[i].y += particals[i].v_y*delta_time;
    }
}

void creat_and_render_to_texture_vector_field(void)
{
    rows = (int)((WINDOW_HEIGHT * (1/zoom))/scale);
    cols = (int)((WINDOW_WIDTH * (1/zoom))/scale);

    vector_field_theta = mat_alloc(rows, cols);
    vector_field_size = mat_alloc(rows, cols);

    generate_vector_field();

    if (to_print) {
        MAT_PRINT(vector_field_size); 
        to_print = 0;
    }
    render_draw_vector_field_to_texture(renderer, vector_field_texture);
    
    free(vector_field_theta.elements);
    free(vector_field_size.elements);
}
