//
// This header provides functions to
// draw ImGui Forms using json schemas
//
//
//
#ifndef SNE_IMGUI_JSON_UI_H
#define SNE_IMGUI_JSON_UI_H

#define IMGUI_DEFINE_MATH_OPERATORS
#include "imgui.h"

#include <nlohmann/json.hpp>
#include <misc/cpp/imgui_stdlib.h>
#include "imgui_widgets_t.h"
#include "json_utils.h"
#include <sstream>
#include <charconv>
#include <iomanip>

namespace ImJSchema
{

using json = nlohmann::json;

/**
 * @brief drawSchemaWidget
 * @param label
 * @param propertyValue
 * @param propertySchema
 * @param cache
 * @return
 *
 * This is the main function you should use. The other functions
 * are used internally. You may use them if you know what you are doing.
 *
 * Draws a widget with the appropriate schema.
 * The cache object is used to store temporary variables used by the UI
 * (eg: the index of a dropdown menu)
 *
 * Example:
 *
 * ImGui::Begin();
 *
 * static ImJSchema::json value = {};
 * static ImJSchema::json cache = {};
 * static ImJSchema::json schema = json::parse(R"foo({
 *     "type": "object",
 *     "properties": {
 *         "float": {
 *             "default": 0.0,
 *             "type": "number"
 *         },
 *         "float_drag": {
 *             "default": 0.0,
 *             "maximum": 1.0,
 *             "minimum": 0.0,
 *             "type": "number",
 *             "ui:speed": 0.0010000000474974513,
 *             "ui:widget": "drag"
 *         },
 *         "float_slider": {
 *             "default": 0.0,
 *             "maximum": 10.0,
 *             "minimum": 0.0,
 *             "type": "number",
 *             "ui:widget": "slider"
 *         },
 *         "int": {
 *             "default": 0,
 *             "type": "integer"
 *         },
 *         "int_drag": {
 *             "default": 0,
 *             "maximum": 10,
 *             "minimum": 0,
 *             "type": "integer",
 *             "ui:widget": "drag"
 *         },
 *         "int_slider": {
 *             "default": 0,
 *             "maximum": 10,
 *             "minimum": 0,
 *             "type": "integer",
 *             "ui:widget": "slider"
 *         }
 *     }
 * })foo");
 *
 * if( drawSchemaWidget("test", value, schema, cache) )
 * {
 * }
 *
 * ImGui::End();
 *
 */
bool drawSchemaWidget(char const *label, json & propertyValue, json const & propertySchema, json &cache);


inline const json boolean = json{
    {"type" , "boolean"},
    {"default" , false}
};

inline const json number = json{
    {"type" , "number"},
    {"default" , 0.0f}
};

inline const json integer = json{
    {"type" , "integer"},
    {"default" , 0},
    {"ui:step" , 1},
    {"ui:step_fast" , 10}
};

inline const json number_normalized = json{
    {"type" , "number"},
    {"minimum" , 0.0f},
    {"maximum" , 1.0},
    {"default" , 0.0f},
    {"ui:speed" , 0.0001f},
    {"ui:widget" , "drag"}
};

inline const json integer_unsigned = json{
    {"type" , "integer"},
    {"minimum" , 0},
    {"default" , 0},
    {"ui:speed" , 0.0f},
    {"ui:step" , 1}
};

inline const json basic_string = json{
    {"type" , "string"}
};

inline const json string_color = json{
                                      {"type" , "string"},
                                      {"ui:widget" , "color"},
                                      };

inline const json string_color_picker = json{
                                      {"type" , "string"},
                                      {"ui:widget" , "color_picker"},
                                      };

inline const json text_area = json{
    {"type" , "string"},
    {"ui:widget", "textarea"},
    {"ui:options", {{"rows" , 5} } }
};

inline const json vec2 = json{
    {"type" , "array"},
    {"minItems" , 2},
    {"maxItems" , 2},
    {"default" , {0,0} },
    {"items",  number},
    {"ui:widget" , "number_list"}
};

inline const json vec3 = json{
    {"type" , "array"},
    {"minItems" , 3},
    {"maxItems" , 3},
    {"default" , {0,0,0} },
    {"items",  number},
    {"class",  "vec3"},
    {"ui:widget" , "number_list"}
};

inline const json vec4 = json{
    {"type" , "array"},
    {"minItems" , 4},
    {"maxItems" , 4},
    {"default" , {0,0,0,0} },
    {"items",  number},
    {"class",  "vec4"},
    {"ui:widget" , "number_list"}
};

inline const json color3 = json{
    {"type" , "array"},
    {"minItems" , 3},
    {"maxItems" , 3},
    {"default" , {0,0,0} },
    {"ui:widget" , "color"},
    {"items",  number_normalized},
    {"class",  "vec3"}
};

inline const json color4 = json{
    {"type" , "array"},
    {"default" , {0,0,0,0} },
    {"minItems" , 4},
    {"maxItems" , 4},
    {"items",  number_normalized},
    {"ui:widget" , "color"},
    {"class",  "vec4"}
};



template<typename T>
inline json range(T min, T max)
{
    return {
        {"minimum", min},
        {"maximum", max}
    };
}

inline json array(json const & items, uint32_t minItems, uint32_t maxItems)
{
    return{
        {"type", "array"},
        {"items", items},
        {"minItems", minItems},
        {"maxItems", maxItems}
    };
}


inline json array(json const & items, uint32_t itemCount)
{
    return array(items, itemCount, itemCount);
}

template<typename T>
inline json initial_value(T const &v)
{
    return json{
        {"default" , v}
    };
}

inline json widget(std::string const &v)
{
    return json{
        {"ui:widget" , v}
    };
}






/**
 * @brief drawSchemaWidget_Object
 * @param label
 * @param objectValue
 * @param schema
 * @return
 *
 * This is the main function you should be using to draw an object.
 */
bool drawSchemaWidget_Object(char const * label, json & objectValue, json const & schema, json &cache);

bool drawSchemaArray(char const *label, json & value, json const & schema, json &cache);


/**
 * @brief _getDefault
 * @param schema
 * @return
 *
 * Returns a json object of the default value in the schema.
 * if the schema defines a default property, that value is returned.
 * If it does not, a standard default value will be returned based
 * on the "type" of the object.
 */
inline json _getDefault(const json & schema)
{
    json J;
    auto & _type  = schema.at("type");

    auto it = schema.find("default");
    if(_type == "number")
    {
        if(it == schema.end() || !it->is_number())
            return 0.0f;
        J = *it;
    }
    else if(_type == "string")
    {
        if(it == schema.end() || !it->is_string())
            return std::string();
        J = *it;
    }
    else if(_type == "boolean")
    {
        if(it == schema.end() || !it->is_boolean())
            return false;
        J = *it;
    }
    else if(_type == "integer")
    {
        if(it == schema.end() || !it->is_number_integer())
            return 0;
        J = *it;
    }
    else if(_type == "array")
    {
        if(it == schema.end() || !it->is_array())
            return json::array_t();
        J = *it;
    }
    else if( _type == "object")
    {
        if(it == schema.end() || !it->is_object())
            return json::object_t();
        J = *it;
    }
    return J;
}


inline std::map<std::string, std::function<bool(char const*, json&, json const&)> > widgets_numbers {
    {
        "slider",
        [](char const* label, json & value, json const& _schema) -> bool
        {
            if(value.is_number_float())
            {
                double & _value = value.get_ref<double&>();
                double minimum = _schema.value("minimum", 0.0);
                double maximum = _schema.value("maximum", minimum + 10.0);
                if(SliderScalar_T<double>(label, &_value, minimum, maximum))
                {
                    _value = std::clamp(_value, minimum, maximum);
                    value = _value;
                    return true;
                }
            }
            else if(value.is_number_integer())
            {
                int64_t & _value = value.get_ref<int64_t&>();
                int64_t minimum = _schema.value("minimum", 0);
                int64_t maximum = _schema.value("maximum", minimum + 10);
                if(SliderScalar_T<int64_t>(label, &_value, minimum, maximum))
                {
                    _value = std::clamp(_value, minimum, maximum);
                    value = _value;
                    return true;
                }
            }
            return false;
        }
    },
    {
        "drag",
        [](char const* label, json & value, json const& _schema) -> bool
        {
            auto _speed  = _schema.value("ui:speed", 1.0f );
            if(value.is_number_float())
            {
                double & _value = value.get_ref<double&>();
                double minimum = _schema.value("minimum", 0.0);
                double maximum = _schema.value("maximum", minimum + 10.0);
                if(DragScalar_T<double>(label, &_value, _speed,minimum, maximum))
                {
                    _value = std::clamp(_value, minimum, maximum);
                    value = _value;
                    return true;
                }
            }
            else if(value.is_number_integer())
            {
                int64_t & _value = value.get_ref<int64_t&>();
                int64_t minimum = _schema.value("minimum", 0);
                int64_t maximum = _schema.value("maximum", minimum + 10);
                if(DragScalar_T<int64_t>(label, &_value, _speed, minimum, maximum))
                {
                    _value = std::clamp(_value, minimum, maximum);
                    value = _value;
                    return true;
                }
            }
            return false;
        }
    }
};


inline bool drawSchemaWidget_Number(char const *label, json & value, json const & schema,json & cache)
{
    // These are the main two you should use
    // according to the json schema
    auto it = schema.find("type");

    if(it == schema.end())
        return false;

    if(*it == "number" && !value.is_number_float())
        value  = _getDefault(schema);

    if(*it == "integer" && !value.is_number_integer())
        value  = _getDefault(schema);

    auto widget_it = schema.find("ui:widget");
    if(widget_it != schema.end() && widget_it->is_string())
    {
        auto _widdraw_it = widgets_numbers.find( *widget_it);
        if(_widdraw_it != widgets_numbers.end() && _widdraw_it->second)
        {
            return _widdraw_it->second(label, value, schema);
        }
    }
    else
    {
        if( *it == "number" )
        {
            auto step      = schema.value("ui:step"     , std::numeric_limits<double>::max() );
            auto step_fast = schema.value("ui:step_fast", std::numeric_limits<double>::max() );
            double & _value = value.get_ref<double&>();
            return InputScalar_T<double>(label,&_value, step, step_fast);
        }
        if( *it == "integer" )
        {
            auto step      = schema.value("ui:step"     , std::numeric_limits<int64_t>::max() );
            auto step_fast = schema.value("ui:step_fast", std::numeric_limits<int64_t>::max() );
            int64_t & _value = value.get_ref<int64_t&>();
            return InputScalar_T<int64_t>(label,&_value, step, step_fast);
        }
        return false;
    }



    return false;
}


/**
 * @brief drawSchemaWidget_enum
 * @param label
 * @param value
 * @param schema
 * @return
 *
 * Draw an string enum widget (combo box). Don't use this directly.
 */
inline bool drawSchemaWidget_enum(char const * label, json & value, json const & schema)
{
    bool return_value = false;
    ImGuiComboFlags _flags = 0;
    std::string _label = schema.count("title") == 1 ? schema.at("title").get<std::string>() : std::string();

    if(!value.is_string())
    {
        if(schema.at("enum").size())
        {
            value = schema.at("enum").front();
        }
        else
        {
            value = "";
        }
    }

    auto _enum = schema.find("enum");
    auto _enumNames = schema.find("enumNames");

    if(!_enum->is_array())
        return false;

    if(_enumNames == schema.end())
        _enumNames = _enum;

    auto enumNamesSize = _enumNames->size();
    auto enumSize = _enum->size();

    std::string & value_str = value.get_ref<std::string &>();
    if (ImGui::BeginCombo( _label.empty() ? label : _label.c_str(), value_str.c_str(), _flags))
    {
        json const * _default = (schema.count("default") == 1 && schema.at("default").is_string()) ? &schema.at("default") : nullptr;

        //auto & items = schema.at("enum");
        //json const * enumNames = &items;

        if(value.empty() && _default != nullptr)
        {
            value = _default->get<std::string>();
        }

        for (size_t n = 0; n < enumSize; n++)
        {
            const bool is_selected = _enum->at(n) == value;

            auto & nameArray = n < enumNamesSize ? _enumNames : _enum;

            std::string const & label = nameArray->at(n).get_ref<std::string const&>();

            if (ImGui::Selectable( label.c_str(), is_selected))
            {
                if(value != _enum->at(n))
                {
                    value = _enum->at(n);//items[n];
                    return_value = true;
                }
            }

            // Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
            if (is_selected)
                ImGui::SetItemDefaultFocus();
        }
        ImGui::EndCombo();
    }
    return return_value;
}

inline bool drawSchemaWidget_enum2(char const * label, json & value, json const & schema, json & _cache)
{
    bool return_value = false;
    ImGuiComboFlags _flags = 0;
    std::string _label = schema.count("title") == 1 ? schema.at("title").get<std::string>() : std::string();

    if(!value.is_string())
    {
        if(schema.at("enum").size())
        {
            value = schema.at("enum").front();
        }
        else
        {
            value = "";
        }
    }

    auto _enum      = schema.find("enum");
    auto _enumNames = schema.find("enumNames");

    if(_enumNames == schema.end() || !_enumNames->is_array())
        return false;

    if(!_cache.is_object())
        _cache = json::object_t();

    uint32_t index = _cache.value("enumIndex", uint32_t(0) );
    index = std::min<uint32_t>(index, _enumNames->size()-1 );
    std::string const &  value_str = _enumNames->at(index).get_ref<std::string const&>();

    uint32_t totalEnums = static_cast<uint32_t>(std::min(_enum->size(), _enumNames->size()));
    if(ImGui::BeginCombo( _label.empty() ? label : _label.c_str(), value_str.c_str(), _flags))
    {
        for(uint32_t i=0; i< totalEnums; i++)
        {
            std::string const & label = _enumNames->at(i).get_ref<std::string const&>();
            bool is_selected = index == i;
            if (ImGui::Selectable( label.c_str(), is_selected))
            {
                if( index != i)
                {
                    value = _enum->at(i);
                    return_value = true;
                    _cache["enumIndex"] = i;
                }
            }
        }
        ImGui::EndCombo();
    }
    return return_value;
}


inline ImVec4 _hexStringToColor(std::string const & col)
{
    ImVec4 out = {0,0,0,1};
    uint32_t x = 0;
    try
    {
        x = std::stoul(col, nullptr, 16);
    }
    catch ( std::exception & e)
    {

    }

    out.x = static_cast<float>( (x >> 0) & 0xFF)  / 255.0f;
    out.y = static_cast<float>( (x >> 8) & 0xFF)  / 255.0f;
    out.z = static_cast<float>( (x >> 16) & 0xFF)  / 255.0f;
    out.w = static_cast<float>( (x >> 24) & 0xFF)  / 255.0f;

    return out;
}


inline std::map<std::string, std::function<bool(char const*, json&, json const&)> > widgets_string {
    {
        "color_picker",
        [](char const* label, json & value, json const& _schema) -> bool
        {
            std::string &json_string_ref = value.get_ref<std::string&>();
            auto _col = _hexStringToColor(json_string_ref);
            ImGui::PushID(&value);
            bool retVal=false;

            if(ImGui::ColorPicker4("", &_col.x,  0) )
            {
                uint32_t your_int = ImGui::GetColorU32(_col);
                std::stringstream stream;
                stream << std::setfill ('0') << std::setw(sizeof(uint32_t)*2)
                       << std::hex << your_int;
                value = stream.str();
                retVal = true;
            }
            ImGui::PopID();
            return retVal;
        }
    },
    {
        "color",
        [](char const* label, json & value, json const& _schema) -> bool
        {
            std::string &json_string_ref = value.get_ref<std::string&>();
            auto _col = _hexStringToColor(json_string_ref);
            ImGui::PushID(&value);
            bool retVal=false;

            if(ImGui::ColorEdit4("", &_col.x,  ImGuiColorEditFlags_NoInputs) )
            {
                uint32_t your_int = ImGui::GetColorU32(_col);
                std::stringstream stream;
                stream << std::setfill ('0') << std::setw(sizeof(uint32_t)*2)
                       << std::hex << your_int;
                value = stream.str();
                retVal = true;
            }
            ImGui::PopID();
            return retVal;
        }
    },
    {
        "textarea",
        [](char const* label, json & value, json const& schema) -> bool
        {
            std::string &json_string_ref = value.get_ref<std::string&>();
            auto options = schema.find("ui:options");
            int rows = 5;
            if(options != schema.end() && options->is_object())
            {
                rows = options->value("rows", 5);
            }

            auto sy = rows * ImGui::GetTextLineHeight();
            return ImGui::InputTextMultiline(label, &json_string_ref, {0,sy}, 0, nullptr, nullptr);
        }
    }
};




/**
 * @brief drawSchemaWidget_string
 * @param label
 * @param value
 * @param schema
 * @return
 *
 * Draw a string widget.
 *
 * Requirements:
 *   schema["type"] == "string"
 *
 * Optional:
 *   schema["enum"] == [ array of strings ]
 *   schema["enumNames] == [ array of strings ] - must be same length as schema["enum"]
 *
 * Do not use this directly
 */
inline bool drawSchemaWidget_string(char const * label, json & value, json const & schema, json & cache)
{
    std::string _label = schema.count("title") == 1 ? schema.at("title").get<std::string>() : std::string();
    json const * _default = (schema.count("default") == 1 && schema.at("default").is_string()) ? &schema.at("default") : nullptr;

    if(!value.is_string())
    {
        value = _getDefault(schema);
    }

    if(schema.contains("enum"))
    {
        return drawSchemaWidget_enum2(label, value, schema, cache);
    }

    auto widget_it = schema.find("ui:widget");
    std::string widget = schema.value("ui:widget" , "" );

    std::string& json_string_ref = value.get_ref<std::string&>();

    auto _widdraw_it = widgets_string.find( widget );
    if(_widdraw_it != widgets_string.end() && _widdraw_it->second)
    {
        return _widdraw_it->second(label, value, schema);
    }
    return ImGui::InputText(_label.empty() ? label : _label.c_str(), &json_string_ref, 0, nullptr, nullptr);
}


inline std::map<std::string, std::function<bool(char const*, json&, json const&)> > widgets_boolean {
    {
        "yesno",
        [](char const* label, json & value, json const& _schema) -> bool
        {
            const json _sch = {
                {"type", "string"},
                {"enum", {"No", "Yes"} }
            };
            bool& _v = value.get_ref<bool&>();
            json jval = _v ? _sch["enum"][1] : _sch["enum"][0];
            bool returnValue = drawSchemaWidget_enum("", jval, _sch);
            _v = jval == _sch["enum"][1];
            return returnValue;
        }
    },
    {
        "truefalse",
        [](char const* label, json & value, json const& _schema) -> bool
        {
            const json _sch = {
                {"type", "string"},
                {"enum", {"False", "True"} }
            };
            bool& _v = value.get_ref<bool&>();
            json jval = _v ? _sch["enum"][1] : _sch["enum"][0];
            bool returnValue = drawSchemaWidget_enum("", jval, _sch);
            _v = jval == _sch["enum"][1];
            return returnValue;
        }
    },
    {
        "enabledisable",
        [](char const* label, json & value, json const& _schema) -> bool
        {
            const json _sch = {
                {"type", "string"},
                {"enum", {"Disabled", "Enabled"} }
            };
            bool& _v = value.get_ref<bool&>();
            json jval = _v ? _sch["enum"][1] : _sch["enum"][0];
            bool returnValue = drawSchemaWidget_enum("", jval, _sch);
            _v = jval == _sch["enum"][1];
            return returnValue;
        }
    }
};


inline std::map<std::string, std::function<bool(char const*, json&, json const&)> > widgets_array {
     {
         "color_picker",
         [](char const* label, json & value, json const& schema) -> bool
         {
             auto minItems = schema.value("minItems"     , 0 );

             if(minItems == 3)
             {
                 if(!value.is_array())
                     value = {0.0f, 0.0f, 0.0f};

                 if( !value[0].is_number_float() ) value[0] = 0.0f;
                 if( !value[1].is_number_float() ) value[1] = 0.0f;
                 if( !value[2].is_number_float() ) value[2] = 0.0f;

                 std::array<float, 3> _col = {
                     value[0].get<float>(),
                     value[1].get<float>(),
                     value[2].get<float>()
                 };

                 if(ImGui::ColorPicker3(label, &_col[0], 0))
                 {
                     value[0] = _col[0];
                     value[1] = _col[1];
                     value[2] = _col[2];
                     return true;
                 }
             }
             if(minItems == 4)
             {
                 if( !value[0].is_number_float() ) value[0] = 0.0f;
                 if( !value[1].is_number_float() ) value[1] = 0.0f;
                 if( !value[2].is_number_float() ) value[2] = 0.0f;
                 if( !value[3].is_number_float() ) value[3] = 1.0f;
                 if(!value.is_array())
                     value = {0.0f, 0.0f, 0.0f, 1.0f};
                 std::array<float, 4> _col = {
                     value[0].get<float>(),
                     value[1].get<float>(),
                     value[2].get<float>(),
                     value[3].get<float>()
                 };

                 if(ImGui::ColorPicker4(label, &_col[0], 0))
                 {
                     value[0] = _col[0];
                     value[1] = _col[1];
                     value[2] = _col[2];
                     value[3] = _col[3];
                     return true;
                 }
             }

             return false;
         }
     },
    {
        "color",
        [](char const* label, json & value, json const& schema) -> bool
        {
             auto minItems = schema.value("minItems"     , 0 );

             if(minItems == 3)
             {
                 if(!value.is_array())
                     value = {0.0f, 0.0f, 0.0f};

                 if( !value[0].is_number_float() ) value[0] = 0.0f;
                 if( !value[1].is_number_float() ) value[1] = 0.0f;
                 if( !value[2].is_number_float() ) value[2] = 0.0f;

                 std::array<float, 3> _col = {
                     value[0].get<float>(),
                     value[1].get<float>(),
                     value[2].get<float>()
                 };

                 if(ImGui::ColorEdit3(label, &_col[0], ImGuiColorEditFlags_NoInputs))
                 {
                     value[0] = _col[0];
                     value[1] = _col[1];
                     value[2] = _col[2];
                     return true;
                 }
             }
             if(minItems == 4)
             {
                 if( !value[0].is_number_float() ) value[0] = 0.0f;
                 if( !value[1].is_number_float() ) value[1] = 0.0f;
                 if( !value[2].is_number_float() ) value[2] = 0.0f;
                 if( !value[3].is_number_float() ) value[3] = 1.0f;
                 if(!value.is_array())
                     value = {0.0f, 0.0f, 0.0f, 1.0f};
                 std::array<float, 4> _col = {
                     value[0].get<float>(),
                     value[1].get<float>(),
                     value[2].get<float>(),
                     value[3].get<float>()
                 };

                 if(ImGui::ColorEdit4(label, &_col[0], ImGuiColorEditFlags_NoInputs))
                 {
                     value[0] = _col[0];
                     value[1] = _col[1];
                     value[2] = _col[2];
                     value[3] = _col[3];
                     return true;
                 }
             }

             return false;
        }
    }
};

inline bool drawSchemaWidget_boolean(char const * label, json & value, json const & schema, json & cache)
{
    if(!value.is_boolean())
        value = false;

    std::string widget = schema.value("ui:widget" , "" );

    bool returnValue = false;
    bool& _v = value.get_ref<bool&>();

    auto _widdraw_it = widgets_boolean.find(widget);
    if(_widdraw_it != widgets_boolean.end() && _widdraw_it->second)
    {
        _widdraw_it->second(label, value, schema);
    }
    else
    {
        ImGui::PushID(&value);
        if(ImGui::Checkbox("", &_v))
        {
            returnValue |= true;
        }
        ImGui::PopID();

    }
    return returnValue;
}


inline bool drawSchemaArray(char const *label, json & value, json const & schema, json & cache)
{
    // auto type_it = schema.find("type");
    auto item_it = schema.find("items");

    if( item_it == schema.end())
        return false;

    //  auto & _type  = *type_it;//schema.at("type");
    auto & _items = *item_it;//schema.at("items");

    std::string widget = schema.value("ui:widget" , "" );

    auto wid_it = widgets_array.find(widget);

    if(!value.is_array())
        value = json::array_t();

    // if(_type == "array")
    {
        auto minItems = schema.value("minItems"     , 0 );
        auto maxItems = schema.value("maxItems"     , value.size()+1 );
        bool re = false;

        if(wid_it != widgets_array.end() && wid_it->second)
        {
            ImGui::PushID(&value);
            re |= wid_it->second(label, value, schema);
            ImGui::PopID();
        }
        else
        {
            ImGui::PushID(&value);
            auto itemCount = value.size();

            while(value.size() < minItems)
                value.push_back( _getDefault(_items));

            auto full_width = ImGui::GetContentRegionAvail().x;

            auto sz = ImGui::GetFrameHeight();
            ImVec2 buttonSize(sz,sz);

            auto spacing = ImGui::GetStyle().ItemSpacing.x;
            auto padding = ImGui::GetStyle().FramePadding.x;
            auto width = full_width - 3 * buttonSize.x - 3*spacing - padding;

            bool showButtons = value.size() > minItems;

            if(!showButtons)
                width = full_width;
            ImGui::BeginTable("arraytable", showButtons ? 2 : 1);
            ImGui::TableSetupColumn("AAA", ImGuiTableColumnFlags_WidthStretch);
            if(showButtons)ImGui::TableSetupColumn("BBB", ImGuiTableColumnFlags_WidthFixed, full_width-width);

            for(int i=0;i<itemCount;i++)
            {
                ImGui::PushID(i);
                ImGui::TableNextColumn();

                ImGui::SetNextItemWidth(width );
                ImGui::PushItemWidth(-1);
                re |= drawSchemaWidget("", value[i], _items, cache);
                ImGui::PopItemWidth();
                if(showButtons)
                {

                    ImGui::TableNextColumn();
                    if(ImGui::Button("x", buttonSize))
                    {
                        value.erase(i);
                        itemCount--;
                        re |= true;
                    }
                    if(ImGui::IsItemHovered())
                    {
                        ImGui::SetTooltip("Delete this item");
                    }
                    ImGui::SameLine();
                    if( ImGui::ArrowButton("MoveUp", ImGuiDir_Up) )
                    {
                        if(i!=0)
                            std::swap(value.at(i), value.at(i-1));
                        re |= true;
                    }
                    if(ImGui::IsItemHovered())
                    {
                        ImGui::SetTooltip("Move the item up the list");
                    }
                    ImGui::SameLine();
                    if(ImGui::ArrowButton("MoveDown", ImGuiDir_Down))
                    {
                        if(i!=itemCount-1)
                            std::swap(value.at(i), value.at(i+1));
                        re |= true;
                    }
                    if(ImGui::IsItemHovered())
                    {
                        ImGui::SetTooltip("Move the item down the list");
                    }
                }
                ImGui::PopID();
            }

            ImGui::EndTable();

            if(value.size() < maxItems)
            {
                if(ImGui::Button("+", {full_width, 0}))
                {
                    value.push_back( _getDefault(_items) );
                    re |= true;
                }
                if(ImGui::IsItemHovered())
                {
                    ImGui::SetTooltip("Append a new item to the array");
                }
            }
            ImGui::PopID();

            return re;
        }

        return re;
    }

    return false;
}


inline bool drawSchemaWidget(char const *label, json & propertyValue, json const & propertySchema, json & cache)
{
    auto type_it = propertySchema.find("type");

    if(type_it == propertySchema.end() )
        return false;

    auto & type  = *type_it;

    bool returnValue = false;

    ImGui::PushItemWidth(-1);
    if(type == "string")
    {
        if(drawSchemaWidget_string(label, propertyValue, propertySchema, cache))
        {
            returnValue |= true;
        }
    }
    else if(type == "number" || type == "integer")
    {
        returnValue |= drawSchemaWidget_Number(label, propertyValue, propertySchema, cache);
    }
    else if(type == "array")
    {
        returnValue |= drawSchemaArray(label, propertyValue, propertySchema,cache);
    }
    else if(type == "boolean")
    {
        ImGui::PushID(&propertyValue);
        returnValue |= drawSchemaWidget_boolean(label, propertyValue, propertySchema, cache);
        ImGui::PopID();
    }
    else if(type == "object")
    {
        ImGui::PushID(&propertyValue);
        returnValue |= drawSchemaWidget_Object(label, propertyValue, propertySchema, cache);
        ImGui::PopID();
    }
    ImGui::PopItemWidth();
    return returnValue;
}



/**
 * @brief drawSchemaWidget_Object
 * @param label
 * @param objectValue
 * @param schema
 * @return
 *
 * Draw a json object.
 */
inline bool drawSchemaWidget_Object(char const * label, json & objectValue, json const & schema, json & cache)
{
    if(!schema.is_object())
        return false;

    if(!cache.is_object())
        cache = json::object_t();

    auto properties_it = schema.find("properties");

    if(properties_it == schema.end() )
        return false;

    auto & properties  = *properties_it;

    bool returnValue = false;
    json const * order = nullptr;
    {
        auto count_it = schema.find("ui:order");
        if(count_it != schema.end())
        {
            order = &(*count_it);
        }
    }

    auto tableFlags = ImGuiTableFlags_SizingStretchSame | ImGuiTableFlags_Resizable;
    auto C1Flags = ImGuiTableColumnFlags_WidthStretch;
    auto C2Flags = ImGuiTableColumnFlags_WidthStretch;
    auto C1Width = 25;
    auto C2Width = 75;

    if(!objectValue.is_object())
        objectValue = json::object_t();

    if(order)
    {
        if (ImGui::BeginTable("split", 2, tableFlags))
        {
            ImGui::TableSetupColumn("AAA", C1Flags, C1Width);
            ImGui::TableSetupColumn("BBB", C2Flags, C2Width);

            for(auto & _ord : *order)
            {
                auto propertyName = _ord.get<std::string>();
                auto propertySchema_it = properties.find(propertyName);
                if(propertySchema_it == properties.end())
                    continue;
                auto & propertySchema = *propertySchema_it;
                auto & propertyValue = objectValue[propertyName];


                std::string const * propertyTitle = &propertyName;
                auto title_it = propertySchema.find("title");
                if(title_it != propertySchema.end() && title_it->is_string())
                {
                    propertyTitle = &title_it->get_ref<std::string const&>();
                }


                ImGui::TableNextColumn();
                ImGui::Text("%s", propertyName.c_str());
                ImGui::TableNextColumn();
                ImGui::PushItemWidth(-1);
                returnValue |= drawSchemaWidget(propertyName.c_str(), propertyValue, propertySchema, cache[propertyName]);
                ImGui::PopItemWidth();
            }

            ImGui::EndTable();
        }
    }
    else
    {
        if (ImGui::BeginTable("split", 2, tableFlags))
        {
            ImGui::TableSetupColumn("AAA", C1Flags, C1Width);
            ImGui::TableSetupColumn("BBB", C2Flags, C2Width);

            for(auto & [propertyName, propertySchema] : properties.items())
            {
                auto & propertyValue = objectValue[propertyName];

                std::string const * propertyTitle = &propertyName;
                auto title_it = propertySchema.find("title");
                if(title_it != propertySchema.end() && title_it->is_string())
                {
                    propertyTitle = &title_it->get_ref<std::string const&>();
                }

                ImGui::TableNextColumn();
                ImGui::Text("%s", propertyTitle->c_str());
                {
                    std::string const * help = nullptr;
                    if(propertyValue.count("ui:help") && propertyValue.at("ui:help").is_string())
                    {
                        help = &propertyValue.at("ui:help").get_ref<std::string&>();
                    }
                    if(help && ImGui::IsItemHovered())
                    {
                        ImGui::SetTooltip("%s", help->c_str());
                    }
                }
                ImGui::TableNextColumn();

                ImGui::PushItemWidth(-1);
                returnValue |= drawSchemaWidget(propertyName.c_str(), propertyValue, propertySchema, cache[propertyName]);
                ImGui::PopItemWidth();
            }
            ImGui::EndTable();
        }
    }
    return returnValue;
}



}

#endif

