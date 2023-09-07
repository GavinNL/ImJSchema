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

std::string _schemaString;

inline IJS::json UNION (IJS::json const & Left, IJS::json const & right)
{
    IJS::json J = Left;
    J.merge_patch(right);
    return J;
};

inline IJS::json UNION (IJS::json const & Left, IJS::json const & right, IJS::json const &right2)
{
    IJS::json J = Left;
    J.merge_patch(right);
    J.merge_patch(right2);
    return J;
};



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

const nlohmann::json person = {
    {"type" , "object"},
    {"properties", {
                    {"first_name", IJS::basic_string},
                    {"last_name" , IJS::basic_string},
                    }
                 }
};

const IJS::json example_numbers {
    {"type", "object"},
    {"properties" ,
        {
         {"float"       , IJS::number},
         {"float_drag"  , UNION(IJS::number  , IJS::range(0.0f,1.0f) , { {"ui:widget", "drag"}, {"ui:speed", 0.001f} })},
         {"float_slider", UNION(IJS::number  , IJS::range(0.f,10.f)  , IJS::widget("slider")) },
         {"int"         , IJS::integer},
         {"int_slider"  , UNION(IJS::integer , IJS::range(0,10)      , IJS::widget("slider")) },
         {"int_drag"    , UNION(IJS::integer , IJS::range(0,10)      , IJS::widget("drag")) },
        }
    }
};

const IJS::json example_boolean = IJS::json::parse(R"foo(
{
    "ui:column_size" : 50,
    "type": "object",
    "properties": {
        "checkbox": {
            "default": false,
            "type": "boolean",
            "title" : "Check Box"
        },
        "enabledisable": {
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
})foo");

const IJS::json example_arrays {
    {"type", "object"},
    {"properties" ,
        {
            {"array"                 , IJS::array(IJS::number_normalized, 4, 10)},
            {"color3"                , IJS::color3},
            {"color4"                , IJS::color4},
            {"array_obj"             , IJS::array(person, 1, 10)}
        }
    }
};


const IJS::json example_strings {
    {"type", "object"},
    {"properties" ,
        {
         {"basic"   , IJS::basic_string},
         {"textarea", IJS::text_area},
         {"color", IJS::string_color},
         {"color_picker", IJS::string_color_picker},
         {"enum", {
                      {"type"     , "string" },
                      {"enum"     , {"wiz", "barb", "fighter", "warlock"} },
                      {"enumNames", {"Wizard", "Barbarian", "Fighter", "Warlock"} },
                      {"title", "Class"}
                  }},
         }
    }
};

nlohmann::json _schemaWithDefs;
nlohmann::json _schema = example_numbers;
nlohmann::json _value = nlohmann::json::object_t();
nlohmann::json _cache = nlohmann::json::object_t();

inline bool noYesButton(char const* no, char const * yes, bool * value, ImVec2 btnSize = {0,0})
{
    bool retValue = false;
    bool &_enable = *value;

    ImJSchema::json J1, J2;

    if(btnSize.x <= 0.0f)
        btnSize.x = ImGui::GetContentRegionAvail().x/ 2 - ImGui::GetStyle().ItemSpacing.x;
    auto buttonCol = ImGui::GetStyle().Colors[ImGuiCol_Button];
    auto buttonActiveCol = ImGui::GetStyle().Colors[ImGuiCol_ButtonActive];

    {
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, !_enable ? buttonActiveCol : buttonCol);
        ImGui::PushStyleColor(ImGuiCol_Button, !_enable ? buttonActiveCol : buttonCol);
        if(ImGui::Button(no, btnSize))
        {
            _enable = false;
            retValue = true;
        }
        ImGui::PopStyleColor(2);
    }

    ImGui::SameLine();

    {
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, _enable ? buttonActiveCol : buttonCol);
        ImGui::PushStyleColor(ImGuiCol_Button       , _enable ? buttonActiveCol : buttonCol);
        if(ImGui::Button(yes, btnSize))
        {
            _enable = true;
            retValue = true;
        }
        ImGui::PopStyleColor(2);
    }

    return retValue;
}



