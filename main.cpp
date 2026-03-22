#include <iostream>
#include <string>
#include "bugCpu.h"
#include "SDL3/SDL.h"
#include "SDL3/SDL_main.h"

// ImGui includes
#include "imgui.h"
#include "imgui_impl_sdl3.h"
#include "imgui_impl_sdlrenderer3.h"
#include "cmake-build-release/_deps/imgui-src/imgui_internal.h"

#include <nfd.h>
#include <filesystem>
#include <format>

#include "bugNES.h"


static SDL_Window *window = nullptr;
static SDL_Renderer *renderer = nullptr;
static ImGuiContext* mainContext = nullptr;
static SDL_Window *traceLoggerWindow = nullptr;
static SDL_Renderer *traceLoggerRenderer = nullptr;
static ImGuiContext *traceLoggerContext = nullptr;
bool show_debug_window = false;

// we use a struct to maintain app state
struct AppState {
    bugNES nes;
};

// We use Native File Dialog for our file needs
char* openFile() {
    NFD_Init();

    nfdu8char_t *outPath;
    nfdu8filteritem_t filters[1] = { { "NES file", "nes" }};
    nfdopendialogu8args_t args = {0};
    args.filterList = filters;
    args.filterCount = 1;
    nfdresult_t result = NFD_OpenDialogU8_With(&outPath, &args);
    if (result == NFD_OKAY)
    {
        puts("Success!");
        puts(outPath);
        NFD_FreePathU8(outPath);
        return outPath;
    }
    if (result != NFD_CANCEL)
    {
        printf("Error: %s\n", NFD_GetError());
    }
    NFD_Quit();
    return nullptr;
}

/* This function runs once at startup. Replaces main()*/
SDL_AppResult SDL_AppInit(void **appstate, int argc, char *argv[])
{
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        return SDL_APP_FAILURE;
    }

    if (!SDL_CreateWindowAndRenderer("BugEmu", 1024, 768, SDL_WINDOW_RESIZABLE, &window, &renderer)) {
        return SDL_APP_FAILURE;
    }

    // Rewritten old main() code
    auto* as = new AppState();
    *appstate = as;

    // Configure ImGui
    IMGUI_CHECKVERSION();
    mainContext = ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

    ImGui::StyleColorsDark();

    // Backends for SDL_Renderer3
    ImGui_ImplSDL3_InitForSDLRenderer(window, renderer);
    ImGui_ImplSDLRenderer3_Init(renderer);


    return SDL_APP_CONTINUE;  /* carry on with the program! */
}

void openDebugWindow() {
    if (traceLoggerWindow) return;

    traceLoggerWindow = SDL_CreateWindow("Trace Logger", 800, 600, SDL_WINDOW_RESIZABLE);
    traceLoggerRenderer = SDL_CreateRenderer(traceLoggerWindow, nullptr);

    ImGuiContext* previousContext = ImGui::GetCurrentContext();

    traceLoggerContext = ImGui::CreateContext();
    ImGui::SetCurrentContext(traceLoggerContext);

    ImGui_ImplSDL3_InitForSDLRenderer(traceLoggerWindow, traceLoggerRenderer);
    ImGui_ImplSDLRenderer3_Init(traceLoggerRenderer);

    ImGui::SetCurrentContext(previousContext);
}

void closeDebugWindow(void *appstate) {
    auto* as = (AppState*)appstate;

    if (!traceLoggerWindow) return;

    ImGui::SetCurrentContext(traceLoggerContext);
    ImGui_ImplSDLRenderer3_Shutdown();
    ImGui_ImplSDL3_Shutdown();
    ImGui::DestroyContext(traceLoggerContext);

    SDL_DestroyRenderer(traceLoggerRenderer);
    SDL_DestroyWindow(traceLoggerWindow);

    traceLoggerWindow   = nullptr;
    traceLoggerRenderer = nullptr;
    traceLoggerContext  = nullptr;
    show_debug_window = false;
    as->nes.cpu.logging = false;
}

