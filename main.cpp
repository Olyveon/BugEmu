#include <string>

#include"bugCpu.h"
#include "SDL3/SDL.h"
#include "SDL3/SDL_main.h"

static SDL_Window *window = NULL;
static SDL_Renderer *renderer = NULL;

/* This function runs once at startup. */
SDL_AppResult SDL_AppInit(void **appstate, int argc, char *argv[])
{
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Couldn't initialize SDL!", SDL_GetError(), NULL);
        return SDL_APP_FAILURE;
    }

    if (!SDL_CreateWindowAndRenderer("BugEmu", 512, 480, 0, &window, &renderer)) {
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Couldn't create window/renderer!", SDL_GetError(), NULL);
        return SDL_APP_FAILURE;
    }

    return SDL_APP_CONTINUE;  /* carry on with the program! */
}

// This function runs once per frame
SDL_AppResult SDL_AppIterate(void *appstate) {
    SDL_SetRenderDrawColor(renderer, 255, 100, 100, 255);
    SDL_RenderClear(renderer);

    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderDebugText(renderer, 180, 100, "Welcome to BugEmu!!");
    SDL_RenderPresent(renderer);

    return SDL_APP_CONTINUE;
}

// This function runs once when the window quits
void SDL_AppQuit(void *appstate, SDL_AppResult result) {
    // SDL will take care of the window/render, any other things that we need to destroy, do it here
}

SDL_AppResult SDL_AppEvent(void *appstate, SDL_Event *event) {
    switch (event->type) {
        case SDL_EVENT_QUIT:
            // end the program, reporting success to the OS
            return SDL_APP_SUCCESS;
        case SDL_EVENT_KEY_DOWN:
            if (event->key.key == SDLK_ESCAPE)
            return SDL_APP_SUCCESS;
        default:
            break;
    }

    return SDL_APP_CONTINUE;
}

int main() {
    bugCpu cpu;
    cpu.filepath = "C:/Users/felip/CLionProjects/BugEmu/test_roms/6_Instructions2.nes";
    cpu.reset();
    return 0;
}