void runApp()
{
    if(_schemaString.empty())
    {
       // _schema =
       //     IJS::json{
       //         {"type", "object"},
       //         {"properties" ,
       //             {
       //                 {"numbers" , example_numbers},
       //                 {"booleans", example_boolean},
       //                 {"arrays"  , example_arrays},
       //                 {"strings" , example_strings},
       //                 {"object"  , person}
       //             }
       //         }
       //     };

        _schemaString = _schema.dump(4);
    }


    BeginFullScreen("Object");

    auto width = ImGui::GetContentRegionAvail().x / 3;
    auto height= ImGui::GetContentRegionAvail().y;

    static bool _enable = false;
    bool _update = false;

    //=========================================================================
    // Examples
    //=========================================================================
    {
        noYesButton("Disable", "Enable", &_enable);

        if(ImGui::Button("Number"))
        {
            _schemaWithDefs = IJS::json::parse(R"foo(
            {
                "type": "number"
            })foo");
            _schemaString = _schemaWithDefs.dump(4);
            _update = true;
        }
        ImGui::SameLine();
        if(ImGui::Button("boolean"))
        {
            _schemaWithDefs = IJS::json::parse(R"foo(
            {
                "type": "boolean"
            })foo");
            _schemaString = _schemaWithDefs.dump(4);
            _update = true;
        }
        ImGui::SameLine();
        if(ImGui::Button("string"))
        {
            _schemaWithDefs = IJS::json::parse(R"foo(
            {
                "type": "string"
            })foo");
            _schemaString = _schemaWithDefs.dump(4);
            _update = true;
        }
        ImGui::SameLine();
        if(ImGui::Button("Object"))
        {
            _schemaWithDefs = IJS::json::parse(R"foo(
            {
                "type": "object",
                "properties" : {
                    "number" : { "type" : "number"},
                    "bool" : { "type" : "boolean"},
                    "string" : { "type" : "string"}
                }
            })foo");
            _schemaString = _schemaWithDefs.dump(4);
            _update = true;
        }
        ImGui::SameLine();
        if(ImGui::Button("Array"))
        {
            _schemaWithDefs = IJS::json::parse(R"foo(
            {
                "type": "array",
                "items" : {
                    "type" : "number"
                }
            })foo");
            _schemaString = _schemaWithDefs.dump(4);
            _update = true;
        }
        if(ImGui::Button("Number Widgets"))
        {
            _schemaWithDefs = IJS::json::parse(R"foo(
            {
                "type": "object",
                "properties": {
                    "float": {
                        "default": 0.0,
                        "type": "number"
                    },
                    "float_drag": {
                        "default": 0.0,
                        "maximum": 1.0,
                        "minimum": 0.0,
                        "type": "number",
                        "ui:speed": 0.0010000000474974513,
                        "ui:widget": "drag"
                    },
                    "float_slider": {
                        "default": 0.0,
                        "maximum": 10.0,
                        "minimum": 0.0,
                        "type": "number",
                        "ui:widget": "slider"
                    },
                    "int": {
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
            })foo");
            _schemaString = _schemaWithDefs.dump(4);
            _update = true;
        }
        ImGui::SameLine();
        if(ImGui::Button("Boolean Widgets"))
        {
            _schemaWithDefs = IJS::json::parse(R"foo(
            {
                "ui:column_size" : 50,
                "type": "object",
                "properties": {
                    "checkbox": {
                        "default": false,
                        "type": "boolean",
                        "title" : "Check Box"
                    },
                    "enabledisable": {
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
            })foo");
            _schemaString = _schemaWithDefs.dump(4);
            _update = true;
        }
        ImGui::SameLine();
        if(ImGui::Button("String Widgets"))
        {
            _schemaWithDefs = IJS::json::parse(R"foo(
            {
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
                },
                "type": "object"
            })foo");
            _schemaString = _schemaWithDefs.dump(4);
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
            _schemaWithDefs = IJS::json::parse(R"foo({
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
                "items": {
                    "$ref": ["#/$defs/number", "#/$defs/normalized"]
                },
                "type": "array"
            })foo");
            _schemaString = _schemaWithDefs.dump(4);
            _update = true;
        }
        if(ImGui::Button("Ordering"))
        {
            _schemaWithDefs = IJS::json::parse(R"foo({
                "type": "object",
                "ui:order" : ["b", "c", "a"],
                "properties": {
                    "a": { "type": "string", "ui:help" : "Hover over the label to show this tooltip"},
                    "b": { "type": "string" },
                    "c": { "type": "string" }
                }
            })foo");
            _schemaString = _schemaWithDefs.dump(4);
            _update = true;
        }
        ImGui::SameLine();
        if(ImGui::Button("Constants"))
        {
            _schemaWithDefs = IJS::json::parse(R"foo({
                "type": "object",
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
            })foo");
            _schemaString = _schemaWithDefs.dump(4);
            _update = true;
        }
        ImGui::SameLine();
        if(ImGui::Button("Enumerated Types"))
        {
            _schemaWithDefs = IJS::json::parse(R"foo({
                "type": "object",
                "properties": {
                    "strings": {
                        "type": "string",
                        "enum" : ["wiz", "src", "war"]
                    },
                    "strings_with_names": {
                        "type": "string",
                        "title" : "String with Names",
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
            })foo");
            _schemaString = _schemaWithDefs.dump(4);
            _update = true;
        }

        if(ImGui::Button("D&D"))
        {
            _schemaWithDefs = IJS::json::parse(R"foo({
                "$defs" : {
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
                                "enum" : ["Human", "Dwarf", "Elf", "Halfling"]
                            },
                            "class" : {
                                "type" : "string",
                                "enum" : ["Wizard", "Sorcerer", "Warlock", "Fighter", "Barbarian", "Artificer", "Rogue", "Monk", "Paladin"]
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
            })foo");
            _schemaString = _schemaWithDefs.dump(4);
            _update = true;
        }


        if(_update)
        {
            _value.clear();
        }
    }


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
        ImGui::TextUnformatted(_value.dump(4).c_str());
        ImGui::TextUnformatted(_cache.dump(4).c_str());

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
    SDL_Window* window = SDL_CreateWindow("Dear ImGui SDL2+SDL_Renderer example", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1920, 1080, window_flags);
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