void renderMain(void *appstate) {
    auto* as = (AppState*)appstate;

    if (ImGui::BeginMainMenuBar()) {
        if (ImGui::BeginMenu("File")) {
            if (ImGui::MenuItem("Open..")) {
                char* filepath = openFile();
                if (filepath) {
                    as->nes.cart.filepath = filepath;
                    std::string new_title = std::format("BugEmu - {}", std::filesystem::path(as->nes.cart.filepath).stem().string());
                    SDL_SetWindowTitle(window, new_title.c_str());
                    as->nes.cart.insertCartridge();
                    as->nes.cpu.reload();
                    // as->cpu.run();
                }

            }
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Emulation")) {
            if (ImGui::MenuItem("Run")) as->nes.cpu.run();
            if (ImGui::MenuItem("Reload")) as->nes.cpu.reload();
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Debug")) {
            if (ImGui::MenuItem("Trace Logger")) {
                openDebugWindow();          // only opens if not already open
                show_debug_window = true;
                as->nes.cpu.logging = true;
            }
            ImGui::EndMenu();
        }
        ImGui::EndMainMenuBar();
    }
    SDL_SetRenderDrawColor(renderer, 255, 90, 100, 255);
    SDL_RenderClear(renderer);
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 100);
    SDL_RenderDebugText(renderer, 180, 100, "Emulator running!!");

}

void renderDebug(void *appstate) {
    auto* as = (AppState*)appstate;

    // get the SDL window size
    int w, h;
    SDL_GetWindowSize(traceLoggerWindow, &w, &h);

    // force imgui window to match it exactly
    ImGui::SetNextWindowPos(ImVec2(0, 0));
    ImGui::SetNextWindowSize(ImVec2((float)w, (float)h));

    // these flags remove all window decorations so it truly fills the space
    ImGuiWindowFlags flags = ImGuiWindowFlags_NoTitleBar      |
                                 ImGuiWindowFlags_NoResize         |
                                 ImGuiWindowFlags_NoMove           |
                                 ImGuiWindowFlags_NoScrollbar      |
                                 ImGuiWindowFlags_NoCollapse       |
                                 ImGuiWindowFlags_NoBringToFrontOnFocus;

    ImGui::Begin("BugEmu Debugger", &show_debug_window, flags);


    ImGui::Text("CPU State:");
    ImGui::Separator();
    ImGui::Text("PC: 0x%04X", as->nes.cpu.PC);
    ImGui::Text("SP: 0x%02X", as->nes.cpu.stackPointer);
    ImGui::Text("A:  0x%02X", as->nes.cpu.A);
    ImGui::Text("X:  0x%02X", as->nes.cpu.X);
    ImGui::Text("Y:  0x%02X", as->nes.cpu.Y);

    if (ImGui::BeginTable("TraceLog", 2, ImGuiTableFlags_Borders |
                                             ImGuiTableFlags_RowBg   |
                                             ImGuiTableFlags_ScrollY,
                                             ImVec2(0, 300)))
    {
        ImGui::TableSetupScrollFreeze(0, 1);
        ImGui::TableSetupColumn("Disassembly");
        ImGui::TableSetupColumn("Registers & Flags");
        ImGui::TableHeadersRow();

        for (const auto& entry : as->nes.cpu.traceLog) {
            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0); ImGui::Text("%X:", entry.programCounter);
            ImGui::SameLine(); ImGui::Text("%s", entry.instruction.c_str());
            ImGui::SetItemTooltip("Opcode: $%X", entry.opcode);      // We make the table easier to read, but we still have the same information available if necessary
            ImGui::SameLine(); ImGui::Text("%s",entry.operand.c_str());
            ImGui::TableSetColumnIndex(1); ImGui::Text("%s %s Cycle:%d", entry.registers.c_str(), entry.flags.c_str(), entry.cycles);
        }

        if (ImGui::GetScrollY() >= ImGui::GetScrollMaxY())
            ImGui::SetScrollHereY(1.0f);

        ImGui::EndTable();
        }

    ImGui::Separator();
    if (ImGui::Button("Run one CPU cycle")) as->nes.cpu.clock();
    if (ImGui::Button("Reset"))             as->nes.cpu.reset();
    if (ImGui::Button("Continue until next instruction")) as->nes.cpu.continue_instruction();

    ImGui::End();

}

