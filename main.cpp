#include <iostream>
#include <string>
#include"bugCpu.h"
#include "SDL3/SDL.h"
#include "SDL3/SDL_main.h"

// ImGui includes
#include "imgui.h"
#include "imgui_impl_sdl3.h"
#include "imgui_impl_sdlrenderer3.h"


static SDL_Window *window = nullptr;
static SDL_Renderer *renderer = nullptr;

// we use a struct to maintain app state
struct AppState {
    bugCpu cpu;
    bool show_debug_window = true;
};

/* This function runs once at startup. Replaces main()*/
SDL_AppResult SDL_AppInit(void **appstate, int argc, char *argv[])
{
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        return SDL_APP_FAILURE;
    }

    if (!SDL_CreateWindowAndRenderer("BugEmu", 1024, 768, 0, &window, &renderer)) {
        return SDL_APP_FAILURE;
    }

    // Rewritten old main() code
    auto* as = new AppState();
    as->cpu.filepath = "C:/Users/felip/CLionProjects/BugEmu/test_roms/6_Instructions2.nes";
    *appstate = as;

    // Configure ImGui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

    ImGui::StyleColorsDark();

    // Backends for SDL_Renderer3
    ImGui_ImplSDL3_InitForSDLRenderer(window, renderer);
    ImGui_ImplSDLRenderer3_Init(renderer);

    return SDL_APP_CONTINUE;  /* carry on with the program! */
}

// This function runs once per frame
SDL_AppResult SDL_AppIterate(void *appstate) {
    auto* as = (AppState*)appstate;

    ImGui_ImplSDLRenderer3_NewFrame();
    ImGui_ImplSDL3_NewFrame();
    ImGui::NewFrame();

    if (ImGui::BeginMainMenuBar()) {
        if (ImGui::BeginMenu("File")) {
            ImGui::Text("%s", as->cpu.filepath.c_str());
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Emulation")) {
            if (ImGui::MenuItem("Run")) {
                as->cpu.run();
            }
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Debug")) {
            ImGui::Checkbox("Show Debug", &as->show_debug_window);
            // todo: add log check box to debug window
            ImGui::Checkbox("Log", &as->cpu.logging);
            ImGui::EndMenu();
        }
        ImGui::EndMainMenuBar();
    }

    // debug window
    // todo: create new functions for restarting the whole emulator (restarting flags, registers, memory, etc.) to differentiate from reset cpu signal
    // todo: add QoL for the debug window, make it a sepparate window for easier view
    if (as->show_debug_window) {
        ImGui::Begin("BugEmu Debugger", &as->show_debug_window);

        ImGui::Text("Estado de la CPU:");
        ImGui::Separator();
        // Registers and important data
        ImGui::Text("PC: 0x%04X", as->cpu.PC);
        ImGui::Text("SP: 0x%02X", as->cpu.stackPointer);
        ImGui::Text("A:  0x%02X", as->cpu.A);
        ImGui::Text("X:  0x%02X", as->cpu.X);
        ImGui::Text("Y:  0x%02X", as->cpu.Y);

        if (ImGui::BeginTable("TraceLog", 2, ImGuiTableFlags_Borders |
                                      ImGuiTableFlags_RowBg   |
                                      ImGuiTableFlags_ScrollY,
                                      ImVec2(0, 300))) // fixed height, scrollable
        {
            ImGui::TableSetupScrollFreeze(0, 1); // freeze header row
            ImGui::TableSetupColumn("Disassembly");
            ImGui::TableSetupColumn("Registers & Flags");
            ImGui::TableHeadersRow();

            for (const auto& entry : as->cpu.traceLog)
            {
                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0); ImGui::Text("%s", entry.disassembly.c_str());
                ImGui::TableSetColumnIndex(1); ImGui::Text("%s %s Cycle:%d", entry.registers.c_str(), entry.flags.c_str(), entry.cycles);
            }

            // Auto-scroll to bottom when new entries arrive
            if (ImGui::GetScrollY() >= ImGui::GetScrollMaxY())
                ImGui::SetScrollHereY(1.0f);

            ImGui::EndTable();
        }

        ImGui::Separator();
        if (ImGui::Button("Run one CPU cycle")) {
            as->cpu.clock();
        }

        if (ImGui::Button("Reset")) {
            as->cpu.reset();
        }

        ImGui::End();
    }

    SDL_SetRenderDrawColor(renderer, 255, 100, 100, 255);
    SDL_RenderClear(renderer);

    // Here would go NES PPU frame
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderDebugText(renderer, 180, 100, "Emulator running!!");

    ImGui::Render();
    ImGui_ImplSDLRenderer3_RenderDrawData(ImGui::GetDrawData(), renderer);

    SDL_RenderPresent(renderer);

    return SDL_APP_CONTINUE;
}

// This function runs once when the window quits
void SDL_AppQuit(void *appstate, SDL_AppResult result) {
    // SDL will take care of the window/render, any other things that we need to destroy, do it here
}

SDL_AppResult SDL_AppEvent(void *appstate, SDL_Event *event) {
    // Send events to ImGui too
    ImGui_ImplSDL3_ProcessEvent(event);

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
