#include <stdio.h>
#include <stdbool.h>
// #include <SDL.h>
#include "../sdl2\include\SDL2\SDL.h"
#include "./constants.h"

///////////////////////////////////////////////////////////////////////////////
// Global variables
///////////////////////////////////////////////////////////////////////////////
int game_is_running = false;
int last_frame_time = 0;
SDL_Window *window = NULL;
SDL_Renderer *renderer = NULL;
const int FRAME_TARGET_TIME = 1000 / FPS;
const float PADDLE_VEL_X = 10.0 * 60.0 / FPS;
struct struct_frames
{
    long n;
    int get_ticks_init;
    int last_ticks;
};
typedef struct struct_frames frames_;
float delta_time;

///////////////////////////////////////////////////////////////////////////////
// Declare two game objects for the ball and the paddle
///////////////////////////////////////////////////////////////////////////////
struct game_object
{
    float x;
    float y;
    float width;
    float height;
    float vel_x;
    float vel_y;
} ball, paddle;

///////////////////////////////////////////////////////////////////////////////
// Function to initialize our SDL window
///////////////////////////////////////////////////////////////////////////////
int initialize_window(void)
{
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
    {
        fprintf(stderr, "Error initializing SDL.\n");
        return false;
    }
    // https://wiki.libsdl.org/SDL2/SDL_CreateWindow
    // flags 	0, or WITH_PADDLE or more SDL_WindowFlags OR'd together
    window = SDL_CreateWindow(
        "A simple game loop using C & SDL",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        WINDOW_WIDTH,
        WINDOW_HEIGHT,
        0);
    if (!window)
    {
        fprintf(stderr, "Error creating SDL Window.\n");
        return false;
    }
#ifdef VSYNC_AND_HARD_ACCELERATED
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
#else
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_SOFTWARE);
#endif
    if (!renderer)
    {
        fprintf(stderr, "Error creating SDL Renderer.\n");
        return false;
    }
    return true;
}

///////////////////////////////////////////////////////////////////////////////
// Function to poll SDL events and process keyboard input
///////////////////////////////////////////////////////////////////////////////
void process_input(void)
{
    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        switch (event.type)
        {
        case SDL_QUIT:
            game_is_running = false;
            break;
        case SDL_KEYDOWN:
            if (event.key.keysym.sym == SDLK_ESCAPE)
            {
                game_is_running = false;
            }
#ifdef WITH_PADDLE
            if (event.key.keysym.sym == SDLK_LEFT)
            {
                paddle.vel_x = -PADDLE_VEL_X;
                // puts("SDLK_LEFT");
            }
            if (event.key.keysym.sym == SDLK_RIGHT)
            {
                paddle.vel_x = +PADDLE_VEL_X;
                // puts("SDLK_RIGHT");
            }
            break;
        case SDL_KEYUP:
        {
            // puts("SDL_KEYUP");
            if (event.key.keysym.sym == SDLK_LEFT)
                paddle.vel_x = 0;
            if (event.key.keysym.sym == SDLK_RIGHT)
                paddle.vel_x = 0;
            break;
        }
#endif
        }
    }
}

///////////////////////////////////////////////////////////////////////////////
// Setup function that runs once at the beginning of our program
///////////////////////////////////////////////////////////////////////////////
void setup(void)
{
    // Initialize the ball object moving down at a constant velocity
    ball.x = 10;
    ball.y = 20;
    ball.width = 20;
    ball.height = 20;
    ball.vel_x = 360;
    ball.vel_y = 280;

    // Initialize the values for the paddle object
    paddle.width = 100;
    paddle.height = 20;
    paddle.x = (WINDOW_WIDTH / 2) - (paddle.width / 2);
    paddle.y = WINDOW_HEIGHT - 40;
    paddle.vel_x = 0;
    paddle.vel_y = 0;
}

///////////////////////////////////////////////////////////////////////////////
// Update function with a fixed time step
///////////////////////////////////////////////////////////////////////////////
void update(void)
{
    // Move ball as a function of delta time
    ball.x += ball.vel_x * delta_time;
    ball.y += ball.vel_y * delta_time;

    // Check for ball collision with the window borders
    if (ball.x < 0)
    {
        ball.x = 0;
        ball.vel_x = -ball.vel_x;
    }
    if (ball.x + ball.height > WINDOW_WIDTH)
    {
        ball.x = WINDOW_WIDTH - ball.width;
        ball.vel_x = -ball.vel_x;
    }
    if (ball.y < 0)
    {
        ball.y = 0;
        ball.vel_y = -ball.vel_y;
    }
#ifdef WITH_PADDLE
    paddle.x += paddle.vel_x;

    // Prevent paddle from moving outside the boundaries of the window
    if (paddle.x < 0)
        paddle.x = 0;
    if (paddle.x > WINDOW_WIDTH - paddle.width)
        paddle.x = WINDOW_WIDTH - paddle.width;
    // paddle.y += paddle.vel_y * delta_time;
    // Check for ball collision with the paddle
    if (ball.y + ball.height >= paddle.y && ball.x + ball.width >= paddle.x && ball.x <= paddle.x + paddle.width)
    {
        ball.y = -ball.height + paddle.y;
        ball.vel_y = -ball.vel_y;
    }
    if (ball.y + ball.height > WINDOW_HEIGHT)
    {
        ball.y = 0;
    }
#else
    if (ball.y + ball.height > WINDOW_HEIGHT)
    {
        ball.y = WINDOW_HEIGHT - ball.height;
        ball.vel_y = -ball.vel_y;
    }
#endif
}