// This function runs once per frame
SDL_AppResult SDL_AppIterate(void *appstate) {
    auto* as = (AppState*)appstate;

    // --- Main Window ---
    ImGui::SetCurrentContext(mainContext);
    ImGui_ImplSDLRenderer3_NewFrame();
    ImGui_ImplSDL3_NewFrame();
    ImGui::NewFrame();

    renderMain(as);

    ImGui::Render();
    ImGui_ImplSDLRenderer3_RenderDrawData(ImGui::GetDrawData(), renderer);
    SDL_RenderPresent(renderer);

    // --- Debug Window ---
    if (show_debug_window && traceLoggerWindow) {
        ImGui::SetCurrentContext(traceLoggerContext);
        ImGui_ImplSDLRenderer3_NewFrame();
        ImGui_ImplSDL3_NewFrame();
        ImGui::NewFrame();

        renderDebug(as);


        if (!show_debug_window)
            closeDebugWindow(appstate);
        else {
            ImGui::Render();
            SDL_SetRenderDrawColor(traceLoggerRenderer, 30, 30, 30, 255);
            SDL_RenderClear(traceLoggerRenderer);
            ImGui_ImplSDLRenderer3_RenderDrawData(ImGui::GetDrawData(), traceLoggerRenderer);
            SDL_RenderPresent(traceLoggerRenderer);
        }


    }

    return SDL_APP_CONTINUE;
}


// This function runs once when the window quits
void SDL_AppQuit(void *appstate, SDL_AppResult result) {
    // SDL will take care of the window/render, any other things that we need to destroy, do it here
}

SDL_AppResult SDL_AppEvent(void* appstate, SDL_Event* event) {
    AppState* state = (AppState*)appstate;

    // handle window close for both windows
    if (event->type == SDL_EVENT_WINDOW_CLOSE_REQUESTED) {
        if (event->window.windowID == SDL_GetWindowID(window))
            return SDL_APP_SUCCESS; // exits the app

        if (traceLoggerWindow &&
            event->window.windowID == SDL_GetWindowID(traceLoggerWindow))
        {
            closeDebugWindow(state);
            return SDL_APP_CONTINUE;
        }
    }

    // route imgui events to the correct context based on which window they came from
    uint32_t eventWindowID = 0;

    // extract windowID depending on event type
    switch (event->type) {
        case SDL_EVENT_MOUSE_MOTION:
        case SDL_EVENT_MOUSE_BUTTON_DOWN:
        case SDL_EVENT_MOUSE_BUTTON_UP:
        case SDL_EVENT_MOUSE_WHEEL:
            eventWindowID = event->motion.windowID;
            break;
        case SDL_EVENT_KEY_DOWN:
        case SDL_EVENT_KEY_UP:
        case SDL_EVENT_TEXT_INPUT:
            eventWindowID = event->key.windowID;
            break;
        default:
            break;
    }

    if (traceLoggerWindow &&
        eventWindowID == SDL_GetWindowID(traceLoggerWindow))
    {
        // route to debug imgui context
        ImGui::SetCurrentContext(traceLoggerContext);
        ImGui_ImplSDL3_ProcessEvent(event);
    } else {
        // route to main imgui context
        ImGui::SetCurrentContext(mainContext);
        ImGui_ImplSDL3_ProcessEvent(event);
    }

    // handle  game inputs here (only for main window)
    if (!traceLoggerWindow ||
        eventWindowID != SDL_GetWindowID(traceLoggerWindow))
    {
        // normal game input handling
    }

    return SDL_APP_CONTINUE;
}
