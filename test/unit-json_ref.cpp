#include <catch2/catch_all.hpp>

#include <iostream>

#include "ImJSchema/detail/json_utils.h"


auto JRefValue(ImJSchema::json const & object,
               ImJSchema::json::object_t::key_type const & key,
               ImJSchema::json const & def)
{
    auto val_it = object.find(key);

    // the key was found in the object's property
    if(val_it != object.end())
    {
        return std::make_pair(val_it, object.end());
    }

    auto ref_it = object.find("$ref");
    if(ref_it != object.end())
    {
        if(ref_it->is_string())
        {
            ImJSchema::json::json_pointer p(ref_it->get<std::string>());

            if(def.contains(p))
            {
                return JRefValue( def.at(p), key, def);
            }
        }
    }

    return std::make_pair(object.end(), object.end());
}



TEST_CASE("$ref")
{
    using namespace ImJSchema;
    json J;

    json def = json::parse(R"foo(
{
    "referenced" : {
        "value" : true
    },
    "ref_value" : 3.14,
    "parent" : {
        "$ref" : "/referenced"
    },
    "child" : {
        "$ref" : "/parent"
    },
    "value" : {
        "$ref" : "/ref_value"
    }
}
)foo");


    {
        ImJSchema::json::json_pointer ptr("/referenced");
        REQUIRE(def.contains(ptr));
    }
    {
        ImJSchema::json::json_pointer ptr("/referenced/value");
        REQUIRE(def.contains(ptr));
    }

    {
        auto [it, end] = JRefValue(def["parent"], "value", def);
        REQUIRE(it != end);
        REQUIRE(it->is_boolean());
        REQUIRE(*it == true);
    }
    {
        auto [it, end] = JRefValue(def["child"], "value", def);
        REQUIRE(it != end);
        REQUIRE(it->is_boolean());
        REQUIRE(*it == true);
    }

}
