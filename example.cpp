// Dear ImGui: standalone example application for SDL2 + SDL_Renderer
// (SDL is a cross-platform general purpose library for handling windows, inputs, OpenGL/Vulkan/Metal graphics context creation, etc.)
// If you are new to Dear ImGui, read documentation from the docs/ folder + read the top of imgui.cpp.
// Read online: https://github.com/ocornut/imgui/tree/master/docs

// Important to understand: SDL_Renderer is an _optional_ component of SDL2.
// For a multi-platform app consider using e.g. SDL+DirectX on Windows and SDL+OpenGL on Linux/OSX.

#define IMGUI_DEFINE_MATH_OPERATORS
#include "imgui.h"
#include "imgui_impl_sdl2.h"
#include "imgui_impl_sdlrenderer2.h"
#include <stdio.h>
#include <SDL.h>

#if !SDL_VERSION_ATLEAST(2,0,17)
#error This backend requires SDL 2.0.17+ because of SDL_RenderGeometry() function
#endif

#include <iostream>
#include <ImJSchema/imgui_json_ui.h>

namespace IJS = ImJSchema;


bool BeginFullScreen(const char* name, bool* p_open = nullptr, ImGuiWindowFlags _flags = 0)
{
    static bool use_work_area = true;
    static ImGuiWindowFlags flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings | _flags;

    // We demonstrate using the full viewport area or the work area (without menu-bars, task-bars etc.)
    // Based on your use case you may want one or the other.
    const ImGuiViewport* viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(use_work_area ? viewport->WorkPos : viewport->Pos);
    ImGui::SetNextWindowSize(use_work_area ? viewport->WorkSize : viewport->Size);

    return ImGui::Begin("Example: Fullscreen window", p_open, flags);
}


const IJS::json example_arrays {
    {"type", "object"},
    {"properties" ,
        {
            {"array"                 , IJS::array(IJS::number_normalized, 4, 10)},
            {"color3"                , IJS::color3},
            {"color4"                , IJS::color4}
        }
    }
};


std::string _schemaString = R"foo(
{
    "type": "number"
})foo";
nlohmann::json _schemaWithDefs;
nlohmann::json _schema;
nlohmann::json _value = nlohmann::json::object_t();
nlohmann::json _cache = nlohmann::json::object_t();


