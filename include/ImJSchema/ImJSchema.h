#ifndef SNE_IMGUI_JSON_UI_H
#define SNE_IMGUI_JSON_UI_H

#include <imgui.h>
#include <imgui_stdlib.h>

#include "detail/imgui_widgets_t.h"
#include "detail/json_utils.h"

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


using widget_draw_function_type = std::function<bool(char const*, json&, json const&, json&, float) >;
#define IMJSCHEMA_LAMBDA_HEADER (char const* _label, json & _value, json const& _schema, json & _cache, float _object_width = 0.0f) -> bool

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

bool drawSchemaWidget_Array(char const *label, json & value, json const & schema, json &cache);


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

inline bool _nodeWidgetModified = false;
inline std::string _path_str;

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
        if(i != std::string::npos )
        {
            _path_str.resize(i);
        }
        while(_path_str.size() && _path_str.back() == '/')
            _path_str.pop_back();
    }
}

inline bool drawSchemaWidget_enum2(char const * label, json & value, json const & schema, json & _cache)
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

    if(!_cache.is_object())
        _cache = json::object_t();

    uint32_t index = _cache.value<uint32_t>("enumIndex", 0xFFFFFFFF );

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
                std::string const & sel_label = _getName(i);

                bool is_selected = index == i;
                if (ImGui::Selectable( sel_label.c_str(), is_selected))
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
}


template<typename value_type>
inline auto numeric_input IMJSCHEMA_LAMBDA_HEADER
{
        (void)_object_width;
     (void)_cache;
     (void)_label;
     (void)_schema;
     //using value_type = double;
     auto & _val = _value.get_ref<value_type &>();
     auto step      = _schema.value("ui:step"     , std::numeric_limits<value_type>::max() );
     auto step_fast = _schema.value("ui:step_fast", std::numeric_limits<value_type>::max() );

     auto _retValue =  InputScalar_T<value_type>("", &_val, step, step_fast);

     _value = std::clamp(_val,
                        JValue(_schema, "minimum", std::numeric_limits<value_type>::lowest()),
                        JValue(_schema, "maximum", std::numeric_limits<value_type>::max()));
     return _retValue;
}

template<typename value_type>
inline auto numeric_slider IMJSCHEMA_LAMBDA_HEADER
{
    (void)_object_width;
    auto & _val = _value.get_ref<value_type &>();
    auto minimum = _schema.value("minimum", std::numeric_limits<value_type>::max() );
    auto maximum = _schema.value("maximum", std::numeric_limits<value_type>::max() );

    if(minimum < std::numeric_limits<value_type>::max() && maximum < std::numeric_limits<value_type>::max() )
    {
        if(SliderScalar_T<value_type>("", &_val, minimum, maximum))
        {
            _value = std::clamp(_val, minimum, maximum);
            return true;
        }
        return false;
    }
    return numeric_input<value_type>(_label, _value, _schema, _cache);
}

template<typename value_type>
inline auto numeric_drag IMJSCHEMA_LAMBDA_HEADER
{
        (void)_object_width;
    auto & _val = _value.get_ref<value_type &>();
    auto minimum = _schema.value("minimum", std::numeric_limits<value_type>::max() );
    auto maximum = _schema.value("maximum", std::numeric_limits<value_type>::max() );

    if(minimum < std::numeric_limits<value_type>::max() && maximum < std::numeric_limits<value_type>::max() )
    {
        auto _speed  = _schema.value("ui:speed", 1.0f );
        if(DragScalar_T<value_type>("", &_val, _speed, minimum, maximum))
        {
            _value = std::clamp(_val, minimum, maximum);
            return true;
        }
        return false;
    }
    return numeric_input<value_type>(_label, _value, _schema, _cache);
}

