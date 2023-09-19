#ifndef SNE_IMGUI_JSON_UI_H
#define SNE_IMGUI_JSON_UI_H

#define IMGUI_DEFINE_MATH_OPERATORS
#include "imgui.h"

#include <nlohmann/json.hpp>
#include <imgui_stdlib.h>
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
 *    std::cout << getModifiedWidgetPath() << std::endl;
 * }
 *
 * ImGui::End();
 *
 */
bool drawSchemaWidget(char const *label, json & propertyValue, json const & propertySchema, json &cache, float object_width = 0.0f);
/**
 * @brief getModifiedWidgetPath
 * @return
 *
 * When drawSchemaWidget(...) returns true
 * you can use this function to get the path within
 * the json object that was modified
 */
std::string const& getModifiedWidgetPath();

/**
 * @brief drawSchemaWidget_Number
 * @param label
 * @param value
 * @param schema
 * @param cache
 * @return
 *
 * Draws a number widget:
 * Requires:
 *    schema.type == "number" or "integer"
 *
 * Optional:
 *    schema.default = float or integer - the default value
 *    schema.minimum = float or integer
 *    schema.maximum = float or integer
 *    schema.ui:widget = "slider" or "drag" - slider requires minimum and maximum to be defined
 *    schema.ui:step = float - speed at which the drag slider moves
 *    schema.ui:step_fast - float - speed at which the drag slider moves when shift is pressed
 *                                  default is 10x ui:step
 */
bool drawSchemaWidget_Number(char const *label, json & value, json const & schema,json & cache);

/**
 * @brief drawSchemaWidget_string
 * @param label
 * @param value
 * @param schema
 * @param cache
 * @return
 *
 * Draws a string widget. Requires: schema.type == "string"
 *
 * Requres:
 *   schema.type == "string"
 *
 * Optional:
 *   schema.ui:widget == "color" - draws the schema as a color picker
 */
bool drawSchemaWidget_string(char const * label, json & value, json const & schema, json & cache);

/**
 * @brief drawSchemaWidget_boolean
 * @param label
 * @param value
 * @param schema
 * @param cache
 * @return
 *
 * Draws a boolean widget:
 *
 *  Requires:
 *       schema.type == "boolean
 *
 */
bool drawSchemaWidget_boolean(char const * label, json & value, json const & schema, json & cache);

/**
 * @brief drawSchemaArray
 * @param label
 * @param value
 * @param schema
 * @param cache
 * @return
 *
 * draws an array widget, requires:
 *   schema.type == "array"
 *   schema.items == schema_type
 *
 * Optional
 *   schema.minItems = integer
 *   schema.maxItems = integer
 */
bool drawSchemaArray(char const *label, json & value, json const & schema, json & cache);

/**
 * @brief drawSchemaWidget_Object
 * @param label
 * @param objectValue
 * @param schema
 * @param cache
 * @param widget_size
 * @return
 *
 * Draws a object widget.
 *
 * Requires:
 *   schema.type == "object"
 *   schema.properties = object containing key/schema pairs
 *
 * Optional:
 *   schema.ui:order = array of keys specifying order the properties should be drawn in
 *   schema.ui:widget == "header" - draw object with a Collapsable Header
 */
bool drawSchemaWidget_Object(char const * label, json & objectValue, json const & schema, json & cache, float widget_size);

/**
 * @brief toggleButton
 * @param label
 * @param value
 * @param btnSize
 * @return
 *
 * A toggle button widget that acts like a checkbox. Works similar to the
 * regular ImGui::CheckBox,  if btnSize == {0,x}, the button will span
 * the available width
 */
inline bool toggleButton(char const *label, bool *value, ImVec2 btnSize = {0,0})
{
    bool retValue = false;
    bool &_enable = *value;

    if(btnSize.x <= 0.0f)
        btnSize.x = ImGui::GetContentRegionAvail().x;

    auto buttonCol = ImGui::GetStyle().Colors[ImGuiCol_Button];
    auto buttonActiveCol = ImGui::GetStyle().Colors[ImGuiCol_ButtonActive];

    {
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, _enable ? buttonActiveCol : buttonCol);
        ImGui::PushStyleColor(ImGuiCol_Button       , _enable ? buttonActiveCol : buttonCol);

        if(ImGui::Button(label, btnSize))
        {
            _enable = !_enable;
            retValue = true;
        }
        ImGui::PopStyleColor(2);
    }
    return retValue;
}

