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
inline size_t jsonExpandDef(json & J, json const & defsRoot, std::string ref = "$ref")
{
    auto it = J.find(ref);
    if(it == J.end())
    {
        return 0;
    }
    if(!it->is_string())
    {
        return 0;
    }

    std::string & path = it->get_ref<std::string&>();
    if(path.empty())
    {
        return 1;
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
        return 1;

    size_t retVal = 0;
    json d = *def_it;

    if(d.is_object())
    {
        d.merge_patch(J);

        // only erase the reference if the
        // ref doesn't exist in the definition
        d.erase(ref);
        retVal = 0;
        if(def_it->contains(ref))
        {
            d[ref] = def_it->at(ref);
            retVal = 1;
        }
    }
    J = std::move(d);
    return retVal;
}


/**
 * @brief jsonExpandAllDefs
 * @param J
 * @param defsRoot
 * @param ref
 *
 * The same as the jsonExpandDef, but does it recursively for all objects/arrays
 *
 * This function only does one pass of the json document. If a definition references
 * another reference, it is not expanded.
 *
 * Returns the number of references that still occur in after the pass. You could
 *
 * You could put this in a loop, but beware that this could explode if you have
 * circular references
 *
 * while( jsonExpandAllDefs(J, defs) != 0)
 */
inline size_t jsonExpandAllDefs(json & J, json const & defsRoot, std::string ref="$ref")
{
    size_t count = 0;
    if(J.is_array())
    {
        for(auto & i : J)
        {
            count += jsonExpandAllDefs(i, defsRoot, ref);
        }
    }
    if(J.is_object())
    {
        count += jsonExpandDef(J, defsRoot, ref);
        for(auto [key, value] : J.items())
        {
            count += jsonExpandAllDefs(value, defsRoot, ref);
        }
    }
    return count;
}



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
void jsonExpandReference(json & J, json const & defs, std::string ref = "$ref")
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
        // for all _refs to be merged, they all ahve to be objects.
        while(_refs.size())
        {
            auto p = jsonFindPath(_refs.back(), defs);
            if(p && p->is_object())
            {
                J.merge_patch(*p);
            }
            else
            {
                J = *p;
                return;
                break;
            }
            _refs.pop_back();
        }
        J.merge_patch(J);
        J.erase("$ref");
    }
}

void jsonExpandAllReferences(json & J, json const & defs, std::string ref= "$ref")
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
        jsonExpandReference(J, defs, ref);
        for(auto [key, value] : J.items())
        {
            jsonExpandAllReferences(value, defs, ref);
        }
    }
}

void jsonExpandAllReferences(json & J, std::string ref= "$ref")
{
    jsonExpandAllReferences(J, J, ref);
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

