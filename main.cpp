
#include "App.h"
#include <ImJSchema/ImJSchema.h>
#include "examples.h"

namespace IJS = ImJSchema;

struct MyApplication : public MainApplication
{
public:

    IJS::json _schema;
    IJS::json _value = IJS::json::object_t();
    IJS::json _cache = IJS::json::object_t();
    IJS::json::json_pointer _lastModifiedpath;
    IJS::json _lastModifiedValue;

    std::string _schemaString = basic_number;

    MyApplication()
    {
        _schema =IJS::json::parse(_schemaString);

        // lets define our own number widget by creating a lambda function
        // inside the widget map
        IJS::detail::widgets_all["number/my_custom_number_widget"] =
            [](IJS::WidgetDrawInput & in) -> bool
        {
            auto W = ImGui::GetContentRegionAvail().x;

            // Use the "cache" object to store any temporary data
            // that may be used for drawing your widget
            float w = IJS::JValue(in.cache, "pos", 0.0f);
            w += 1.0f;
            if(w > W)
                w = 0;
            in.cache["pos"] = w;

            if( ImGui::Button(in.label, {w,0}) )
            {
                // when you set the value
                // make sure you return true
                in.value = in.value.get<float>() + 1.0f;
                return true;
            }

            return false;
        };
    }

    /**
     * @brief imgui_render
     *
     * Put all your imgui rendering code here
     */
    void imgui_render() override
    {
        static bool _update = false;
        BeginFullScreen("Main Window");

#define EXAMPLE_(VAR)\
        if(ImGui::Button(#VAR))\
        {\
            _schemaString = VAR;\
            _update = true;\
        }

        EXAMPLE_(basic_number)  ImGui::SameLine();
        EXAMPLE_(basic_boolean) ImGui::SameLine();
        EXAMPLE_(basic_string)  ImGui::SameLine();
        EXAMPLE_(basic_object)  ImGui::SameLine();
        EXAMPLE_(basic_array)

        EXAMPLE_(titles)  ImGui::SameLine();
        EXAMPLE_(descriptions)  ImGui::SameLine();
        EXAMPLE_(ordering)  ImGui::SameLine();
        EXAMPLE_(help)  ImGui::SameLine();
        EXAMPLE_(optional_properties)

        EXAMPLE_(number_widgets)  ImGui::SameLine();
        EXAMPLE_(boolean_widgets)  ImGui::SameLine();
        EXAMPLE_(string_widgets)  ImGui::SameLine();
        EXAMPLE_(array_widgets)  ImGui::SameLine();
        EXAMPLE_(object_widgets)  ImGui::SameLine();
        EXAMPLE_(constants)  ImGui::SameLine();
        EXAMPLE_(enumerated_types)

        EXAMPLE_(references) ImGui::SameLine();
        EXAMPLE_(DnD) ImGui::SameLine();
        EXAMPLE_(PBR)

        EXAMPLE_(custom_widgets)

        if(_update)
        {
            _value.clear();
            _cache.clear();
            _lastModifiedpath = IJS::json::json_pointer{};
        }


        auto width = ImGui::GetContentRegionAvail().x / 3;
        auto height= ImGui::GetContentRegionAvail().y;

        if(ImGui::BeginChild("Schema", {width, 0}))
        {
            ImGui::PushItemWidth(-1);
            if(ImGui::InputTextMultiline("##Schema", &_schemaString, {width, height}) || _update)
            {
                auto J = IJS::json::parse(_schemaString, nullptr, false);
                if(J.type() == IJS::json::value_t::discarded)
                {
                    J = IJS::json::object_t();
                    J["type"] = "object";
                    J["description"] = "Error Parsing json";
                }

                // POI: If the json Schema contains $ref then we need to expand the
                // json object.
                IJS::jsonExpandAllReferences(J);

                _schema = std::move(J);
                _update = false;
            }
            ImGui::PopItemWidth();

            ImGui::EndChild();
        }

        ImGui::SameLine();


        if(ImGui::BeginChild("form", {width, 0}))
        {
            // POI: This is the main function that is used to draw the actual widget
            //
            // It requires 3 json objects:
            //    1. a json object that the final value will be stored
            //    2. a json object that defines the schema
            //    3. a json object that can be use to cache data
            //
            // The WidgetDrawInput struct contains references to the objects
            IJS::WidgetDrawInput in { "object", _value, _schema, _cache};

            // Draw the schema as a widget and return true if any of the values
            // have been modified
            if(IJS::drawSchemaWidget(in))
            {
                // We can get which widget within the entire Schema was modified using the
                // getModifiedWidgetPath() function which returns a json_pointer object
                //std::cout << "Last Edited JSON path: " << IJS::getModifiedWidgetPath() << std::endl;

                if(_value.contains(IJS::getModifiedWidgetPath()))
                {
                    _lastModifiedValue = _value.at(IJS::getModifiedWidgetPath());
                }
                _lastModifiedpath = IJS::getModifiedWidgetPath();
            }
            ImGui::EndChild();
        }

        ImGui::SameLine();

        if(ImGui::BeginChild("Value", {width, 0}))
        {
            if(ImGui::CollapsingHeader("Last Modified path", ImGuiTreeNodeFlags_DefaultOpen))
            {
                ImGui::TextUnformatted( _lastModifiedpath.to_string().c_str());
                ImGui::TextUnformatted(_lastModifiedValue.dump(4).c_str());
            }
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
    }

    bool BeginFullScreen(const char* name, bool* p_open = nullptr, ImGuiWindowFlags _flags = 0)
    {
        static bool use_work_area = true;
        static ImGuiWindowFlags flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings | _flags;

        // We demonstrate using the full viewport area or the work area (without menu-bars, task-bars etc.)
        // Based on your use case you may want one or the other.
        const ImGuiViewport* viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(use_work_area ? viewport->WorkPos : viewport->Pos);
        ImGui::SetNextWindowSize(use_work_area ? viewport->WorkSize : viewport->Size);

        return ImGui::Begin(name, p_open, flags);
    }
};

MyApplication App;


void main_loop()
{
    App.main_loop();
}


int main(int, char**)
{
    if(!App.initSDL("ImJSchema: Make ImGui forms with JSON", 1920, 1080))
    {
        return EXIT_FAILURE;
    }

    App.initImGui();

#ifdef __EMSCRIPTEN__
    //emscripten_set_canvas_element_size("#canvas", 1920, 1080);
    emscripten_set_main_loop(main_loop, 0, 1);
#else
    while (!App.g_done)
    {
        App.main_loop();
    }
#endif

    App.destroySDL();

    return EXIT_SUCCESS;
}



// Include the header with warnings disabled
#define IMGUI_DEFINE_MATH_OPERATORS
#include "imgui.h"

#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui_impl_sdl2.cpp>
#include <imgui_impl_sdlrenderer2.cpp>

#include <imgui_demo.cpp>
#include <imgui_widgets.cpp>
#include <imgui_draw.cpp>
#include <imgui_tables.cpp>
#include <imgui.cpp>
#include <imgui_stdlib.cpp>