// the key should be:
//
//  json_type/ui:widget
//
// eg: boolean/switch
#define IMJSCHEMA_DRAW_DESCRIPTION(_schema) \
            (void)_object_width;\
            (void)_schema;\
            (void)_value;\
            (void)_cache;\
            ImJSchema::doIfKeyExists("description", _schema, [](auto & S) \
                          {\
                              ImGui::TextWrapped("%s", S.template get_ref<std::string const&>().c_str());\
                              ImJSchema::detail::SeparatorLine(); \
                          })

// This is a list of all the widgets that can be drawn using ImJSchema
// you may add or remove items as you please.
//
// the key for the map must be "[object,number,boolean,integer,string,array]/WIDGET_NAME"
inline std::map<std::string, widget_draw_function_type > widgets_all {
    {
        "object/",
        []IMJSCHEMA_LAMBDA_HEADER
        {
             IMJSCHEMA_DRAW_DESCRIPTION(_schema);
             auto returnValue = drawSchemaWidget_Object(_label, _value, _schema, _cache, _object_width);
             return returnValue;
        }
    },
    {
        "object/header",
        []IMJSCHEMA_LAMBDA_HEADER
        {
             IMJSCHEMA_DRAW_DESCRIPTION(_schema);
             auto returnValue = drawSchemaWidget_Object(_label, _value, _schema, _cache, _object_width);
             return returnValue;
        }
    },
    {
        "object/collapsing",
        []IMJSCHEMA_LAMBDA_HEADER
        {
             IMJSCHEMA_DRAW_DESCRIPTION(_schema);
             auto returnValue = drawSchemaWidget_Object(_label, _value, _schema, _cache, _object_width);
             return returnValue;
        }
    },
    {
        "array/",
        []IMJSCHEMA_LAMBDA_HEADER
        {
             IMJSCHEMA_DRAW_DESCRIPTION(_schema);
             auto returnValue = drawSchemaWidget_Array(_label, _value, _schema, _cache);
             return returnValue;
        }
    },
    {
        "array/color",
        []IMJSCHEMA_LAMBDA_HEADER
        {
            IMJSCHEMA_DRAW_DESCRIPTION(_schema);

            bool isInt = false;
            auto & _type = _schema.at("items").at("type");
            auto minItems = JValue(_schema, "minItems", 0);
            auto maxItems = JValue(_schema, "maxItems", 0);

            if( _type != "number" && _type != "integer")
            {
                return detail::widgets_all["array/"](_label, _value, _schema, _cache, _object_width);
            }
            isInt = _type.get_ref<std::string const &>()[0] == 'i';
            if(minItems != maxItems)
            {
                return detail::widgets_all["array/"](_label, _value, _schema, _cache, _object_width);
            }
            if(minItems < 3 || minItems > 4)
            {
                return detail::widgets_all["array/"](_label, _value, _schema, _cache, _object_width);
            }
            ImGuiColorEditFlags flags = 0;

            if(minItems == 3)
            {
                flags |= ImGuiColorEditFlags_NoAlpha;
            }

            std::array<float, 4> _col = {
                JValue(_value, 0, 0.0f),
                JValue(_value, 1, 0.0f),
                JValue(_value, 2, 0.0f),
                JValue(_value, 3, 0.0f)
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
                        _value = {_col[0], _col[1], _col[2]};
                    }
                    else
                    {
                        _value = _col;
                    }
                }
                else
                {
                    if(minItems == 3)
                    {
                        _value = { static_cast<int64_t>(_col[0] * 255),
                                  static_cast<int64_t>(_col[1] * 255),
                                  static_cast<int64_t>(_col[2] * 255)};
                    }
                    else
                    {
                        _value = { static_cast<int64_t>(_col[0] * 255),
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
        []IMJSCHEMA_LAMBDA_HEADER
        {
             auto f = numeric_input<double>(_label, _value, _schema, _cache);
             IMJSCHEMA_DRAW_DESCRIPTION(_schema);
             return f;
        }
    },
    {
        "number/slider",
        []IMJSCHEMA_LAMBDA_HEADER
        {
            auto f =  numeric_slider<double>(_label, _value, _schema, _cache);
            IMJSCHEMA_DRAW_DESCRIPTION(_schema);
            return f;
        }
    },
    {
        "number/drag",
        []IMJSCHEMA_LAMBDA_HEADER
        {
             auto f = numeric_drag<double>(_label, _value, _schema, _cache);
             IMJSCHEMA_DRAW_DESCRIPTION(_schema);
             return f;
        }
    },
    {
        "integer/",
        []IMJSCHEMA_LAMBDA_HEADER
        {
             auto f = numeric_input<int64_t>(_label, _value, _schema, _cache);
             IMJSCHEMA_DRAW_DESCRIPTION(_schema);
             return f;
        }
    },
    {
        "integer/slider",
        []IMJSCHEMA_LAMBDA_HEADER
        {
            auto f = numeric_slider<int64_t>(_label, _value, _schema, _cache);
            IMJSCHEMA_DRAW_DESCRIPTION(_schema);
            return f;
        }
    },
    {
        "integer/drag",
        []IMJSCHEMA_LAMBDA_HEADER
        {
            auto f = numeric_drag<int64_t>(_label, _value, _schema, _cache);
            IMJSCHEMA_DRAW_DESCRIPTION(_schema);
            return f;
        }
    },
    // Boolean widgets
    {
        "boolean/",
        []IMJSCHEMA_LAMBDA_HEADER
        {
             (void)_cache;
             (void)_label;
             (void)_schema;
             auto & _val = _value.get_ref<bool &>();
             auto f=  ImGui::Checkbox("", &_val);
             IMJSCHEMA_DRAW_DESCRIPTION(_schema);
             return f;
        }
    },
    {
        "boolean/truefalse",
        []IMJSCHEMA_LAMBDA_HEADER
        {
             (void)_label;
             (void)_schema;
             (void)_cache;


            const json _sch = {
                {"type", "string"},
                {"enum", {"False", "True"} }
            };
            bool& _v = _value.get_ref<bool&>();
            json jval = _v ? _sch["enum"][1] : _sch["enum"][0];
            bool returnValue = drawSchemaWidget_enum2("", jval, _sch, _cache);
            _v = jval == _sch["enum"][1];
            IMJSCHEMA_DRAW_DESCRIPTION(_schema);
            return returnValue;
        }
    },
    {
        "boolean/enabledisable",
        []IMJSCHEMA_LAMBDA_HEADER
        {
            (void)_label;
            (void)_schema;
            (void)_cache;

            const json _sch = {
                {"type", "string"},
                {"enum", {"Disabled", "Enabled"} }
            };
            bool& _v = _value.get_ref<bool&>();
            json jval = _v ? _sch["enum"][1] : _sch["enum"][0];
            bool returnValue = drawSchemaWidget_enum2("", jval, _sch, _cache);
            _v = jval == _sch["enum"][1];
            IMJSCHEMA_DRAW_DESCRIPTION(_schema);
            return returnValue;
        }
    },

    /// STRING WIDGETS
    ///
    {
        "string/",
        []IMJSCHEMA_LAMBDA_HEADER
        {
            (void)_label;
            (void)_schema;
            (void)_value;
            (void)_cache;
            std::string& json_string_ref = _value.get_ref<std::string&>();
            auto t = ImGui::InputText("", &json_string_ref, 0, nullptr, nullptr);
            IMJSCHEMA_DRAW_DESCRIPTION(_schema);
            return t;
        }
    },
    {
        "string/color_picker",
        []IMJSCHEMA_LAMBDA_HEADER
        {
            (void)_label;
            (void)_schema;
            (void)_value;
            (void)_cache;
            std::string &json_string_ref = _value.get_ref<std::string&>();
            auto _col = _hexStringToColor(json_string_ref);

            bool retVal=false;

            if(ImGui::ColorPicker4("", &_col.x,  0) )
            {
                uint32_t your_int = ImGui::GetColorU32(_col);
                std::stringstream stream;
                stream << std::setfill ('0') << std::setw(sizeof(uint32_t)*2)
                       << std::hex << your_int;
                _value = stream.str();
                retVal = true;
            }
            IMJSCHEMA_DRAW_DESCRIPTION(_schema);
            return retVal;
        }
    },
    {
        "string/color",
        []IMJSCHEMA_LAMBDA_HEADER
        {
            (void)_label;
            (void)_schema;
            (void)_value;
            (void)_cache;
            std::string &json_string_ref = _value.get_ref<std::string&>();
            auto _col = _hexStringToColor(json_string_ref);

            bool retVal=false;

            if(ImGui::ColorEdit4("", &_col.x) )
            {
                uint32_t your_int = ImGui::GetColorU32(_col);
                std::stringstream stream;
                stream << std::setfill ('0') << std::setw(sizeof(uint32_t)*2)
                       << std::hex << your_int;
                _value = stream.str();
                retVal = true;
            }
            IMJSCHEMA_DRAW_DESCRIPTION(_schema);
            return retVal;
        }
    },
    {
        "string/textarea",
        []IMJSCHEMA_LAMBDA_HEADER
        {
            (void)_label;
            (void)_schema;
            (void)_value;
            (void)_cache;

            IMJSCHEMA_DRAW_DESCRIPTION(_schema);
            std::string &json_string_ref = _value.get_ref<std::string&>();

            int rows = 5;
            doIfKeyExists("ui:options", _schema, [&](auto & options)
            {
                rows = JValue(options, "rows", 5);
            });

            auto sy = static_cast<float>(rows) * ImGui::GetTextLineHeight();
            auto v =  ImGui::InputTextMultiline("", &json_string_ref, {0,sy}, 0, nullptr, nullptr);
            return v;
        }
    }
};


inline bool drawSchemaWidget_Array(char const *label, json & value, json const & schema, json & cache)
{
    auto item_it = schema.find("items");
    if( item_it == schema.end())
        return false;
    auto & _items = *item_it;

   // std::string widget = schema.value("ui:widget" , "" );

    (void)label;
   // auto wid_it = widgets_array.find(widget);

    if(!value.is_array())
    {
        value = _getDefault(schema);
    }

    // if(_type == "array")
    {
        auto minItems = JValue(schema, "minItems", value.size()*0);//static_cast<uint32_t>(schema.value("minItems" , 0 ) );
        auto maxItems = JValue(schema, "maxItems", value.size()+1);//static_cast<uint32_t>(schema.value("maxItems" , value.size()+1 ) );
        bool re = false;

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

            auto width = full_width - 3 * buttonSize.x - 2*spacing - padding;

            bool showButtons = (value.size() > minItems) || (value.size() == 0);

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

            if(value.size() < maxItems)
            {
                ImGui::TableNextColumn();
                ImGui::TableNextColumn();
                if(ImGui::Button("+", {ImGui::GetContentRegionAvail().x, 0}))
                {
                    value.push_back( _getDefault(_items) );
                    re |= true;
                }
                if(ImGui::IsItemHovered())
                {
                    ImGui::SetTooltip("Append a new item to the array");
                }
            }

            ImGui::EndTable();


            ImGui::PopID();

            return re;
        }

        return re;
    }

    return false;
}


/**
 * @brief setDefaultIfNeeded
 * @param value
 * @param schema
 *
 * If the schema defines a "default", then sets value to the
 * default if value.type != default.type
 *
 */
void setDefaultIfNeeded(json & value, json const &schema);

inline bool drawSchemaWidget_internal(char const *label, json & propertyValue, json const & propertySchema, json & cache, float object_width)
{
    bool returnValue = false;

    setDefaultIfNeeded(propertyValue, propertySchema);

    _pushName(label);

    // check if it is an enum first
    if(propertySchema.contains("enum"))
    {
        ImGui::PushItemWidth(-1);
        returnValue |= drawSchemaWidget_enum2(label, propertyValue, propertySchema, cache );
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
                    returnValue = _widdraw_it->second(label, propertyValue, propertySchema, cache, object_width);
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


inline void setDefaultIfNeeded(json & value, json const &schema)
{
    if(value.is_null())
        value = _getDefault(schema);
    auto type = JValue(schema, "type", std::string());

    if(type == "boolean" && !value.is_boolean())
        value = _getDefault(schema);
    if(type == "number" && !value.is_number())
        value = _getDefault(schema);
    if(type == "integer" && !value.is_number_integer())
        value = _getDefault(schema);
    if(type == "string" && !value.is_string())
        value = _getDefault(schema);
    if(type == "array" && !value.is_array())
        value = _getDefault(schema);
    if(type == "object" && !value.is_object())
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
    (void)label;
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


    auto label_width   = JValue(schema, "ui:label_width", 0.0f);
    auto label_width_fixed   = JValue(schema, "ui:label_width_fixed", false);
    auto column_size   = JValue(schema, "ui:column_size", 0.0f);
    auto column_resize = JValue(schema, "ui:column_resizable", false);
    column_size = std::clamp(column_size, 0.0f, 100.0f);

    auto tableFlags = ImGuiTableFlags_SizingStretchSame | ImGuiTableFlags_SizingStretchSame;
    auto C1Flags = ImGuiTableColumnFlags_WidthStretch;
    auto C2Flags = ImGuiTableColumnFlags_WidthStretch;

    float availWidth = ImGui::GetContentRegionAvail().x;
    float textSize = 0.0f;

    auto C1Width = 25.0f;
    auto C2Width = 75.0f;

    availWidth = widget_size > 0.0f ? widget_size : availWidth;
    C1Flags    = ImGuiTableColumnFlags_WidthFixed;
    C2Flags    = ImGuiTableColumnFlags_WidthStretch;
    tableFlags = ImGuiTableFlags_SizingFixedSame;

    if(cache.count("label_size"))
    {
        C1Width = cache.at("label_size").get<float>();
    }

    if(label_width > 0.0f)
    {
        if(label_width_fixed)
        {
            C1Width = label_width;
        }
        else
        {
            C1Width = label_width * availWidth;
        }
    }
    C2Width = availWidth - C1Width;

    if(column_resize)
        tableFlags |= ImGuiTableFlags_Resizable;

    auto _drawProperty = [&](std::string const & propertyName,
                             std::string const & propertyTitle,
                            json const & propertySchema,
                            json & propertyValue)
    {
        auto isHidden   = JValue(propertySchema, "ui:hidden",   false);
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

            returnValue |= drawSchemaWidget_internal(propertyName.c_str(), propertyValue, propertySchema, cache[propertyName]);

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
    auto _getVisibleTitle = [](json const & _schema, std::string const & propName) -> std::string const&
    {
        auto it = _schema.find("title");
        if(it != _schema.end() && it->is_string())
            return it->get_ref<std::string const&>();
        return propName;
    };

    // for each property, call the lambda with the property name and the property schema
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
            setDefaultIfNeeded(propertyValue, propertySchema);

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
                doIfKeyExists("ui:help", propertySchema, [](auto & _help)
                              {
                                  if(_help.is_string() && ImGui::IsItemHovered(ImGuiHoveredFlags_DelayNormal) )
                                  {
                                      ImGui::SetTooltip("%s", _help.template get_ref<std::string const&>().c_str());
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



} // detail


inline bool drawSchemaWidget(char const *label, json & propertyValue, json const & propertySchema, json & cache, float object_width)
{
    detail::_nodeWidgetModified = false;
    detail::_path_str.clear();
    return detail::drawSchemaWidget_internal(label, propertyValue, propertySchema, cache, object_width);
}


inline std::string const& getModifiedWidgetPath()
{
    return detail::_path_str;
}


}

#endif

