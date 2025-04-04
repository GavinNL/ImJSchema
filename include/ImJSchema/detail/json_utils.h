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
#ifndef IMJSCHEMA_JSON_UTILS_H
#define IMJSCHEMA_JSON_UTILS_H

#include <nlohmann/json.hpp>
#include <charconv>

namespace ImJSchema
{
using json = nlohmann::json;

/**
 * @brief jsonExpandAllReferences
 * @param J
 *
 * Given a json object that has a property named, ref, which is
 * a string that represents a path to an object in defsRoot
 *
 # eg:
"items": {
    "name" : "Hello",
    "defaultValue" : "World",
    "$ref": "#/$defs/positiveInteger"
}

// defs
{
    "$defs": {
        "positiveInteger": {
            "type": "integer",
            "exclusiveMinimum": 0,
            "defaultValue" : "Hello"
        }
    }
}

Copies the values from defs which are not in the original object
{
    "items": {
        "name" : "Hello",
        "defaultValue" : "World",
        "type": "integer",
        "exclusiveMinimum": 0
    }
}
*/
inline void jsonExpandAllReferences(json & J, json const & defs, std::string ref= "$ref");
inline void jsonExpandAllReferences(json & J, std::string ref= "$ref");

/**
 * @brief jsonFindPath
 * @param path
 * @param obj
 * @return
 *
 * Returns a pointer to a json object that exists in obj based on its path.
 *
 * eg:
 * a path looks like: "objName/array/0"
 *
 * {
 *    "objName" : {
 *        "array" : ["first" , "second" ]
 *    }
 * }
 */
inline json * jsonFindPath(std::string_view const path, json & obj);
inline json const* jsonFindPath(std::string_view const path, json const & obj);


/**
 * @brief doIfKeyExists
 * @param K
 * @param J
 * @param C
 *
 * Executes the callable if a key, K, exists in J
 */
template<typename jsonObject, typename Callable_type>
bool doIfKeyExists(json::object_t::key_type const &K, jsonObject & J, Callable_type && C)
{
    auto it = J.find(K);
    if(it == J.end())
        return false;
    C(*it);
    return true;
}


template<typename ValueType>
ValueType _JValue( json const *it, const ValueType& default_value)
{
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
        else
        {
            // likely an array
            if(it->is_array())
            {
                //using value_type = typename ValueType::value_type;
                return it->get<ValueType>();
            }
            return default_value;
        }
    }
}

/**
 * @brief JValue
 * @param J
 * @param key
 * @param default_value
 * @return
 *
 * Returns the ValueType, if the key, exists in J and is the same type as ValueType, If not,
 * returns the default value
 */
template<typename ValueType>
ValueType JValue(json const & J, json::object_t::key_type const & key, const ValueType& default_value)
{
    auto it = J.find(key);
    if(it == J.end())
        return default_value;

    return _JValue<ValueType>( &*it, default_value);
}


/**
 * @brief JValue
 * @param J
 * @param index
 * @param default_value
 * @return
 *
 * Given a json array, J, return element, index. If element does not exist or is
 * an incorrect type, returns default_value
 */
template<typename ValueType>
ValueType JValue(json const & J, json::array_t::size_type index, const ValueType& default_value)
{
    if(!J.is_array())
        return default_value;

    if(index >= J.size())
    {
        return default_value;
    }
    auto it = &J[index];
    return _JValue<ValueType>(it, default_value);
}


template<typename T>
inline T * _jsonFindPath(std::string_view const path, T & obj);

/**
 * @brief jsonFindPath
 * @param path
 * @param obj
 * @return
 *
 * Given a path in a json object. Returns a pointer to the json object
 */
inline json const* jsonFindPath(std::string_view const path, json const & obj)
{
    return _jsonFindPath<json const>(path, obj);
}
inline json * jsonFindPath(std::string_view const path, json & obj)
{
    return _jsonFindPath<json>(path, obj);
}



//=============== Private Functions ======================

/**
 * @brief jsonExpandDefs
 * @param J
 * @param defs
 * @param ref
 *
 * Expands any ref definitions in J.
 * J[ref] can be either a string in the form "#/path/to/object/in/def"
 * or an array of strings in that same form.
 *
 * When this function returns, J[ref] will be erased and J will be
 * the merged value of all references
 */
