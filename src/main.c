#include <SDL2/SDL.h>
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_mouse.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_video.h>
#include <stdbool.h>
#include <math.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "images.h"

#define PET_VALUE 300.f
#define PET_BASE 2
#define PET_MULTIPLIER 1
#define RETURN_SPEED 10

#define MOD(x, y) ((x) - floorf((x) / (y)) * (y))

SDL_Texture* make_texture(unsigned char* buf, int len, SDL_Renderer* renderer) {
    int width, height, channels;
    unsigned char* data = stbi_load_from_memory(buf, len, &width, &height, &channels, 4);
    SDL_Surface* surface = SDL_CreateRGBSurfaceFrom(data, width, height, 32, 4 * width, 0x000000FF, 0x0000FF00, 0x00FF0000, 0xFF000000);
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);
    return texture;
}

int get_pet_state(float counter, int maxval) {
    float x = MOD(counter, PET_VALUE) / PET_VALUE;
    if (x < 0.5) x = x * 2;
    else x = x * -2 + 2;
    return maxval * x;
}

int main() {
    SDL_Init(SDL_INIT_VIDEO);
    SDL_Window* window = SDL_CreateWindow("Niko Petting Simulator", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 480, 480, 0);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    SDL_Texture* niko_petting = make_texture(niko_pancakes, niko_pancakes_len, renderer);
    SDL_Texture* niko_petted = make_texture(niko_83c, niko_83c_len, renderer);
    SDL_Texture* niko_default = make_texture(niko, niko_len, renderer);
    SDL_Texture* curr_niko = niko_default;
    SDL_Cursor* cursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_HAND);
    float pet_counter = 0;
    int prev_mouse_x = 0;
    int prev_mouse_y = 0;
    bool mouse_down = false;
    while (true) {
        int mouse_x, mouse_y;
        SDL_GetMouseState(&mouse_x, &mouse_y);
        int delta_x = mouse_x - prev_mouse_x;
        int delta_y = mouse_y - prev_mouse_y;
        prev_mouse_x = mouse_x;
        prev_mouse_y = mouse_y;
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) goto exit;
            if (event.type == SDL_MOUSEBUTTONDOWN) {
                mouse_down = true;
                curr_niko = niko_petting;
            }
            if (event.type == SDL_MOUSEBUTTONUP) {
                mouse_down = false;
                curr_niko = niko_petted;
            }
        }
        if (mouse_down) pet_counter += sqrt(pow(delta_x, 2) + pow(delta_y, 2)) * PET_MULTIPLIER + PET_BASE;
        else {
            float x = MOD(pet_counter, PET_VALUE);
            if (x < PET_VALUE / 2) x -= RETURN_SPEED;
            else x += RETURN_SPEED;
            if (x < 0) x = 0;
            if (x > PET_VALUE) x = PET_VALUE;
            pet_counter = x;
        }
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        int nikow = get_pet_state(pet_counter,  128) + 480;
        int nikoh = get_pet_state(pet_counter, -128) + 480;
        SDL_Rect dst;
        dst.x = nikow / -2 + 240;
        dst.y = 480 - nikoh;
        dst.w = nikow;
        dst.h = nikoh;
        SDL_Rect src;
        src.x = 0;
        src.y = 0;
        unsigned int fmt;
        int access;
        SDL_QueryTexture(curr_niko, &fmt, &access, &src.w, &src.h);
        SDL_RenderCopy(renderer, curr_niko, &src, &dst);
        SDL_RenderPresent(renderer);
        SDL_SetCursor(cursor);
        SDL_Delay(16);
    }
    exit:
    SDL_FreeCursor(cursor);
    SDL_DestroyTexture(niko_default);
    SDL_DestroyTexture(niko_petting);
    SDL_DestroyTexture(niko_petted);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}