/**
 *
 * MIT License
 *
 * Copyright (c) 2023 GavinNL
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */
#ifndef IMJSCHEMA_H
#define IMJSCHEMA_H

#define IMJSCHEMA_VERSION_MAJOR 0
#define IMJSCHEMA_VERSION_MINOR 3

#include <imgui.h>
#include <imgui_stdlib.h>
#include <imgui_internal.h>

#include "detail/imgui_widgets_t.h"
#include "detail/json_utils.h"

#include <sstream>
#include <unordered_set>
#include <iomanip>

namespace ImJSchema
{

using json = nlohmann::json;

/**
 * @brief The WidgetDrawInput class
 *
 * A struct that contains references to various values
 *
 */
struct WidgetDrawInput
{
    char const * label;         // The label that will be printed
    json & value;               // Reference to the json object where the
                                //    final value will be stored
    json const & schema;        // Reference to the schema that describes
                                //    what the object is
    json & cache;               // Reference to json object that can be used
                                //    to cache intermediate data
    float object_width = 0.0f;  // The how wide to draw the object
};
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
 * WidgetDrawInput in = {"label", value, schema, cache};
 * if( drawSchemaWidget(in) )
 * {
 *    std::cout << getModifiedWidgetPath() << std::endl;
 * }
 *
 * ImGui::End();
 *
 */
bool drawSchemaWidget(WidgetDrawInput & in);

/**
 * @brief getModifiedWidgetPath
 * @return
 *
 * When drawSchemaWidget(...) returns true
 * you can use this function to get the path within
 * the json object that was modified
 */
json::json_pointer getModifiedWidgetPath();


// detail namespace, used internally
namespace detail {
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


using widget_draw_function_type = std::function<bool(WidgetDrawInput &in) >;

//#define IMJSCHEMA_UNUSED (void)_label; (void)_value; (void)_schema; (void)in.cache; (void)_object_width;
#define IMJSCHEMA_UNUSED

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

bool drawSchemaWidget_Array(char const *label, json & value, json const & schema, json &cache, float object_width = 0.0f);



inline bool _nodeWidgetModified = false;
inline json::json_pointer _path_ptr;

inline void _pushName(std::string const & name)
{
    if(!_nodeWidgetModified)
    {
        _path_ptr.push_back(name);
    }
}

inline void _popName()
{
    if(!_nodeWidgetModified)
    {
        _path_ptr.pop_back();
    }
}

inline bool drawSchemaWidget_enum(char const * label, json & value, json const & schema, json & cache)
{
    bool return_value = false;
    ImGuiComboFlags _flags = 0;
    std::string _label = schema.count("title") == 1 ? schema.at("title").get<std::string>() : std::string();

    auto _enum      = schema.find("enum");
    auto _enumNames = schema.find("enumNames");

    if(_enum == schema.end())
    {
        throw std::runtime_error(" \"enum\" property does not exist");
    }
    if(!_enum->is_array())
    {
        throw std::runtime_error(" \"enum\" property is not an array");
    }
    if(_enum->size() == 0)
    {
        throw std::runtime_error(" \"enum\" array is zero size");
    }
    // enumNames does not exist. does
    if(_enumNames == schema.end() || !_enumNames->is_array() )
    {
        _enumNames = _enum;
    }

    if(!cache.is_object())
        cache = json::object_t();

    uint32_t index = cache.value<uint32_t>("enumIndex", 0xFFFFFFFF );

    if(index == 0xFFFFFFFF)
    {
        uint32_t c = 0;
        // this is the first time we're drawing the widgetg
        //value = _getDefault(schema);
        bool found = false;
        for(auto & _e : *_enum)
        {
            if(value == _e)
            {
                found = true;
                index = c;
                cache["enumIndex"] = index;
                found = true;
                break;
            }
            ++c;
        }
        if(!found)
        {
            value = _enum->front();
            index = 0;
            cache["enumIndex"] = index;
        }
    }
    index = std::min<uint32_t>(index, static_cast<uint32_t>(_enum->size())-1 );

    std::string _tmpName;
    // Gets the appropriate label that should be visible
    // in the dropdown menu
    auto _getName = [&_tmpName, &_enumNames](size_t i) -> std::string const&
    {
        if(i >= _enumNames->size())
        {
            _tmpName = " ";
            return _tmpName;
        }
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

            auto w = (ImGui::GetContentRegionAvail().x - ImGui::GetStyle().ItemSpacing.x * float(itemsPerRow-1)) / static_cast<float>(itemsPerRow);

            for(uint32_t i=0; i < totalEnums; i++)
            {
                std::string const & btn_label = _getName(i);//_enumNames->at(i).get_ref<std::string const&>();

                bool is_selected = index == i;
                ImGui::PushID(&btn_label);

                auto prevValue = is_selected;
                if(toggleButton(btn_label.c_str(), &is_selected, {w,0}))
                {
                    // we pressed the toggle button
                    // did we go from off->on state
                    if( !prevValue && is_selected)
                    {
                        value = _enum->at(i);
                        return_value = true;
                        cache["enumIndex"] = i;
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
                std::string const & sel_label = _getName(i);

                bool is_selected = index == i;
                if (ImGui::Selectable( sel_label.c_str(), is_selected))
                {
                    if( index != i)
                    {
                        value = _enum->at(i);
                        return_value = true;
                        cache["enumIndex"] = i;
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
#if 1
    // for some reason std::stoul was crashing on
    // EMSCRIPTEN, so using this instead
    ImVec4 out = {0,0,0,1};

    auto hexCharToUInt = [](char hexChar) -> uint32_t {
        if (hexChar >= '0' && hexChar <= '9') {
            return static_cast<uint32_t>(hexChar - '0');
        } else if (hexChar >= 'a' && hexChar <= 'f') {
            return static_cast<uint32_t>(hexChar - 'a' + 10);
        } else if (hexChar >= 'A' && hexChar <= 'F') {
            return static_cast<uint32_t>(hexChar - 'A' + 10);
        } else {
            // Invalid hexadecimal character
            //throw std::invalid_argument("Invalid hexadecimal character");
            return 0;
        }
    };

    auto hexStringToUInt = [&](const std::string& hexString) -> uint32_t
    {
        unsigned int result = 0;

        for (char hexChar : hexString) {
            result = (result << 4) | hexCharToUInt(hexChar);
        }
        return result;
    };

    auto col_u = hexStringToUInt(col);
    out.x = static_cast<float>( (col_u >> 0) & 0xFF)  / 255.0f;
    out.y = static_cast<float>( (col_u >> 8) & 0xFF)  / 255.0f;
    out.z = static_cast<float>( (col_u >> 16) & 0xFF)  / 255.0f;
    out.w = static_cast<float>( (col_u >> 24) & 0xFF)  / 255.0f;
    return out;
#else


    auto r = _hexToInt(col[0], col[1]);
    auto r = _hexToInt(col[2], col[3]);
    auto r = _hexToInt(col[4], col[5]);
    auto r = _hexToInt(col[6], col[7]);

    return out;
    uint32_t x = 0;
    try
    {
        x = static_cast<uint32_t>(std::stoul(col, nullptr, 16));
    }
    catch ( std::exception & e)
    {

    }

    out.x = static_cast<float>( (x >> 0) & 0xFF)  / 255.0f;
    out.y = static_cast<float>( (x >> 8) & 0xFF)  / 255.0f;
    out.z = static_cast<float>( (x >> 16) & 0xFF)  / 255.0f;
    out.w = static_cast<float>( (x >> 24) & 0xFF)  / 255.0f;

    return out;
#endif
}


template<typename value_type>
inline auto numeric_input (WidgetDrawInput &in) -> bool
{
    auto & _object_width = in.object_width;
    //auto & in.cache = in.cache;
    //auto & _label = in.label;
    auto & _schema = in.schema;
    auto & _value = in.value;

    //using value_type = double;
    auto & _val = _value.get_ref<value_type &>();
    auto step      = _schema.value("ui:step"     , std::numeric_limits<value_type>::max() );
    auto step_fast = _schema.value("ui:step_fast", std::numeric_limits<value_type>::max() );

    if(_object_width > 0.0f) ImGui::SetNextItemWidth(_object_width);
    auto _retValue =  Input_T<value_type>("", &_val, step, step_fast);

    _value = std::clamp(_val,
                        JValue(_schema, "minimum", std::numeric_limits<value_type>::lowest()),
                        JValue(_schema, "maximum", std::numeric_limits<value_type>::max()));
    return _retValue;
}

template<typename value_type>
inline auto numeric_slider (WidgetDrawInput &in) -> bool
{
    auto & _object_width = in.object_width;

    auto & _schema = in.schema;
    auto & _value = in.value;
    (void)_object_width;
    auto & _val = _value.get_ref<value_type &>();
    auto minimum = _schema.value("minimum", std::numeric_limits<value_type>::max() );
    auto maximum = _schema.value("maximum", std::numeric_limits<value_type>::max() );

    if(_object_width > 0.0f) ImGui::SetNextItemWidth(_object_width);
    if(minimum < std::numeric_limits<value_type>::max() && maximum < std::numeric_limits<value_type>::max() )
    {
        if(Slider_T<value_type>("", &_val, minimum, maximum))
        {
            _value = std::clamp(_val, minimum, maximum);
            return true;
        }
        return false;
    }
    return numeric_input<value_type>(in);
}

template<typename value_type>
inline auto numeric_drag (WidgetDrawInput &in) -> bool
{
    auto & _object_width = in.object_width;
    //auto & _label = in.label;
    auto & _schema = in.schema;
    auto & _value = in.value;

    (void)_object_width;
    auto & _val = _value.get_ref<value_type &>();
    auto minimum = _schema.value("minimum", std::numeric_limits<value_type>::lowest() );
    auto maximum = _schema.value("maximum", std::numeric_limits<value_type>::max() );

    if(_object_width > 0.0f) ImGui::SetNextItemWidth(_object_width);
    //if(minimum < std::numeric_limits<value_type>::max() && maximum < std::numeric_limits<value_type>::max() )
    {
        auto _speed  = _schema.value("ui:speed", 1.0f );
        if(Drag_T<value_type>("", &_val, _speed, minimum, maximum))
        {
            _value = std::clamp(_val, minimum, maximum);
            return true;
        }
        return false;
    }

    return numeric_input<value_type>(in);
}

inline void drawSchemaDescription(json const & _schema)
{
    ImJSchema::doIfKeyExists("description", _schema, [](auto & S)
                             {
                                 ImGui::TextWrapped("%s", S.template get_ref<std::string const&>().c_str());
                                 ImJSchema::detail::SeparatorLine();
                             });
}
/**
 * @brief getSchemaTitle
 * @param schema
 * @param _default
 * @return
 *
 * Returns the the title of the schema, if the "title" property exists
 * in the schema and is a string, it will return a pointer to the raw
 * string. If not, will return _default
 */
inline char const * getSchemaTitle(json const & schema, char const * _default, char const * prop="title")
{
    auto it = schema.find(prop);
    if(it == schema.end() || !it->is_string())
        return _default;
    return it->get_ref<std::string const&>().c_str();
}

// This is a list of all the widgets that can be drawn using ImJSchema
// you may add or remove items as you please.
//
// the key for the map must be "[object,number,boolean,integer,string,array]/WIDGET_NAME"
//
// ImGui::PushItemWidth(-1) will be called prior to executing any
// of these functions.
inline std::map<std::string, widget_draw_function_type > widgets_all {
    {
        "object/",
        [](WidgetDrawInput &in) -> bool
        {
            IMJSCHEMA_UNUSED
            drawSchemaDescription(in.schema);
            auto returnValue = drawSchemaWidget_Object(in.label, in.value, in.schema, in.cache, in.object_width);
            return returnValue;
        }
    },
    {
        "object/header",
        [](WidgetDrawInput &in) -> bool
        {
            IMJSCHEMA_UNUSED
            drawSchemaDescription(in.schema);
            auto returnValue = drawSchemaWidget_Object(in.label, in.value, in.schema, in.cache, in.object_width);
            return returnValue;
        }
    },
    {
        "object/collapsing",
        [](WidgetDrawInput &in) -> bool
        {
            IMJSCHEMA_UNUSED
            drawSchemaDescription(in.schema);
            auto returnValue = drawSchemaWidget_Object(in.label, in.value, in.schema, in.cache, in.object_width);
            return returnValue;
        }
    },
    {
        "array/",
        [](WidgetDrawInput &in) -> bool
        {
            IMJSCHEMA_UNUSED
            drawSchemaDescription(in.schema);
            auto returnValue = drawSchemaWidget_Array(in.label, in.value, in.schema, in.cache, in.object_width);
            return returnValue;
        }
    },
    {
        "array/vec",
        [](WidgetDrawInput &in) -> bool
        {
            IMJSCHEMA_UNUSED
            auto & _item = in.schema.at("items");
            auto minItems = JValue(in.schema, "minItems", 1);
            auto maxItems = JValue(in.schema, "maxItems", 4);

            bool returnValue = false;
            drawSchemaDescription(in.schema);

            if(minItems != maxItems)
            {
                ImGui::TextWrapped("Cannot draw this widget. schema.minItems != schema.maxItems");
                return false;
            }
            auto totalItems = static_cast<float>(minItems);
            if(true)
            {
                auto W = ImGui::GetContentRegionAvail().x;
                auto t = ImGui::CalcTextSize("A").x;
                auto spacing = ImGui::GetStyle().ItemSpacing.x;

                auto w = (W - (totalItems * (t+spacing)) - (totalItems-1)*spacing) / totalItems;
                ImGui::Text("X");
                ImGui::SameLine();
                returnValue |= drawSchemaWidget_internal("0", in.value[0], _item, in.cache[0], w);

                if(minItems >= 2)
                {
                    ImGui::SameLine();
                    ImGui::Text("Y");
                    ImGui::SameLine();
                    returnValue |= drawSchemaWidget_internal("1", in.value[1], _item, in.cache[1], w);
                }
                if(minItems >= 3)
                {
                    ImGui::SameLine();
                    ImGui::Text("Z");
                    ImGui::SameLine();
                    returnValue |= drawSchemaWidget_internal("2", in.value[2], _item, in.cache[2], w);
                }
                if(minItems >= 4)
                {
                    ImGui::SameLine();
                    ImGui::Text("W");
                    ImGui::SameLine();
                    returnValue |= drawSchemaWidget_internal("3", in.value[3], _item, in.cache[3], w);
                }

            }

            return returnValue;
        }
    },
    {
        "array/color",
        [](WidgetDrawInput &in) -> bool
        {
            IMJSCHEMA_UNUSED
            drawSchemaDescription(in.schema);

            bool isInt = false;
            auto & _type = in.schema.at("items").at("type");
            auto minItems = JValue(in.schema, "minItems", 0);
            auto maxItems = JValue(in.schema, "maxItems", 0);

            if( _type != "number" && _type != "integer")
            {
                return detail::widgets_all["array/"](in);
            }
            isInt = _type.get_ref<std::string const &>()[0] == 'i';
            if(minItems != maxItems)
            {
                return detail::widgets_all["array/"](in);
            }
            if(minItems < 3 || minItems > 4)
            {
                return detail::widgets_all["array/"](in);
            }
            ImGuiColorEditFlags flags = 0;

            if(minItems == 3)
            {
                flags |= ImGuiColorEditFlags_NoAlpha;
            }

            std::array<float, 4> _col = {
                JValue(in.value, 0, 0.0f),
                JValue(in.value, 1, 0.0f),
                JValue(in.value, 2, 0.0f),
                JValue(in.value, 3, 0.0f)
            };

            if(isInt)
            {
                _col[0] /= 255;
                _col[1] /= 255;
                _col[2] /= 255;
                _col[3] /= 255;
            }

            auto ret = ImGui::ColorEdit4("", &_col[0], flags);
            if(ret)
            {
                if(!isInt)
                {
                    if(minItems == 3)
                    {
                        in.value = {_col[0], _col[1], _col[2]};
                    }
                    else
                    {
                        in.value = _col;
                    }
                }
                else
                {
                    if(minItems == 3)
                    {
                        in.value = { static_cast<int64_t>(_col[0] * 255),
                                  static_cast<int64_t>(_col[1] * 255),
                                  static_cast<int64_t>(_col[2] * 255)};
                    }
                    else
                    {
                        in.value = { static_cast<int64_t>(_col[0] * 255),
                                  static_cast<int64_t>(_col[1] * 255),
                                  static_cast<int64_t>(_col[2] * 255),
                                  static_cast<int64_t>(_col[3] * 255)};
                    }
                }
            }
            return ret;
        }
    },
    {
        "number/",
        [](WidgetDrawInput &in) -> bool
        {
            IMJSCHEMA_UNUSED
            auto f = numeric_input<double>(in);
            drawSchemaDescription(in.schema);
            return f;
        }
    },
    {
        "number/slider",
        [](WidgetDrawInput &in) -> bool
        {
            IMJSCHEMA_UNUSED
            auto f =  numeric_slider<double>(in);
            drawSchemaDescription(in.schema);
            return f;
        }
    },
    {
        "number/drag",
        [](WidgetDrawInput &in) -> bool
        {
            IMJSCHEMA_UNUSED
            auto f = numeric_drag<double>(in);
            drawSchemaDescription(in.schema);
            return f;
        }
    },
    {
        "integer/",
        [](WidgetDrawInput &in) -> bool
        {
            IMJSCHEMA_UNUSED
            auto f = numeric_input<int64_t>(in);
            drawSchemaDescription(in.schema);
            return f;
        }
    },
    {
        "integer/slider",
        [](WidgetDrawInput &in) -> bool
        {
            IMJSCHEMA_UNUSED
            auto f = numeric_slider<int64_t>(in);
            drawSchemaDescription(in.schema);
            return f;
        }
    },
    {
        "integer/drag",
        [](WidgetDrawInput &in) -> bool
        {
            IMJSCHEMA_UNUSED
            auto f = numeric_drag<int64_t>(in);
            drawSchemaDescription(in.schema);
            return f;
        }
    },
    // Boolean widgets
    {
        "boolean/",
        [](WidgetDrawInput &in) -> bool
        {
            IMJSCHEMA_UNUSED
            auto & _val = in.value.get_ref<bool &>();
            auto i = ImGui::GetContentRegionAvail().x;
            auto f=  ImGui::Checkbox("", &_val);
            if(in.object_width > 0.0f){
                ImGui::SameLine();
                auto F = ImGui::GetContentRegionAvail().x;
                auto W = i-F;
                ImGui::Dummy({in.object_width-W, 0});
            }
            drawSchemaDescription(in.schema);
            return f;
        }
    },
    {
        "boolean/truefalse",
        [](WidgetDrawInput &in) -> bool
        {
            IMJSCHEMA_UNUSED
            const json _sch = {
                {"type", "string"},
                {"enum", {"False", "True"} }
            };

            bool& _v = in.value.get_ref<bool&>();
            json jval = _v ? _sch["enum"][1] : _sch["enum"][0];
            bool returnValue = drawSchemaWidget_enum("", jval, _sch, in.cache);
            _v = jval == _sch["enum"][1];
            drawSchemaDescription(in.schema);
            return returnValue;
        }
    },
    {
        "boolean/enabledisable",
        [](WidgetDrawInput &in) -> bool
        {
            IMJSCHEMA_UNUSED
            const json _sch = {
                {"type", "string"},
                {"enum", {"Disabled", "Enabled"} }
            };
            bool& _v = in.value.get_ref<bool&>();
            json jval = _v ? _sch["enum"][1] : _sch["enum"][0];
            bool returnValue = drawSchemaWidget_enum("", jval, _sch, in.cache);
            _v = jval == _sch["enum"][1];
            drawSchemaDescription(in.schema);
            return returnValue;
        }
    },
    {
        "boolean/yesno",
        [](WidgetDrawInput &in) -> bool
        {
            IMJSCHEMA_UNUSED
            const json _sch = {
                {"type", "string"},
                {"enum", {"No", "Yes"} }
            };
            bool& _v = in.value.get_ref<bool&>();
            json jval = _v ? _sch["enum"][1] : _sch["enum"][0];
            bool returnValue = drawSchemaWidget_enum("", jval, _sch, in.cache);
            _v = jval == _sch["enum"][1];
            drawSchemaDescription(in.schema);
            return returnValue;
        }
    },
    /// STRING WIDGETS
    ///
    {
        "string/",
        [](WidgetDrawInput &in) -> bool
        {
            IMJSCHEMA_UNUSED
            std::string& json_string_ref = in.value.get_ref<std::string&>();
            auto t = ImGui::InputText("", &json_string_ref, 0, nullptr, nullptr);
            drawSchemaDescription(in.schema);
            return t;
        }
    },
    {
        "string/color_picker",
        [](WidgetDrawInput &in) -> bool
        {
            IMJSCHEMA_UNUSED
            std::string &json_string_ref = in.value.get_ref<std::string&>();
            auto _col = _hexStringToColor(json_string_ref);

            bool retVal=false;

            if(ImGui::ColorPicker4("", &_col.x,  0) )
            {
                uint32_t your_int = ImGui::GetColorU32(_col);
                std::stringstream stream;
                stream << std::setfill ('0') << std::setw(sizeof(uint32_t)*2)
                       << std::hex << your_int;
                in.value = stream.str();
                retVal = true;
            }
            drawSchemaDescription(in.schema);
            return retVal;
        }
    },
    {
        "string/color",
        [](WidgetDrawInput &in) -> bool
        {
            IMJSCHEMA_UNUSED
                std::string &json_string_ref = in.value.get_ref<std::string&>();
            auto _col = _hexStringToColor(json_string_ref);

            bool retVal=false;

            if(ImGui::ColorEdit4("", &_col.x) )
            {
                uint32_t your_int = ImGui::GetColorU32(_col);
                std::stringstream stream;
                stream << std::setfill ('0') << std::setw(sizeof(uint32_t)*2)
                       << std::hex << your_int;
                in.value = stream.str();
                retVal = true;
            }
            drawSchemaDescription(in.schema);
            return retVal;
        }
    },
    {
        "string/textarea",
        [](WidgetDrawInput &in) -> bool
        {
            IMJSCHEMA_UNUSED

            drawSchemaDescription(in.schema);
            std::string &json_string_ref = in.value.get_ref<std::string&>();

            int rows = 5;
            doIfKeyExists("ui:options", in.schema, [&](auto & options)
                          {
                              rows = JValue(options, "rows", 5);
                          });

            auto sy = static_cast<float>(rows) * ImGui::GetTextLineHeight();
            auto v =  ImGui::InputTextMultiline("", &json_string_ref, {0,sy}, 0, nullptr, nullptr);
            return v;
        }
    }
};

inline bool drawSchemaWidget_Array(char const *label, json & value, json const & schema, json & cache, float object_width)
{
    (void)object_width;
    auto item_it = schema.find("items");
    if( item_it == schema.end())
        return false;
    auto & _items = *item_it;

    (void)label;

    {
        auto minItems = JValue(schema, "minItems", value.size()*0);//static_cast<uint32_t>(schema.value("minItems" , 0 ) );
        auto maxItems = JValue(schema, "maxItems", value.size()+1);//static_cast<uint32_t>(schema.value("maxItems" , value.size()+1 ) );
        bool re = false;

        {
            ImGui::PushID(&value);
            auto itemCount = value.size();

            while(value.size() < minItems)
            {
                json J;
                initializeToDefaults(J, _items);
                value.push_back( std::move(J));
            }

            auto full_width = ImGui::GetContentRegionAvail().x;

            auto sz = ImGui::GetFrameHeight();
            ImVec2 buttonSize(sz,sz);

            auto spacing = ImGui::GetStyle().ItemSpacing.x;
            auto padding = ImGui::GetStyle().FramePadding.x;

            auto width = full_width - 3 * buttonSize.x - 2*spacing - padding;

            bool showButtons = (value.size() > minItems) || (value.size() == 0);

            auto appendButtonSize = full_width - width;
            if(!showButtons)
                width = full_width;
            std::string tbName = std::string("ar") + label;
            ImGui::BeginTable("arraytable", showButtons ? 2 : 1);
            ImGui::TableSetupColumn("AAA", ImGuiTableColumnFlags_WidthStretch);

            if(showButtons)
                ImGui::TableSetupColumn("BBB", ImGuiTableColumnFlags_WidthFixed, full_width-width);

            if(!cache.is_array())
                cache = json::array_t();

            bool drawLine = false;
            if(_items.at("type") == "object")
                drawLine = true;
            for(size_t i=0;i<itemCount;i++)
            {
                auto _label = std::to_string(i);

                ImGui::PushID(static_cast<int>(i));
                ImGui::TableNextColumn();

                ImGui::SetNextItemWidth(width );
                ImGui::PushItemWidth(-1);
                re |= drawSchemaWidget_internal(_label.c_str(), value[i], _items, cache[i]);
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
            }
            ImGui::EndTable();

            if(value.size() < maxItems)
            {
                ImGui::Dummy({ImGui::GetContentRegionAvail().x - appendButtonSize - spacing, 0});
                ImGui::SameLine();
                if(ImGui::Button("+", {appendButtonSize, 0}))
                {
                    json J;
                    initializeToDefaults(J, _items);
                    value.push_back( std::move(J));
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
    }
}

inline bool drawSchemaWidget_internal(char const *label, json & propertyValue, json const & propertySchema, json & cache, float object_width)
{
    bool returnValue = false;

    _pushName(label);

    // check if it is an enum first
    if(propertySchema.contains("enum"))
    {
        ImGui::PushItemWidth(-1);
        returnValue |= drawSchemaWidget_enum(label, propertyValue, propertySchema, cache );
        ImGui::PopItemWidth();
        if(returnValue)
        {
            _nodeWidgetModified = true;
        }
    }
    else
    {
        auto _type = JValue(propertySchema, "type", std::string(""));
        {           
            auto _widdraw_it = widgets_all.find( _type + "/" + JValue(propertySchema, "ui:widget", std::string("")) );
            if(_widdraw_it != widgets_all.end() && _widdraw_it->second)
            {
                ImGui::PushItemWidth(-1);
                ImGui::PushID(&propertyValue);
                if(propertyValue.is_null())
                    initializeToDefaults(propertyValue, propertySchema);
                WidgetDrawInput in{label, propertyValue, propertySchema, cache, object_width};
                returnValue = _widdraw_it->second(in);
                if(returnValue)
                {
                    _nodeWidgetModified = true;
                }
                ImGui::PopID();
                ImGui::PopItemWidth();
            }
        }
    }
    _popName();

    return returnValue;
}


/**
 * @brief forEachProperty
 * @param schema
 * @param value
 * @param cache
 * @param cC
 *
 * Template function which will be called for each property in an object schema
 */
template<typename callable_t>
void forEachProperty(json const & schema,
                     json & value,
                     json & cache,
                     callable_t && callback)
{
    auto order_it = schema.find("ui:order");
    auto properties_it = schema.find("properties");

    if(properties_it == schema.end())
        return;

    //
    std::unordered_set<json::object_t::key_type const*> _order;
    if(order_it != schema.end() )
    {
        for(auto & _ord : *order_it)
        {
            auto propertyName      = _ord.get<std::string>();
            auto propertySchema_it = properties_it->find(propertyName);

            if(propertySchema_it == properties_it->end())
                continue;

            auto & propertySchema = *propertySchema_it;

            _order.insert(&propertySchema_it.key());
            if(propertySchema_it->count("type"))
            {
                callback(propertyName, value[propertyName], propertySchema, cache[propertyName]);
            }
        }
    }

    for(auto & [propertyName, propertySchema] : properties_it->items())
    {
        if(_order.count(&propertyName) == 0)
        {
            if(propertySchema.count("type"))
            {
                callback(propertyName, value[propertyName], propertySchema, cache[propertyName]);
            }
        }
    }
}

/**
 * @brief drawSchemaToolTip
 * @param schema
 *
 * draws the tooltip for the schema if it exists
 */
inline void drawSchemaToolTip(json const & schema)
{
    //bool _doheader = true;
    doIfKeyExists("ui:help", schema, [](auto & _help)
                  {
                      if(_help.is_string() && ImGui::IsItemHovered(ImGuiHoveredFlags_DelayNormal) )
                      {
                          ImGui::SetTooltip("%s", _help.template get_ref<std::string const&>().c_str());
                      }
                  });
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
inline bool drawSchemaWidget_Object_withoutOneOf(char const * label, json & objectValue, json const & schema, json & cache, float widget_size)
{
    (void)label;
    if(!schema.is_object())
        return false;

    if(!cache.is_object())
        cache = json::object_t();

    auto properties_it = schema.find("properties");

    if(properties_it == schema.end() )
        return false;

    bool returnValue = false;

    float max_label_size = 0.0f;

    auto C1Width = 25.0f;
    auto C2Width = 75.0f;

    // if we are given an input widget size, use that as
    // the total size of the object instead of whatever
    // is calculated
    auto availWidth = widget_size > 0.0f ? widget_size : ImGui::GetContentRegionAvail().x;

    auto C1Flags    = ImGuiTableColumnFlags_WidthFixed;
    auto C2Flags    = ImGuiTableColumnFlags_WidthStretch;
    auto tableFlags = ImGuiTableFlags_SizingFixedSame | ImGuiTableFlags_SizingFixedSame;

    // get the maximum label size from the cache
    // this will be written during the first draw
    // cycle.
    C1Width = JValue(cache, "max_label_size", C1Width);
    C2Width = availWidth - C1Width;

    bool showReset = JValue(schema, "ui:showReset", false);
    tableFlags |= JValue(schema, "ui:resizable", false) ? ImGuiTableFlags_Resizable : 0;

    auto & optional_items = cache["optional_items"];
    auto & required_items = cache["required_items"];

    std::string addPropertyStr = "Add";
    bool showAddProperty = false;
    //if(required_items.empty())
    {
        // all properties by default will be required
        // if the 'required' properties is not provided
        forEachProperty(schema, objectValue, cache, [&](std::string const & propertyName, json & propertyValue, json const & propertySchema, json & propertyCache)
                        {
                            (void)propertyValue;
                            (void)propertySchema;
                            (void)propertyCache;
                            required_items[propertyName] = true;
                        });

        auto required_it = schema.find("required");
        if(required_it != schema.end() && required_it->is_array())
        {
            // if required array is set, then all
            // false reuqired items should be set to false
            // and only the ones listed in the array should be true
            for(auto & [r,s] : required_items.items())
            {
                (void)r;
                s = false;
            }
            uint32_t count=0;
            for(auto & v : *required_it)
            {
                required_items[v] = true;
                ++count;
            }
            if(count == properties_it->size())
            {
                showAddProperty = false;
            }
            else
            {
                showAddProperty = true;
            }

        }
    }

    auto currPath = ImJSchema::detail::_path_ptr;
    bool propertyHasBeenEnabled = false;
    if (ImGui::BeginPopupContextItem("my popup"))
    {
        forEachProperty(schema, objectValue, cache, [&](std::string const & propertyName, json & propertyValue, json const & propertySchema, json & propertyCache)
                        {
                            (void)propertyValue;
                            (void)propertySchema;
                            (void)propertyCache;

                            bool _selected = !(!objectValue.contains(propertyName) || objectValue.at(propertyName).is_null());

                            if(required_items[propertyName] == false)
                            {
                                auto _title = getSchemaTitle(propertySchema, propertyName.c_str());
                                if(ImGui::Checkbox(_title, &_selected))
                                {
                                    if(_selected)
                                    {
                                        propertyValue = {};
                                        initializeToDefaults(propertyValue, propertySchema);
                                    }
                                    else
                                    {
                                        propertyValue = {};
                                        optional_items[propertyName] = true;
                                    }
                                    currPath.push_back(propertyName);
                                    returnValue = true;
                                    propertyHasBeenEnabled = true;
                                }
                            }
                        });
        ImGui::EndPopup();
    }
    if(showAddProperty)
    {
        auto _label = getSchemaTitle(schema, addPropertyStr.c_str(), "ui:addPropertyButtonLabel");

        const auto label_size = ImGui::CalcTextSize(_label, nullptr, true);
        auto & style = ImGui::GetStyle();

        ImVec2 size = ImGui::CalcItemSize({0,0}, label_size.x + style.FramePadding.x * 2.0f, label_size.y + style.FramePadding.y * 2.0f);

        ImGui::Dummy({ImGui::GetContentRegionAvail().x - size.x - style.ItemSpacing.x, 0});
        ImGui::SameLine();
        if(ImGui::Button(_label, size))
        {
            ImGui::OpenPopup("my popup");
        }
    }

    if(showReset)
    {
        auto _label = getSchemaTitle(schema, "Reset", "ui:resetButtonLabel");

        const auto label_size = ImGui::CalcTextSize(_label, nullptr, true);
        auto & style = ImGui::GetStyle();

        ImVec2 size = ImGui::CalcItemSize({0,0}, label_size.x + style.FramePadding.x * 2.0f, label_size.y + style.FramePadding.y * 2.0f);

        ImGui::Dummy({ImGui::GetContentRegionAvail().x - size.x - style.ItemSpacing.x, 0});
        ImGui::SameLine();
        if(ImGui::Button(_label, size))
        {
            objectValue = {};
            initializeToDefaults(objectValue, schema);
            cache = {};
            //ImGui::OpenPopup("my popup");
        }
    }
    std::string _tableName = std::string("tb") + label;
    ImGui::BeginTable(_tableName.c_str(), 2, tableFlags, {availWidth, 0.0f});
    ImGui::TableSetupColumn("AAA", C1Flags, C1Width);
    ImGui::TableSetupColumn("BBB", C2Flags, C2Width);
    {
        forEachProperty(schema, objectValue, cache, [&](std::string const & propertyName, json & propertyValue, json const & propertySchema, json & propertyCache)
                        {
                            auto type = JValue(propertySchema, "type", std::string(""));
                            auto _title = getSchemaTitle(propertySchema, propertyName.c_str());

                            bool hidden = JValue(propertySchema, "ui:hidden", false);
                            if(hidden)
                                return;

                            if(propertyValue.is_null() && required_items[propertyName] == false)
                            {
                                //optional_items[propertyName] = true;
                                return;
                            }

                            max_label_size = std::max(max_label_size, ImGui::CalcTextSize(_title).x) + 5;
                            cache["max_label_size"] = max_label_size;

                            if(type == "object" || type == "array")
                            {
                                auto ui_widget = JValue(propertySchema, "ui:widget", std::string());
                                if( ui_widget == "header" )
                                {
                                    ImGui::EndTable();

                                    HeaderText(_title);
                                    returnValue |= drawSchemaWidget_internal(propertyName.c_str(), propertyValue, propertySchema, propertyCache);

                                    ImGui::BeginTable("OuterTable", 2, tableFlags, {availWidth, 0.0f});
                                    ImGui::TableSetupColumn("AAA", C1Flags, C1Width);
                                    ImGui::TableSetupColumn("BBB", C2Flags, C2Width);
                                }
                                else if (ui_widget == "collapsing")
                                {
                                    ImGui::EndTable();

                                    if(ImGui::CollapsingHeader(_title, ImGuiTreeNodeFlags_DefaultOpen))
                                    {
                                        returnValue |= drawSchemaWidget_internal(propertyName.c_str(), propertyValue, propertySchema, propertyCache);
                                    }

                                    ImGui::BeginTable("OuterTable", 2, tableFlags, {availWidth, 0.0f});
                                    ImGui::TableSetupColumn("AAA", C1Flags, C1Width);
                                    ImGui::TableSetupColumn("BBB", C2Flags, C2Width);
                                }
                                else
                                {
                                    ImGui::TableNextColumn();
                                    ImGui::Text("%s", _title);
                                    drawSchemaToolTip(propertySchema);
                                    ImGui::TableNextColumn();
                                    returnValue |= drawSchemaWidget_internal(propertyName.c_str(), propertyValue, propertySchema, propertyCache);
                                }
                            }
                            else
                            {
                                ImGui::TableNextColumn();
                                ImGui::Text("%s", _title);
                                drawSchemaToolTip(propertySchema);
                                ImGui::TableNextColumn();
                                returnValue |= drawSchemaWidget_internal(propertyName.c_str(), propertyValue, propertySchema, propertyCache);
                            }
                            if(optional_items.is_object())
                                optional_items.erase(propertyName);
                        });
        for(auto & [key, val]: optional_items.items())
        {
            objectValue.erase(key);
        }
    }
    ImGui::EndTable(); // OuterTable


    if(propertyHasBeenEnabled)
        ImJSchema::detail::_path_ptr = currPath;
    return returnValue;
}


inline bool drawSchemaWidget_Object(char const * label, json & objectValue, json const & schema, json & cache, float widget_size)
{
    auto oneOf_it = schema.find("oneOf");
    if(oneOf_it == schema.end())
        return drawSchemaWidget_Object_withoutOneOf(label, objectValue, schema, cache, widget_size) ;

    auto _getTitle = [](json::const_iterator &J) -> char const*
    {
        if(J->count("title"))
        {
            return J->at("title").get_ref<std::string const&>().c_str();
        }
        return "Option";
    };

    bool returnVal = false;

    if(schema.count("oneOf"))
    {
        auto & oneOf = schema.at("oneOf");
        auto index = JValue(cache, "oneOfIndex", uint32_t{0xFFFFFFFF});

        if(index == 0xFFFFFFFF)
        {
            initializeToDefaults(objectValue, *oneOf.begin());
            index = 0u;
        }

        auto it_s = oneOf.begin() + static_cast<int32_t>(index);

        if(ImGui::BeginCombo("One Of", _getTitle(it_s)))
        {
            uint32_t i=0;
            for(auto it = oneOf.begin(); it != oneOf.end(); ++it)
            {
                auto sel_label = _getTitle(it);

                bool is_selected = index == i;
                if (ImGui::Selectable( sel_label, is_selected))
                {
                    if( index != i)
                    {
                        cache["oneOfIndex"] = i;
                        cache["cachedValue"][index] = objectValue;
                        it_s = it;
                        objectValue = cache["cachedValue"][i] = objectValue;
                        initializeToDefaults(objectValue, *it_s);

                        returnVal = true;
                    }
                }
                ++i;
            }

            ImGui::EndCombo();
        }

        if(it_s != oneOf.end())
        {
            returnVal |= drawSchemaWidget_Object_withoutOneOf(label, objectValue, *it_s, cache, widget_size) ;
        }
    }


    return returnVal;
}

} // detail


inline bool drawSchemaWidget(WidgetDrawInput& in)
{
    detail::_nodeWidgetModified = false;
    detail::_path_ptr = json::json_pointer{};
    initializeToDefaults(in.value, in.schema);
    return detail::drawSchemaWidget_internal(in.label, in.value, in.schema, in.cache, in.object_width);
}


inline json::json_pointer getModifiedWidgetPath()
{
    auto str = detail::_path_ptr.to_string();
    if(str.front() == '/')
    {
        auto i = str.find_first_of('/',1);
        if( i != std::string::npos)
        {
            return json::json_pointer( str.substr(i));
        }
    }
    return detail::_path_ptr;
}

}

#endif