inline void _jsonExpandReference(json & J, json const & defs, std::string ref = "$ref")
{
    struct _local
    {
        static void _findRefsRecursive(ImJSchema::json const& J,
                                       ImJSchema::json const& defs,
                                       std::string const & ref,
                                       std::vector<std::string> & _refs)
        {
            auto ref_it = J.find(ref);
            if(ref_it == J.end())
                return;

            std::vector<std::string> listOfRefs;
            if(ref_it->is_string())
                listOfRefs.push_back( ref_it->get<std::string>() );
            if(ref_it->is_array())
            {
                for(auto & a : *ref_it)
                {
                    if(a.is_string())
                        listOfRefs.push_back( a.get<std::string>() );
                }
            }

            for(auto & path : listOfRefs)
            {
                //auto & path = ref_it->get_ref<std::string const&>();
                std::string_view path_view(path);
                if(path_view.front() == '#')
                {
                    path_view = path_view.substr(1);
                }
                if(path_view.front() == '/')
                {
                    path_view = path_view.substr(1);
                }
                _refs.push_back( std::string(path_view));
                auto def_it = ImJSchema::jsonFindPath(path_view, defs);
                if(def_it && def_it->is_object())
                {
                    _findRefsRecursive(*def_it,defs, ref, _refs);
                }
            }
        }
    };

    std::vector<std::string> _refs;
    _local::_findRefsRecursive(J, defs, ref, _refs);

    {
        json final;
        // merge everything
        // for all _refs to be merged, they all have to be objects.
        while(_refs.size())
        {
            auto p = jsonFindPath(_refs.back(), defs);
            if(p)
            {
                if(p && p->is_object())
                {
                    final.merge_patch(*p);
                }
                else
                {
                    final = *p;
                    J = final;
                    return;
                }
            }
            _refs.pop_back();
        }
        final.merge_patch(J);
        final.erase("$ref");
        J = final;
    }
}


/**
 * @brief jsonExpandAllReferences
 * @param J
 * @param defs
 * @param ref
 *
 * Recursive fuction which expands ALL definitions
 */
inline void jsonExpandAllReferences(json & J, json const & defs, std::string ref)
{
    if(J.is_array())
    {
        for(auto & i : J)
        {
            jsonExpandAllReferences(i, defs, ref);
        }
    }
    if(J.is_object())
    {
        _jsonExpandReference(J, defs, ref);
        for(auto [key, value] : J.items())
        {
            jsonExpandAllReferences(value, defs, ref);
        }
    }
}

inline void jsonExpandAllReferences(json & J, std::string ref)
{
    jsonExpandAllReferences(J, J, ref);
}





template<typename T>
inline T * _jsonFindPath(std::string_view const path, T & obj)
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
            size_t i3 = {};
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
            size_t i3 = {};
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

/**
 * @brief initialize
 * @param value
 * @param schema
 *
 * Initialize a json value according to the json schema
 *
 * For boolean, number, integer, string, if
 * the "default" value defined,
 * the value will be set to the default. If it is not
 * it will be initialized to zero.
 *
 * For arrays and objects, if the "default" value is defined,
 * it will be set to that value. If not, it will recurively go
 * through each of the child properties and call initialize on it
 */
inline void initializeToDefaults(json & value, json const & schema)
{
    auto type = JValue(schema, "type", std::string());
    if(type == "object")
    {
        // a default value for the object was
        // defined
        if(!value.is_object())
        {
            value = json::object_t();
            auto it = schema.find("default");
            if(it != schema.end())
            {
                value = *it;
            }
        }

        auto properties_it = schema.find("properties");
        if(properties_it == schema.end())
            return;

        // if the required property is NOT found, then
        // all properties are required
        auto required_it = schema.find("required");
        if(required_it != schema.end() && required_it->is_array())
        {
            // if the required property is found, then only the
            // items listed in the array are required, the rest are
            // optional
            for(auto & propertyName : *required_it)
            {
                initializeToDefaults( value[propertyName], properties_it->at(propertyName));
            }
        }
        else
        {
            for(auto & [propertyName, propertySchema] : properties_it->items())
            {
                initializeToDefaults( value[propertyName], propertySchema);
            }
        }

    }
    else if(type == "array" )
    {
        // its not already an array,
        // so convert it into an array
        if(!value.is_array())
        {
            auto it = schema.find("default");
            if(it != schema.end())
            {
                value = *it;
                return;
            }
            else
            {
                value = json::array_t();
            }
        }

        // No default value, so we should check if
        // there is a minimum number of items needed
        auto minItems = JValue(schema, "minItems", size_t(0u));
        auto maxItems = JValue(schema, "maxItems", std::numeric_limits<size_t>::max());

        auto & _arr = value.get_ref<json::array_t&>();
        _arr.resize( std::clamp(value.size(), minItems, maxItems));

        auto it = schema.find("items");
        for(auto & a : _arr)
        {
            initializeToDefaults(a, *it);
        }
    }
    else if(type == "boolean")
    {
        if(!value.is_boolean())
        {
            auto it = schema.find("default");
            if(it == schema.end() || !it->is_boolean() )
            {
                value = false;
                return;
            }
            value = *it;
        }
    }
    else if(type == "number")
    {
        if(!value.is_number())
        {
            auto it = schema.find("default");
            if(it == schema.end() || !it->is_number() )
            {
                value = 0.0f;
                return;
            }
            value = *it;
        }
    }
    else if(type == "integer")
    {
        if(!value.is_number_integer())
        {
            auto it = schema.find("default");
            if(it == schema.end() || !it->is_number_integer() )
            {
                value = 0;
                return;
            }
            value = *it;
        }
    }
    else if(type == "string")
    {
        if(!value.is_string())
        {
            auto it = schema.find("default");
            if(it == schema.end() || !it->is_string() )
            {
                value = "";
                return;
            }
            value = *it;
        }
    }
}

}


#endif

