//
// TNT (Triage Narrative Tool) created by 5' Digital. 
// All rights reserved.
//

#include <stdio.h>

#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

#include "app.h"

int main(int argc, char* argv[]) {
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS);
    printf("TNT\n");

    auto* app = new tnt::App();
    
    uint64_t startTicks = SDL_GetTicksNS();
    uint64_t lastFrameTicks = 0;
    while (!app->wantsExit()) {
        // Poll the events.
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            app->handleSdlEvent(event);
        }

        // Step the frame.
        app->doFrame((float)lastFrameTicks / 1000.f);

        // Calculate the delta time.
        lastFrameTicks = SDL_GetTicksNS() - startTicks;
    }

    return 0;
}