///////////////////////////////////////////////////////////////////////////////
// Render function to draw game objects in the SDL window
///////////////////////////////////////////////////////////////////////////////
void render(void)
{
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);

    SDL_RenderClear(renderer);

    // Draw a rectangle for the ball object
    SDL_Rect ball_rect = {
        (int)ball.x,
        (int)ball.y,
        (int)ball.width,
        (int)ball.height};
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderFillRect(renderer, &ball_rect);

#ifdef WITH_PADDLE
    // Draw a rectangle for the paddle object
    SDL_Rect paddle_rect = {
        (int)paddle.x,
        (int)paddle.y,
        (int)paddle.width,
        (int)paddle.height};
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderFillRect(renderer, &paddle_rect);
#endif

    SDL_RenderPresent(renderer);
}

///////////////////////////////////////////////////////////////////////////////
// Function to destroy SDL window and renderer
///////////////////////////////////////////////////////////////////////////////
void destroy_window(void)
{
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

///////////////////////////////////////////////////////////////////////////////
// It is non blocking with a desired FPS
///////////////////////////////////////////////////////////////////////////////
const int M = 16;
void delay_1() // https://www.youtube.com/watch?v=C4uUeRlpAhY
{
    int elapsed_time = SDL_GetTicks() - last_frame_time;
    // printf("u %d\n", u);
    if (elapsed_time < M) // 16.66 = 1000 ms / (60 fps)
    {
        SDL_Delay(M - elapsed_time);
        // printf("yes %d\n", 16 - elapsed_time);
    }
    delta_time = (SDL_GetTicks() - last_frame_time) / 1000.0;
    last_frame_time = SDL_GetTicks();
}

void delay_2() // https://www.youtube.com/watch?v=C4uUeRlpAhY
{
    int elapsed_time = SDL_GetTicks() - last_frame_time;
    // printf("u %d\n", u);
    if (elapsed_time < FRAME_TARGET_TIME) // 16.66 = 1000 ms / (60 fps)
    {
        SDL_Delay(FRAME_TARGET_TIME - elapsed_time);
        // printf("yes %d\n", 16 - elapsed_time);
    }
    delta_time = (SDL_GetTicks() - last_frame_time) / 1000.0;
    last_frame_time = SDL_GetTicks();
}
///////////////////////////////////////////////////////////////////////////////
// Get the max from the machine
///////////////////////////////////////////////////////////////////////////////
void delay_SDL_RENDERER_PRESENTVSYNC() // SDL_RENDERER_PRESENTVSYNC
{
    // Get delta_time factor converted to seconds to be used to update objects
    delta_time = (SDL_GetTicks() - last_frame_time) / 1000.0;

    // Store the milliseconds of the current frame to be used in the next WITH_PADDLE
    last_frame_time = SDL_GetTicks();
}
void get_frame_per_sec(frames_ *frames)
{
    frames->n++;
    int get_ticks = SDL_GetTicks();
    // printf("%d\n", get_ticks - frames->last_ticks);
    if (get_ticks - frames->last_ticks > 1000)
    {
        frames->last_ticks = get_ticks;
        float frame_per_sec = (float)frames->n / (get_ticks - frames->get_ticks_init) * 1000.0;
        printf("frame_per_sec %f\n", frame_per_sec);
        delta_time = (SDL_GetTicks() - last_frame_time) / 1000.0;
        printf("delta_time %f\n", delta_time);
    }
}

///////////////////////////////////////////////////////////////////////////////
// Main function
///////////////////////////////////////////////////////////////////////////////
int main(int argc, char *args[])
{
    frames_ frames = {0, 0, 0};
    frames.get_ticks_init = SDL_GetTicks();
    printf("%ld %d\n", frames.n, frames.get_ticks_init);
    printf("FRAME_TARGET_TIME %d\n", FRAME_TARGET_TIME);
    // exit(0);

    game_is_running = initialize_window();

    setup();

    last_frame_time = SDL_GetTicks();

    while (game_is_running)
    {
        process_input();
        update();
#ifdef VSYNC_AND_HARD_ACCELERATED
        delay_SDL_RENDERER_PRESENTVSYNC();
#else
        delay_2();
#endif
        render();
        get_frame_per_sec(&frames);
    }

    destroy_window();

    return 0;
}

// Modified file FROM https://github.com/gustavopezzi/sdl-gameloop

// COMPILE WITH COMMAND LINE
// win
// gcc -std=c18 main.c -Isdl2\include\SDL2 -Lsdl2\lib -Wall -lmingw32 -lSDL2main -lSDL2 -o main
// mac (brew install SDL2)
// clear; rm main; gcc -std=c18 main_mac_v2.c -lSDL2 -o main; ./main

// COMPIL WITH SCONS
// cls;scons -Q;.\game.exe

// SEARCH A FILE IN WIN
// Get-ChildItem -Path C:\mingw64 -Filter *libmingw32* -Recurse -ErrorAction SilentlyContinue -Force | %{$_.FullName}
// Get-ChildItem -Path .\sdl2 -Filter *libsdl2* -Recurse -ErrorAction SilentlyContinue -Force | %{$_.FullName}

// BLOCKING WAIT
// while (!SDL_TICKS_PASSED(SDL_GetTicks(), last_frame_time + FRAME_TARGET_TIME))
//     ;
// // Get a delta time factor converted to seconds to be used to update my objects
// float delta_time = (SDL_GetTicks() - last_frame_time) / 1000.0;