void runApp()
{
    if(_schemaString.empty())
    {
        _schemaString = _schema.dump(4);
    }


    BeginFullScreen("Object");

    static bool _enable = false;
    static bool _update = true;

    //=========================================================================
    // Examples
    //=========================================================================
    {
        if(ImGui::Button("Number"))
        {
            _schemaString = R"foo(
{
    "type": "number",
"description" : " Hello"
})foo";
            _update = true;
        }
        ImGui::SameLine();
        if(ImGui::Button("boolean"))
        {
            _schemaString = R"foo(
{
    "type": "boolean"
})foo";
            _update = true;
        }
        ImGui::SameLine();
        if(ImGui::Button("string"))
        {
            _schemaString = R"foo(
{
    "type": "string"
})foo";
            _update = true;
        }
        ImGui::SameLine();
        if(ImGui::Button("Object"))
        {
            _schemaString = R"foo(
{
    "type": "object",
    "properties" : {
        "number" : { "type" : "number"},
        "bool" : { "type" : "boolean"},
        "string" : { "type" : "string"},
        "object" : {
            "type" : "object",
            "description" : "Objects within an object type will show up as a Collapsable header",
            "properties" : {
                 "number" : { "type" : "number"}
            }
        }
    }
})foo";
            _update = true;
        }
        ImGui::SameLine();
        if(ImGui::Button("Array"))
        {
            _schemaString = R"foo(
{
    "type": "array",
    "items" : {
        "type" : "number"
    }
})foo";
            _update = true;
        }
        if(ImGui::Button("Number Widgets"))
        {
            _schemaString =
R"foo({
    "type": "object",
    "properties": {
        "float": {
            "description" : "Default floating point number. if ui:step is not provided, the increment buttons will not show. Default ui:step_fast is 10x ui:step",
            "default": 0.0,
            "type": "number",
            "ui:step": 1,
            "ui:step_fast": 10
        },
        "float_slider": {
            "description" : "Slider widgets require both a minimum and maximum property to be set. Will default to drag widget if either of these are not provided",
            "default": 0.0,
            "maximum": 10.0,
            "minimum": 0.0,
            "type": "number",
            "ui:widget": "slider"
        },
        "float_drag": {
            "description" : "Drag widgets",
            "default": 0.0,
            "maximum": 1.0,
            "minimum": 0.0,
            "type": "number",
            "ui:speed": 0.0010000000474974513,
            "ui:widget": "drag"
        },
        "int": {
            "description" : "Integer widgets can be used as well, by setting the type to \"integer\"",
            "default": 0,
            "type": "integer",
            "ui:step": 1,
            "ui:step_fast": 10
        },
        "int_drag": {
            "default": 0,
            "maximum": 10,
            "minimum": 0,
            "type": "integer",
            "ui:step": 1,
            "ui:step_fast": 10,
            "ui:widget": "drag"
        },
        "int_slider": {
            "default": 0,
            "maximum": 10,
            "minimum": 0,
            "type": "integer",
            "ui:step": 1,
            "ui:step_fast": 10,
            "ui:widget": "slider"
        }
    }
})foo";
            _update = true;
        }
        ImGui::SameLine();
        if(ImGui::Button("Boolean Widgets"))
        {
            _schemaString = R"foo({
    "description" : "Boolean widgets. This object makes use of the \"ui:column_size\" property.",
    "ui:column_size" : 50,
    "type": "object",
    "properties": {
        "checkbox": {
            "description" : "The default is checkbox",
            "default": false,
            "type": "boolean",
            "title" : "Check Box"
        },
        "enabledisable": {
            "description" : "But there are also different styles. See Enumerated Types for an alternative method",
            "default": false,
            "type": "boolean",
            "ui:widget": "enabledisable",
            "title" : "Enable/Disable"
        },
        "truefalse": {
            "default": false,
            "type": "boolean",
            "ui:widget": "truefalse",
            "title" : "True/False"
        },
        "yesno": {
            "default": true,
            "type": "boolean",
            "ui:widget": "yesno",
            "title" : "Yes/No"
        }
    }
})foo";
            _update = true;
        }
        ImGui::SameLine();
        if(ImGui::Button("String Widgets"))
        {
            _schemaString =
R"foo({
    "type": "object"
    "ui:order" : ["basic", "textarea", "color", "color_picker"],
    "properties": {
        "basic": {
            "type": "string"
        },
        "color": {
            "type": "string",
            "ui:widget": "color"
        },
        "color_picker": {
            "type": "string",
            "ui:widget": "color_picker"
        },
        "textarea": {
            "type": "string",
            "ui:options": {
                "rows": 5
            },
            "ui:widget": "textarea"
        }
    }
})foo";
            _update = true;
        }
        ImGui::SameLine();
        if(ImGui::Button("Array Widgets"))
        {
            _schema = example_arrays;
            _schemaString = _schema.dump(4);
            _update = true;
        }
        ImGui::SameLine();
        if(ImGui::Button("$Def"))
        {
            _schemaString =
R"foo({
    "$defs": {
        "number": {
            "default": 0.75,
            "type": "number"
        },
        "normalized": {
            "maximum": 1.0,
            "minimum": 0.0,
            "ui:widget" : "slider"
        }
    },
    "description" : "You can use references using the '$ref' keyword.",
    "properties" : {
        "single_ref" : {
            "description" : "Explicit properties are override the definition",
            "$ref": "#/$defs/number"
        },
        "multi_ref" : {
            "description" : "Multiple references can be can be provided in an array. Earlier items are overridden by later values",
            "$ref": ["#/$defs/number", "#/$defs/normalized"]
        }
    },
    "type": "object"
})foo";
            _update = true;
        }
        if(ImGui::Button("Ordering"))
        {
            _schemaString =
R"foo({
    "type": "object",
    "description" : "By default, properties in an object will be displayed in alphabetical order. You can set the order the widgets appear by setting the ui:order property. If you do not list the widget, it will not show up in the list",
    "ui:order" : ["b", "c", "a"],
    "properties": {
        "a": { "type": "string", "ui:help" : "Hover over the label to show this tooltip"},
        "b": { "type": "string" },
        "c": { "type": "string" },
        "d": { "type": "string" }
    }
})foo";
            _update = true;
        }
        ImGui::SameLine();
        if(ImGui::Button("Constants"))
        {
            _schemaString =
R"foo({
    "type": "object",
    "description" : "You can hide constant values in the form by setting the 'default', 'ui:hidden', and 'ui:disabled' properties.",
    "properties": {
        "constant_value": {
            "type": "string",
            "default" : "This is the default value",
            "ui:hidden" : true,
            "ui:disabled" : true
        },
        "b": { "type": "string" },
        "c": { "type": "string" }
    }
})foo";
            _update = true;
        }
        ImGui::SameLine();
        if(ImGui::Button("Enumerated Types"))
        {
            _schemaString =
R"foo({
    "type": "object",
    "properties": {
        "strings": {
            "type": "string",
            "enum" : ["wiz", "src", "war"]
        },
        "strings_with_names": {
            "type": "string",
            "title" : "String with Names",
            "description" : "When using \"ui:widget\" : \"buttons\", You can set \"ui:options/columns\" to be a number to set the total number of columns for the buttons ",
            "enum" : ["wiz", "sorc", "war", "fight", "barb", "art", "rogue", "monk", "pal"],
            "enumNames" : ["Wizard", "Sorcerer", "Warlock", "Fighter", "Barbarian", "Artificer", "Rogue", "Monk", "Paladin"],
            "ui:widget" : "button",
            "ui:options" : {
               "columns" : 3
            }
        },
        "numbers": {
            "type": "number",
            "title" : "Numbers",
            "enum" : [1,2,3]
        },
        "numbers_with_names": {
            "type": "number",
            "title" : "Numbers With Names",
            "enum" : [1,2,3],
            "enumNames" : ["first", "second", "third"]
        },
        "booleans": {
            "type": "boolean",
            "title" : "boolean",
            "enum" : [false, true]
        },
        "booleans_with_names": {
            "type": "boolean",
            "title" : "Boolean with Names",
            "enum" : [false, true],
            "enumNames" : ["False", "True"]
        }
    }
})foo";
            _update = true;
        }

        if(ImGui::Button("Descriptions"))
        {
            _schemaString = R"foo(
{
    "description" : "Each schema object can have a \"description\" property to display visible text",
    "type": "object",
    "ui:order" : ["name", "age", "imguiAwesome", "object"],
    "properties" : {
        "name" : {
            "type" : "string",
            "title" : "Name",
            "description" : "Please enter the name wish to be called"
        },
        "age" : {
            "type" : "number",
            "title" : "Age",
            "description" : "The age you will be at the end of this year"
        },
        "imguiAwesome" : {
            "type" : "boolean",
            "title" : "Is ImGui Awesome?",
            "description" : "Check this box if you think ImGui is awesome."
        },
        "object" : {
            "type" : "object",
            "description" : "Objects within an object type will show up as a Collapsable header",
            "properties" : {
                 "number" : { "type" : "number"}
            }
        }
    }
})foo";
            _update = true;
        }
        ImGui::SameLine();
        if(ImGui::Button("Help"))
        {
            _schemaString = R"foo(
{
    "description" : "You can use the \"ui:help\" : \"tool tip text\" to show tooltip. Hover over the object label",
    "type": "object",
    "ui:order" : ["name", "age", "imguiAwesome", "object"],
    "properties" : {
        "name" : {
            "type" : "string",
            "title" : "Name",
            "ui:help" : "Please enter the name wish to be called"
        },
        "age" : {
            "type" : "number",
            "title" : "Age",
            "ui:help" : "The age you will be at the end of this year"
        },
        "imguiAwesome" : {
            "type" : "boolean",
            "title" : "Is ImGui Awesome?",
            "ui:help" : "Check this box if you think ImGui is awesome."
        },
        "object" : {
            "type" : "object",
            "ui:help" : "Objects within an object type will show up as a Collapsable header",
            "properties" : {
                 "number" : { "type" : "number"}
            }
        }
    }
})foo";
            _update = true;
        }

        if(ImGui::Button("D&D"))
        {
            _schemaString =
R"foo({
    "$defs" : {
        "class_list" : ["Wizard", "Sorcerer", "Warlock", "Fighter", "Barbarian", "Artificer", "Rogue", "Monk", "Paladin"],
        "race_list" : ["Human", "Dwarf", "Elf", "Halfling"],
        "stat" : {
            "type" : "integer",
            "ui:step" : 1,
            "minimum" : 1,
            "maximum" : 20,
            "default" : 10
        },
        "character" : {
            "type" : "object",
            "ui:order" : ["name", "race", "class", "alignment_1", "alignment_2", "stats"],
            "properties" : {
                "name" : {"type" : "string"},
                "race" : {
                    "type" : "string",
                    "enum" : { "$ref" : "#/$defs/race_list" }
                },
                "class" : {
                    "type" : "string",
                    "enum" : { "$ref" : "#/$defs/class_list" }
                },
                "alignment_1" : {
                    "type" : "string",
                    "enum" : ["Lawful", "Neutral", "Chaotic"],
                    "ui:widget" : "button"
                },
                "alignment_2" : {
                    "type" : "string",
                    "enum" : ["Good", "Neutral", "Evil"],
                    "ui:widget" : "button"
                },
                "stats" : {
                    "type" : "object",
                    "properties" : {
                        "str" : { "$ref" : "#/$defs/stat" },
                        "con" : { "$ref" : "#/$defs/stat" },
                        "dex" : { "$ref" : "#/$defs/stat" },
                        "int" : { "$ref" : "#/$defs/stat" },
                        "wis" : { "$ref" : "#/$defs/stat" },
                        "cha" : { "$ref" : "#/$defs/stat" }
                    }
                }
            }
        }
    },
    "type": "object",
    "properties": {
        "party" : {
            "type" : "array",
            "items" : {
                "$ref" : "#/$defs/character"
            }
        }
    }
})foo";
            _update = true;
        }

        if(_update)
        {
            _value.clear();
        }
    }


    auto width = ImGui::GetContentRegionAvail().x / 3;
    auto height= ImGui::GetContentRegionAvail().y;
    if(ImGui::BeginChild("Schema", {width, 0}))
    {
        ImGui::PushItemWidth(-1);
        if(ImGui::InputTextMultiline("##Schema", &_schemaString, {width, height}) || _update)
        {
            try
            {
                auto J = IJS::json::parse(_schemaString);

                IJS::jsonExpandAllReferences(J);
                _schema = std::move(J);
            }
            catch(std::exception & e)
            {

            }
            _update = false;
        }
        ImGui::PopItemWidth();

        ImGui::EndChild();
    }

    ImGui::SameLine();

    if(ImGui::BeginChild("form", {width, 0}))
    {
        if(IJS::drawSchemaWidget("object",
                                  _value,
                                  _schema,
                                  _cache))
        {
            //std::cout << _value.dump(4) << std::endl;
        }
        ImGui::EndChild();
    }

    ImGui::SameLine();

    if(ImGui::BeginChild("Value", {width, 0}))
    {
        if(ImGui::CollapsingHeader("Output JSON", ImGuiTreeNodeFlags_DefaultOpen))
        {
            ImGui::TextUnformatted(_value.dump(4).c_str());
        }
        if(ImGui::CollapsingHeader("Schema Cache", ImGuiTreeNodeFlags_DefaultOpen))
        {
            ImGui::TextUnformatted(_cache.dump(4).c_str());
        }

        ImGui::EndChild();
    }


    ImGui::End();

    //ImGui::Begin("Value");
    //ImGui::TextUnformatted(_value.dump(4).c_str());
    //ImGui::End();

    ImGui::ShowDemoWindow(nullptr);
}


