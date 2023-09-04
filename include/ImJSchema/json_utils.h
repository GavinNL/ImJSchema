#ifndef IMJSCHEMA_JSON_UTILS_H
#define IMJSCHEMA_JSON_UTILS_H

#include <nlohmann/json.hpp>
#include <charconv>

namespace ImJSchema
{
using json = nlohmann::json;

template<typename ValueType>
ValueType JValue(json const & J, json::object_t::key_type const & key, const ValueType& default_value)
{
    auto it = J.find(key);
    if(it == J.end())
        return default_value;

    if constexpr ( std::is_same_v<ValueType, std::string> )
    {
        if(it->is_string())
            return it->get<ValueType>();
        return default_value;
    }
    else if constexpr ( std::is_same_v<ValueType, bool> )
    {
        if(it->is_boolean())
            return it->get<ValueType>();
        return default_value;
    }
    else
    {
        if constexpr ( std::is_arithmetic_v<ValueType> )
        {
            if(it->is_number())
                return it->get<ValueType>();
            return default_value;
        }
    }
    return default_value;
}

template<typename T>
inline T * _jsonFindPath(std::string_view path, T & obj);

/**
 * @brief jsonFindPath
 * @param path
 * @param obj
 * @return
 *
 * Given a path in a json object. Returns a pointer to the json object
 */
inline json const* jsonFindPath(std::string_view path, json const & obj)
{
    return _jsonFindPath<json const>(path, obj);
}
inline json * jsonFindPath(std::string_view path, json & obj)
{
    return _jsonFindPath<json>(path, obj);
}

/**
 * @brief jsonExpandDef
 * @param J
 *
 * Given a json object that has a property named, ref, which is
 * a string that represents a path to an object in defsRoot
 # eg:
"items": {
    "name" : "Hello",
    "defaultValue" : "World",
    "$ref": "#/$defs/positiveInteger"
}

// defsRoot
{
    "$defs": {
        "positiveInteger": {
            "type": "integer",
            "exclusiveMinimum": 0,
            "defaultValue" : "Hello"
        }
    }
}

Copies the values from defsRoot which are not in the original object
{
    "items": {
        "name" : "Hello",
        "defaultValue" : "World",
        "type": "integer",
        "exclusiveMinimum": 0
    }
}
*/
inline void jsonExpandDef(json & J, json const & defsRoot, std::string ref = "$ref")
{
    auto it = J.find(ref);
    if(it == J.end())
    {
        return;
    }
    if(!it->is_string())
    {
        return;
    }

    std::string & path = it->get_ref<std::string&>();
    if(path.empty())
    {
        return;
    }
    std::string_view path_view(path);
    if(path_view.front() == '#')
    {
        path_view = path_view.substr(1);
    }
    if(path_view.front() == '/')
    {
        path_view = path_view.substr(1);
    }

    auto def_it = jsonFindPath(path_view, defsRoot);
    if(!def_it)
        return;

    json d = *def_it;
    if(d.is_object())
    {
        d.merge_patch(J);
        d.erase(ref);
    }
    J = std::move(d);

}

/**
 * @brief jsonExpandAllDefs
 * @param J
 * @param defsRoot
 * @param ref
 *
 * The same as the jsonExpandDef, but does it recursively for all objects/arrays
 */
inline void jsonExpandAllDefs(json & J, json const & defsRoot, std::string ref="$ref")
{
    if(J.is_array())
    {
        for(auto & i : J)
        {
            jsonExpandAllDefs(i, defsRoot, ref);
        }
    }
    if(J.is_object())
    {
        jsonExpandDef(J, defsRoot, ref);
        for(auto [key, value] : J.items())
        {
            jsonExpandAllDefs(value, defsRoot, ref);
        }
    }
}


//=============== Private Functions ======================
template<typename T>
inline T * _jsonFindPath(std::string_view path, T & obj)
{
    // path == "grandParent/parent/child"
    if(path.empty())
    {
        return &obj;
    }

    auto i = path.find_first_of('/');
    std::string_view stem;
    std::string_view leaf;

    if(i != std::string_view::npos)
    {
        stem = path.substr(0,i); // "grandParent"
        leaf = path.substr(i+1,std::string_view::npos);  // "parent/child"
    }
    else
    {
        leaf = path;
    }

    if(!stem.empty())
    {
        if(obj.is_object())
        {
            auto it = obj.find(stem);
            if(it != obj.end())
            {
                return _jsonFindPath<T>(leaf, *it);
            }
        }
        else if(obj.is_array())
        {
            size_t i3;
            auto result = std::from_chars(stem.data(), stem.data() + stem.size(), i3);
            if (result.ec != std::errc::invalid_argument)
            {
                if(i3 < obj.size())
                {
                    return _jsonFindPath<T>(leaf, obj.at(i3));
                }
            }
        }
        return nullptr;
    }
    else
    {
        if(obj.is_object())
        {
            auto it = obj.find(leaf);
            if( it != obj.end())
            {
                return &(*it);
            }
        }
        else if(obj.is_array())
        {
            size_t i3;
            auto result = std::from_chars(leaf.data(), leaf.data() + leaf.size(), i3);
            if (result.ec != std::errc::invalid_argument)
            {
                if(i3 < obj.size())
                {
                    return &obj.at(i3);
                }
            }
        }
        return nullptr;
    }
}


}


#endif

