#include <catch2/catch_all.hpp>

#include <iostream>
#include <string>
#include <regex>


#include "ImJSchema/json_utils.h"

TEST_CASE("jsonFindPath")
{
    using namespace ImJSchema;
    json J;
    J["grandparent"]["parent"]["child"] = 34;
    J["grandparent"]["array"][4]["child"] = 25;

    std::cout << J.dump(4) << std::endl;
    {
        auto p = jsonFindPath("grandparent/parent/child", J);
        REQUIRE(p != nullptr);
        REQUIRE(*p == 34);
    }
    {
        auto p = jsonFindPath("grandparent/parent/sibling", J);
        REQUIRE(p == nullptr);
    }

    // finds an array
    {
        auto p = jsonFindPath("grandparent/array/4/child", J);
        REQUIRE(p != nullptr);
        REQUIRE(*p == 25);
    }
    // we didn't set this value, but it was defaulted
    {
        auto p = jsonFindPath("grandparent/array/3", J);
        REQUIRE(p != nullptr);
        REQUIRE(p->is_null());
    }
    {
        auto p = jsonFindPath("grandparent/array/7", J);
        REQUIRE(p == nullptr);
    }
    {
        auto p = jsonFindPath("grandparent/you/dont/exist/323", J);
        REQUIRE(p == nullptr);
    }
}

TEST_CASE("jsonExpandDef")
{
    using namespace ImJSchema;
    json J;

    auto prop = json::parse(R"foo(
    {
        "name" : "Hello",
        "$ref": "#/$defs/positiveInteger"
    })foo");

    auto defsRoot = json::parse(R"foo(
    {
        "$defs": {
            "positiveInteger": {
                "type": "integer",
                "exclusiveMinimum": 0
            }
        }
    })foo");

    json final = json::parse(R"foo({
    "exclusiveMinimum": 0,
    "name": "Hello",
    "type": "integer"
})foo");
    jsonExpandDef(prop, defsRoot);
    std::cout << prop.dump(4) << std::endl;
    REQUIRE( prop.dump(4) == final.dump(4));

}

TEST_CASE("jsonExpandDef with array")
{
    using namespace ImJSchema;
    json J;

    auto prop = json::parse(R"foo(
    {
        "name" : "Hello",
        "$ref": "#/$defs/positiveInteger"
    })foo");

    auto defsRoot = json::parse(R"foo(
    {
        "$defs": {
            "positiveInteger": [2,3,4,5,6]
        }
    })foo");

    json final = json::parse(R"foo({
    "exclusiveMinimum": 0,
    "name": "Hello",
    "type": "integer"
})foo");

    jsonExpandDef(prop, defsRoot);
    std::cout << prop.dump(4) << std::endl;
    REQUIRE( prop.dump(4) == defsRoot["$defs"]["positiveInteger"].dump(4));

}

TEST_CASE("jsonExpandAllDefs")
{
    using namespace ImJSchema;
    json J;

    auto prop = json::parse(R"foo(
    {
        "items": {
            "$ref": "#/$defs/positiveInteger"
        },
        "item2": [
            {
                "$ref": "#/$defs/positiveInteger"
            },
            3,
            "hello world"
        ]
    })foo");

    auto defsRoot = json::parse(R"foo(
    {
        "$defs": {
            "positiveInteger": {
                "type": "integer",
                "exclusiveMinimum": 0
            }
        }
    })foo");

    json final2 =
    {
        {"item2", {defsRoot["$defs"]["positiveInteger"],3, "hello world"} },
        {"items", defsRoot["$defs"]["positiveInteger"] }
    };
    jsonExpandAllDefs(prop, defsRoot);

    REQUIRE(prop.dump(4) == final2.dump(4));
}

