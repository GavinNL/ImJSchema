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

  //  std::cout << J.dump(4) << std::endl;
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

TEST_CASE("json merge patch")
{
    using namespace ImJSchema;
    json J;

    auto prop = json::parse(R"foo(
    {
        "name" : "bob",
        "item1": {
            "x" : "hello"
        }
    })foo");

    auto defsRoot = json::parse(R"foo(
    {
        "name" : "bob",
        "lastName" : "Brker",
        "item1": {
            "y" : "world"
        }
    })foo");


    prop.merge_patch(defsRoot);

    std::cout << prop.dump(4) << std::endl;;
}



TEST_CASE("Find Reference list")
{
    using namespace ImJSchema;

    auto J = json::parse(R"foo(
    {
        "$ref" : ["#/$defs/A"]
    })foo");
    auto defs = json::parse(R"foo(
    {
        "$defs" : {
            "A" : {
                "$ref" : ["#/$defs/B", "#/$defs/C"],
                "a_value" : true
            },
            "B" : {
                "$ref" : "#/$defs/D",
                "b_value" : true,
                "d_value" : false
            },
            "C" : {
                "c_value" : true,
                "b_value" : false
            },
            "D" : {
                "d_value" : true
            }
        }
    })foo");

    _jsonExpandReference(J, defs);

    json final;
    final["a_value"] = true;
    final["b_value"] = true;
    final["c_value"] = true;
    final["d_value"] = false;

    std::cout << J.dump(4) << std::endl;;
    REQUIRE( J.dump(4) == final.dump(4));
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
   // std::cout << prop.dump(4) << std::endl;
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
    //std::cout << prop.dump(4) << std::endl;
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


TEST_CASE("jsonExpandAllDefs2")
{
    using namespace ImJSchema;
    json J;

    auto prop = json::parse(R"foo(
    {
        "item1": {
            "$ref": "#/$defs/reference_1"
        },
        "item2": {
            "$ref": "#/$defs/reference_2"
        }
    })foo");

    auto defsRoot = json::parse(R"foo(
    {
        "$defs" : {
            "reference_1" : {
                "x" : true,
                "y" : 2.3
            },
            "reference_2" : {
                "$ref" : "#/$defs/reference_1",
                "name" : "reference_2"
            }
        }
    })foo");


    {
        auto totalRefs = jsonExpandAllDefs(prop, defsRoot);
        REQUIRE(totalRefs == 1);
    }


    {
        auto totalRefs = jsonExpandAllDefs(prop, defsRoot);
        REQUIRE(totalRefs == 0);
    }

    {
        json final2;
        final2["item1"] = defsRoot["$defs"]["reference_1"];
        final2["item2"] = defsRoot["$defs"]["reference_1"];
        final2["item2"]["name"] = "reference_2";
        REQUIRE(prop.dump(4) == final2.dump(4));
    }
    std::cout << prop.dump(4);
}


TEST_CASE("jsonExpandAllDefs3 - check that each type of JSON property can be referenced")
{
    using namespace ImJSchema;
    json J;

    auto prop = json::parse(R"foo(
    {
        "string": {
            "$ref": "#/$defs/string"
        },
        "number": {
            "$ref": "#/$defs/number"
        },
        "boolean": {
            "$ref": "#/$defs/boolean"
        },
        "array": {
            "$ref": "#/$defs/array"
        },
        "object": {
            "$ref": "#/$defs/object"
        }
    })foo");

    auto defsRoot = json::parse(R"foo(
    {
        "$defs" : {
            "string" : "Hello",
            "number" : 3,
            "boolean" : true,
            "array" : [1,2,3],
            "object" : {
                 "test": "hello world"
            }
        }
    })foo");


    jsonExpandAllReferences(prop, defsRoot);

    {
        REQUIRE(prop.dump(4) == defsRoot["$defs"].dump(4));
    }
    std::cout << prop.dump(4);
}



TEST_CASE("jsonExpandAllDefs3 - check that each type of JSON property can be referenced asd")
{
    using namespace ImJSchema;
    json J;

    auto prop = json::parse(R"foo(
    {
        "$defs" : {
            "class_list" : ["Wizard", "Sorcerer", "Barbarian"],
            "party_member" : {
                "type" : "object",
                "properties" : {
                    "class" : {
                        "enumNames" : { "$ref" : "#/$defs/class_list" },
                        "enum" : { "$ref" : "#/$defs/class_list" },
                        "type" : "string"
                    }
                }
            }
        },
        "type" : "object",
        "properties" : {
            "member" : { "$ref" : "#/$defs/party_member"}
        }
    })foo");


    jsonExpandAllReferences(prop, prop);

    std::cout << prop.dump(4);
}