/**
 * @brief falseTrueButton
 * @param no
 * @param yes
 * @param value
 * @param btnSize
 * @return
 *
 * Similar to a checkbox but but provides two buttons to choose from.
 *
 */
inline bool falseTrueButton(char const* no, char const * yes, bool * value, ImVec2 btnSize = {0,0})
{
    bool retValue = false;
    bool &_enable = *value;

    ImJSchema::json J1, J2;

    if(btnSize.x <= 0.0f)
        btnSize.x = (ImGui::GetContentRegionAvail().x - ImGui::GetStyle().ItemSpacing.x) / 2;

    auto buttonCol = ImGui::GetStyle().Colors[ImGuiCol_Button];
    auto buttonActiveCol = ImGui::GetStyle().Colors[ImGuiCol_ButtonActive];

    bool no_value = !*value;

    if(toggleButton(no, &no_value, {btnSize.x, 0}))
    {
        if(no_value == true)
            *value = false;
        retValue = true;
    }
    ImGui::SameLine();
    if(toggleButton(yes, value, {btnSize.x, 0}))
    {
        retValue = true;
    }
    ImGui::SameLine();

    return retValue;
}


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


inline bool HeaderText(char const *label)
{
    ImGui::SeparatorText(label);
    return true;
}

inline bool SeparatorLine()
{
    ImGui::PushStyleVar(ImGuiStyleVar_SeparatorTextBorderSize, 0.1f);
    ImGui::SeparatorText("");
    ImGui::PopStyleVar(1);
    return true;
}

bool drawSchemaWidget_internal(char const *label, json & propertyValue, json const & propertySchema, json &cache, float object_width = 0.0f);

/**
 * @brief drawSchemaWidget_Object
 * @param label
 * @param objectValue
 * @param schema
 * @return
 *
 * This is the main function you should be using to draw an object.
 */
bool drawSchemaWidget_Object(char const * label, json & objectValue, json const & schema, json &cache, float widget_size=0.0f);

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
        if(it == schema.end() || !it->is_number() )
            return 0.0;
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

bool _nodeWidgetModified = false;
std::string _path_str;

inline void _pushName(std::string const & name)
{
    if(!_nodeWidgetModified)
    {
        _path_str += '/';
        _path_str += name;
    }
}

inline void _popName()
{
    if(!_nodeWidgetModified)
    {
        auto i = _path_str.find_last_of('/');
        if(i >= 0 && i != std::string::npos )
        {
            _path_str.resize(i);
        }
    }
}


template<typename T>
inline bool _dragWidget_t(char const* label, json & value, json const& _schema)
{
    static_assert( std::is_same_v<double, T> || std::is_same_v<int64_t, T>);

    auto _speed  = _schema.value("ui:speed", 1.0f );

    {
        using value_type = T;
        value_type & _value = value.get_ref<value_type&>();
        value_type minimum = _schema.value("minimum", std::numeric_limits<value_type>::lowest() );
        value_type maximum = _schema.value("maximum", std::numeric_limits<value_type>::max() );
        bool retValue = false;
        if(DragScalar_T<value_type>(label, &_value, _speed, minimum, maximum))
        {
            _value = std::clamp(_value, minimum, maximum);
            value = _value;
            retValue = true;
        }
        return retValue;
    }
    return false;
}


inline bool _dragWidget(char const* label, json & value, json const& _schema)
{
    auto _speed  = _schema.value("ui:speed", 1.0f );
    if(value.is_number_float())
    {
        return _dragWidget_t<double>(label, value, _schema);
    }
    else if(value.is_number_integer())
    {
        return _dragWidget_t<int64_t>(label, value, _schema);
    }
    return false;
}