// Main code
int main(int, char**)
{
    // Setup SDL
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_GAMECONTROLLER) != 0)
    {
        printf("Error: %s\n", SDL_GetError());
        return -1;
    }

// From 2.0.18: Enable native IME.
#ifdef SDL_HINT_IME_SHOW_UI
    SDL_SetHint(SDL_HINT_IME_SHOW_UI, "1");
#endif

    // Create window with SDL_Renderer graphics context
    SDL_WindowFlags window_flags = (SDL_WindowFlags)(SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI);
    SDL_Window* window = SDL_CreateWindow("ImJSchema: Build ImGui Forms with JSON", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1920, 1080, window_flags);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_PRESENTVSYNC | SDL_RENDERER_ACCELERATED);
    if (renderer == nullptr)
    {
        SDL_Log("Error creating SDL_Renderer!");
        return 0;
    }
    //SDL_RendererInfo info;
    //SDL_GetRendererInfo(renderer, &info);
    //SDL_Log("Current SDL_Renderer: %s", info.name);

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsLight();

    // Setup Platform/Renderer backends
    ImGui_ImplSDL2_InitForSDLRenderer(window, renderer);
    ImGui_ImplSDLRenderer2_Init(renderer);

    // Load Fonts
    // - If no fonts are loaded, dear imgui will use the default font. You can also load multiple fonts and use ImGui::PushFont()/PopFont() to select them.
    // - AddFontFromFileTTF() will return the ImFont* so you can store it if you need to select the font among multiple.
    // - If the file cannot be loaded, the function will return a nullptr. Please handle those errors in your application (e.g. use an assertion, or display an error and quit).
    // - The fonts will be rasterized at a given size (w/ oversampling) and stored into a texture when calling ImFontAtlas::Build()/GetTexDataAsXXXX(), which ImGui_ImplXXXX_NewFrame below will call.
    // - Use '#define IMGUI_ENABLE_FREETYPE' in your imconfig file to use Freetype for higher quality font rendering.
    // - Read 'docs/FONTS.md' for more instructions and details.
    // - Remember that in C/C++ if you want to include a backslash \ in a string literal you need to write a double backslash \\ !
    //io.Fonts->AddFontDefault();
    //io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\segoeui.ttf", 18.0f);
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/DroidSans.ttf", 16.0f);
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/Roboto-Medium.ttf", 16.0f);
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/Cousine-Regular.ttf", 15.0f);
    //ImFont* font = io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\ArialUni.ttf", 18.0f, nullptr, io.Fonts->GetGlyphRangesJapanese());
    //IM_ASSERT(font != nullptr);

    // Our state
    bool show_demo_window = true;
    bool show_another_window = false;
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);


    // Main loop
    bool done = false;
    while (!done)
    {
        // Poll and handle events (inputs, window resize, etc.)
        // You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
        // - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application, or clear/overwrite your copy of the mouse data.
        // - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application, or clear/overwrite your copy of the keyboard data.
        // Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            ImGui_ImplSDL2_ProcessEvent(&event);
            if (event.type == SDL_QUIT)
                done = true;
            if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_CLOSE && event.window.windowID == SDL_GetWindowID(window))
                done = true;
        }

        // Start the Dear ImGui frame
        ImGui_ImplSDLRenderer2_NewFrame();
        ImGui_ImplSDL2_NewFrame();
        ImGui::NewFrame();


        runApp();

        // Rendering
        ImGui::Render();
        SDL_RenderSetScale(renderer, io.DisplayFramebufferScale.x, io.DisplayFramebufferScale.y);
        SDL_SetRenderDrawColor(renderer, (Uint8)(clear_color.x * 255), (Uint8)(clear_color.y * 255), (Uint8)(clear_color.z * 255), (Uint8)(clear_color.w * 255));
        SDL_RenderClear(renderer);
        ImGui_ImplSDLRenderer2_RenderDrawData(ImGui::GetDrawData());
        SDL_RenderPresent(renderer);
    }
    // Cleanup
    ImGui_ImplSDLRenderer2_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}


#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui_impl_sdl2.cpp>
#include <imgui_impl_sdlrenderer2.cpp>
#include <imgui_demo.cpp>
#include <imgui_widgets.cpp>
#include <imgui_draw.cpp>
#include <imgui_tables.cpp>
#include <imgui.cpp>
#include <misc/cpp/imgui_stdlib.cpp>