inline std::map<std::string, std::function<bool(char const*, json&, json const&)> > widgets_numbers {
    {
        "slider",
        [](char const* label, json & value, json const& _schema) -> bool
        {
            if(value.is_number_float())
            {
                double & _value = value.get_ref<double&>();
                double minimum = _schema.value("minimum", std::numeric_limits<double>::max());
                double maximum = _schema.value("maximum", std::numeric_limits<double>::max());

                if(minimum < std::numeric_limits<double>::max() && maximum < std::numeric_limits<double>::max() )
                {
                    if(SliderScalar_T<double>(label, &_value, minimum, maximum))
                    {
                        _value = std::clamp(_value, minimum, maximum);
                        value = _value;
                        return true;
                    }
                }
                else
                {
                    return _dragWidget(label, value, _schema);
                }
            }
            else if(value.is_number_integer())
            {
                int64_t & _value = value.get_ref<int64_t&>();
                int64_t minimum = _schema.value("minimum", std::numeric_limits<int64_t>::max());
                int64_t maximum = _schema.value("maximum", std::numeric_limits<int64_t>::max());

                if(minimum < std::numeric_limits<int64_t>::max() && maximum < std::numeric_limits<int64_t>::max() )
                {
                    if(SliderScalar_T<int64_t>(label, &_value, minimum, maximum))
                    {
                        _value = std::clamp(_value, minimum, maximum);
                        value = _value;
                        return true;
                    }
                }
                else
                {
                    return _dragWidget(label, value, _schema);
                }
            }
            return false;
        }
    },
    {
        "drag",

        [](char const* label, json & value, json const& _schema) -> bool
        {
            return _dragWidget(label, value, _schema);
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


    bool retValue = false;
    auto widget_it = schema.find("ui:widget");
    if(widget_it != schema.end() && widget_it->is_string())
    {
        auto _widdraw_it = widgets_numbers.find( *widget_it);
        if(_widdraw_it != widgets_numbers.end() && _widdraw_it->second)
        {
            retValue = _widdraw_it->second(label, value, schema);
        }
    }
    else
    {
        if( *it == "number" )
        {
            if(!value.is_number_float())
                value = value.get<double>();

            auto step      = schema.value("ui:step"     , std::numeric_limits<double>::max() );
            auto step_fast = schema.value("ui:step_fast", std::numeric_limits<double>::max() );
            double & _value = value.get_ref<double&>();
            retValue =  InputScalar_T<double>(label,&_value, step, step_fast);

            auto it_min = schema.find("minimum");
            auto it_max = schema.find("maximum");
            if(it_min != schema.end() && it_min->is_number())
                value = std::max( _value, it_min->get<double>());
            if(it_max != schema.end() && it_max->is_number())
                value = std::min( _value, it_max->get<double>());
        }
        if( *it == "integer" )
        {
            if(!value.is_number_integer())
                value = value.get<int64_t>();

            auto step      = schema.value("ui:step"     , std::numeric_limits<int64_t>::max() );
            auto step_fast = schema.value("ui:step_fast", std::numeric_limits<int64_t>::max() );
            int64_t & _value = value.get_ref<int64_t&>();
            retValue = InputScalar_T<int64_t>(label,&_value, step, step_fast);

            auto it_min = schema.find("minimum");
            auto it_max = schema.find("maximum");
            if(it_min != schema.end() && it_min->is_number())
                value = std::max( _value, it_min->get<int64_t>());
            if(it_max != schema.end() && it_max->is_number())
                value = std::min( _value, it_max->get<int64_t>());
        }
    }



    return retValue;
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

    auto _enum      = schema.find("enum");
    auto _enumNames = schema.find("enumNames");

    // enumNames does not exist. does
    if(_enumNames == schema.end() || !_enumNames->is_array() )
    {
        _enumNames = _enum;
    }

    if(!_cache.is_object())
        _cache = json::object_t();

    uint32_t index = _cache.value("enumIndex", uint32_t(0xFFFFFFFF) );

    if(index == 0xFFFFFFFF)
    {
        uint32_t c = 0;
        // this is the first time we're drawing the widgetg
        value = _getDefault(schema);
        bool found = false;
        for(auto & _e : *_enum)
        {
            if(value == _e)
            {
                found = true;
                index = c;
                found = true;
                break;
            }
            ++c;
        }
        if(!found)
        {
            value = _enum->front();
            index = 0;
        }
    }
    index = std::min<uint32_t>(index, _enum->size()-1 );

    std::string _tmpName;
    // Gets the appropriate label that should be visible
    // in the dropdown menu
    auto _getName = [&_tmpName, &_enumNames](size_t i) -> std::string const&
    {
        auto & name = _enumNames->at(i);
        if(name.is_string())
        {
            return name.get_ref<std::string const&>();
        }
        else if(name.is_number() || name.is_boolean())
        {
            _tmpName = name.dump();
        }
        else if(name.is_array())
        {
            _tmpName = "array_" + std::to_string(i);
        }
        else if(name.is_object())
        {
            _tmpName = "object_" + std::to_string(i);
        }
        else
        {
            _tmpName = "unknown";
        }
        return _tmpName;
    };

    std::string const &  value_str = _getName(index);

    uint32_t totalEnums = static_cast<uint32_t>(std::min(_enum->size(), _enumNames->size()));

    auto wid_it = schema.find("ui:widget");
    if(wid_it != schema.end() && wid_it->is_string())
    {
        if(*wid_it == "button")
        {
            uint32_t itemsPerRow = 3;
            uint32_t itemInRowCount = 0;

            auto opt_it = schema.find("ui:options");
            if(opt_it != schema.end() && opt_it->is_object())
            {
                itemsPerRow = opt_it->value("columns", itemsPerRow);
            }

            auto w = (ImGui::GetContentRegionAvail().x- ImGui::GetStyle().ItemSpacing.x * (itemsPerRow-1)) / itemsPerRow;

            for(uint32_t i=0; i < totalEnums; i++)
            {
                std::string const & label = _getName(i);//_enumNames->at(i).get_ref<std::string const&>();

                bool is_selected = index == i;
                ImGui::PushID(&label);

                auto prevValue = is_selected;
                if(toggleButton(label.c_str(), &is_selected, {w,0}))
                {
                    // we pressed the toggle button
                    // did we go from off->on state
                    if( !prevValue && is_selected)
                    {
                        value = _enum->at(i);
                        return_value = true;
                        _cache["enumIndex"] = i;
                    }
                }
                itemInRowCount++;
                if(itemInRowCount >= itemsPerRow)
                {
                    itemInRowCount = 0;
                }
                else
                {
                    ImGui::SameLine();
                }
                ImGui::PopID();
            }
        }
    }
    else
    {
        if(ImGui::BeginCombo( _label.empty() ? label : _label.c_str(), value_str.c_str(), _flags))
        {
            for(uint32_t i=0; i < totalEnums; i++)
            {
                std::string const & label = _getName(i);

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

            if(ImGui::ColorEdit4("", &_col.x) )
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

    auto t = ImGui::InputText(_label.empty() ? label : _label.c_str(), &json_string_ref, 0, nullptr, nullptr);
    return t;
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

                 if(ImGui::ColorEdit3(label, &_col[0]))
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

                 if(ImGui::ColorEdit4(label, &_col[0]))
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
        returnValue |= _widdraw_it->second(label, value, schema);
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

            if(!cache.is_array())
                cache = json::array_t();

            bool drawLine = false;
            if(_items.at("type") == "object")
                drawLine = true;
            for(int i=0;i<itemCount;i++)
            {
                _pushName(std::to_string(i));

                ImGui::PushID(i);
                ImGui::TableNextColumn();

                ImGui::SetNextItemWidth(width );
                ImGui::PushItemWidth(-1);
                re |= drawSchemaWidget_internal("", value[i], _items, cache[i]);
                ImGui::PopItemWidth();
                if(drawLine && i != itemCount-1)
                    SeparatorLine();
                if(showButtons)
                {
                    ImGui::TableNextColumn();
                    if(ImGui::Button("x", buttonSize))
                    {
                        value.erase(i);
                        cache.erase(i);
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
                        {
                            std::swap(value.at(i), value.at(i-1));
                            std::swap(cache.at(i), cache.at(i-1));
                        }
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
                        {
                            std::swap(value.at(i), value.at(i+1));
                            std::swap(cache.at(i), cache.at(i+1));
                        }
                        re |= true;
                    }
                    if(ImGui::IsItemHovered())
                    {
                        ImGui::SetTooltip("Move the item down the list");
                    }
                }
                ImGui::PopID();

                _popName();
            }

            ImGui::EndTable();

            if(value.size() < maxItems)
            {
                //SeparatorText("");
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

inline bool drawSchemaWidget(char const *label, json & propertyValue, json const & propertySchema, json & cache, float object_width)
{
    _nodeWidgetModified = false;
    _path_str.clear();
    return drawSchemaWidget_internal(label, propertyValue, propertySchema, cache, object_width);
}

inline bool drawSchemaWidget_internal(char const *label, json & propertyValue, json const & propertySchema, json & cache, float object_width)
{
    bool returnValue = false;

    doIfKeyExists("type", propertySchema, [&returnValue, &propertyValue, label, &propertySchema, &cache, &object_width](auto & type)
    {
        _pushName(label);

        ImGui::PushItemWidth(-1);

        if(propertySchema.contains("enum"))
        {
            returnValue |= drawSchemaWidget_enum2(label, propertyValue, propertySchema, cache);
        }
        else
        {
            if(type == "string")
            {
                returnValue |= drawSchemaWidget_string(label, propertyValue, propertySchema, cache);
            }
            else if(type == "number" || type == "integer")
            {
                returnValue |= drawSchemaWidget_Number(label, propertyValue, propertySchema, cache);
            }
            else if(type == "boolean")
            {
                ImGui::PushID(&propertyValue);
                returnValue |= drawSchemaWidget_boolean(label, propertyValue, propertySchema, cache);
                ImGui::PopID();
            }
        }

        // enum, string, boolean, numbers should ahve their descriptions
        // after the widget. Objects and arrays should have it before
        bool _hasDescription = false;
        doIfKeyExists("description", propertySchema, [&_hasDescription](auto & S)
                      {
                          ImGui::TextWrapped("%s", S.template get_ref<std::string const&>().c_str());
                         _hasDescription = true;
                      });

        if(returnValue)
        {
            _nodeWidgetModified = true;
            ImGui::PopItemWidth();
            return;
        }

        if(type == "array")
        {
            if(_hasDescription) SeparatorLine();
            returnValue |= drawSchemaArray(label, propertyValue, propertySchema,cache);
            if(returnValue)
            {
                _nodeWidgetModified = true;
            }
        }
        else if(type == "object")
        {
            ImGui::PushID(&propertyValue);
            if(_hasDescription) SeparatorLine();
            returnValue |= drawSchemaWidget_Object(label, propertyValue, propertySchema, cache, object_width);
            if(returnValue)
            {
                _nodeWidgetModified = true;
            }
            ImGui::PopID();
        }
        _popName();
        ImGui::PopItemWidth();

    });

    return returnValue;
}


inline void setDefaultIfNeeded(json & value, json const &schema)
{
    if(value.is_null())
        value = _getDefault(schema);

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
inline bool drawSchemaWidget_Object(char const * label, json & objectValue, json const & schema, json & cache, float widget_size)
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
    if(!objectValue.is_object())
        objectValue = json::object_t();


    auto column_size   = JValue(schema, "ui:column_size", 0.0f);
    auto column_resize = JValue(schema, "ui:column_resizable", false);
    column_size = std::clamp(column_size, 0.0f, 100.0f);

    auto tableFlags = ImGuiTableFlags_SizingStretchSame | ImGuiTableFlags_SizingStretchSame;
    auto C1Flags = ImGuiTableColumnFlags_WidthStretch;
    auto C2Flags = ImGuiTableColumnFlags_WidthStretch;

    float availWidth = ImGui::GetContentRegionAvail().x;
    float textSize = 0.0f;

    auto C1Width = 25;
    auto C2Width = 75;

    availWidth = widget_size > 0.0f ? widget_size : availWidth;
    C1Flags    = ImGuiTableColumnFlags_WidthFixed;
    C2Flags    = ImGuiTableColumnFlags_WidthStretch;
    tableFlags = ImGuiTableFlags_SizingFixedSame;
    C1Width    = availWidth / 2;
    C2Width    = availWidth / 2;

    if(cache.count("label_size"))
    {
        C1Width = cache.at("label_size").get<float>();
    }

    if(column_resize)
        tableFlags |= ImGuiTableFlags_Resizable;

    auto _drawProperty = [&](std::string const & propertyName,
                             std::string const & propertyTitle,
                            json const & propertySchema,
                            json & propertyValue)
    {
        setDefaultIfNeeded(propertyValue, propertySchema);
        auto isHidden   = JValue(propertySchema, "ui:hidden", false);
        auto isDisabled = JValue(propertySchema, "ui:disabled", false);

        if(isHidden)
            return;

        ImGui::BeginDisabled(isDisabled);

            textSize = std::max(textSize, ImGui::CalcTextSize(propertyTitle.c_str()).x) + 5;
            cache["label_size"] = textSize;

            ImGui::TableNextColumn();
            ImGui::Text("%s", propertyTitle.c_str());
            {
                doIfKeyExists("ui:help", propertySchema, [](auto & H)
                {
                    if(H.is_string() && ImGui::IsItemHovered(ImGuiHoveredFlags_DelayNormal))
                    {
                        ImGui::SetTooltip("%s", H.template get_ref<std::string const&>().c_str());
                    }
                });
            }
            ImGui::TableNextColumn();

            ImGui::PushItemWidth(-1);
                returnValue |= drawSchemaWidget_internal(propertyName.c_str(), propertyValue, propertySchema, cache[propertyName]);
            ImGui::PopItemWidth();

        ImGui::EndDisabled();
    };

    bool _tableStarted=false;
    auto _beginTable = [=, &_tableStarted]()
    {
        if(_tableStarted)
            return;
        ImGui::BeginTable("split", 2, tableFlags, {availWidth,0.0f});
        ImGui::TableSetupColumn("AAA", C1Flags, C1Width);
        ImGui::TableSetupColumn("BBB", C2Flags, C2Width);
        _tableStarted = true;
    };

    auto _endTable = [=, &_tableStarted]()
    {
        if(!_tableStarted)
            return;
        ImGui::EndTable();
        _tableStarted = false;
    };

    // returns the schema's "title" property or propName if it doesn't exist
    auto _getVisibleTitle = [](json const & schema, std::string const & propName) -> std::string const&
    {
        auto it = schema.find("title");
        if(it != schema.end() && it->is_string())
            return it->get_ref<std::string const&>();
        return propName;
    };


    auto _forEachProperty = [&](auto && lambda)
    {
        if(order)
        {
            for(auto & _ord : *order)
            {
                auto propertyName = _ord.get<std::string>();
                auto propertySchema_it = properties.find(propertyName);

                if(propertySchema_it == properties.end())
                    continue;

                auto & propertySchema = *propertySchema_it;
                auto & propertyValue = objectValue[propertyName];

                lambda(propertyName, propertySchema);
            }
        }
        else
        {
            for(auto & [propertyName, propertySchema] : properties.items())
            {
                lambda(propertyName, propertySchema);
            }
        }
    };

    _forEachProperty([&](std::string const & propertyName, json const & propertySchema)
    {
        auto & propertyValue = objectValue[propertyName];
        auto const & title = _getVisibleTitle(propertySchema, propertyName);

        doIfKeyExists("type", propertySchema, [&](auto & type)
        {
            bool drawObjectAsHeader = false;
            bool collapsing = false;
            doIfKeyExists("ui:widget", propertySchema, [&drawObjectAsHeader,&collapsing](auto & widget)
                          {
                                if(widget == "header")
                                {
                                    drawObjectAsHeader = true;
                                    collapsing = false;
                                }
                                else if(widget == "collapsing")
                                {
                                    drawObjectAsHeader = true;
                                    collapsing = true;
                                }
                          });

            if(drawObjectAsHeader && (type == "object" || type == "array") )
            {
                _endTable();
                ImGui::PushID(&title);

                //bool _doheader = ImGui::CollapsingHeader(title.c_str(), ImGuiTreeNodeFlags_DefaultOpen );
                bool _doheader = collapsing ?  ImGui::CollapsingHeader(title.c_str(), ImGuiTreeNodeFlags_DefaultOpen )
                               : HeaderText(title.c_str());

                //bool _doheader = true;
                doIfKeyExists("ui:help", propertySchema, [](auto & help)
                              {
                                  if(help.is_string() && ImGui::IsItemHovered(ImGuiHoveredFlags_DelayNormal) )
                                  {
                                      ImGui::SetTooltip("%s", help.template get_ref<std::string const&>().c_str());
                                  }
                              });
                if(_doheader)
                {
                    ImGui::Dummy({10,10});
                    ImGui::SameLine();
                    ImGui::BeginGroup();
                    ImGui::PushItemWidth(-1);

                    returnValue |= drawSchemaWidget_internal(propertyName.c_str(), propertyValue, propertySchema, cache[propertyName]);

                    ImGui::PopItemWidth();
                    ImGui::EndGroup();
                }

                ImGui::PopID();
            }
            else
            {
                _beginTable();
                _drawProperty(propertyName, title, propertySchema, propertyValue);
            }
        });
    });
     _endTable();

    return returnValue;
}




inline std::string const& getModifiedWidgetPath()
{
    return _path_str;
}

}

#endif

