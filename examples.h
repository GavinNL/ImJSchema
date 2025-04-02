#ifndef IMJSCHEMA_EXAMPLES_H
#define IMJSCHEMA_EXAMPLES_H

constexpr auto basic_number =
    R"foo({
    "type": "number"
})foo";

constexpr auto basic_boolean =
    R"foo({
    "type": "boolean"
})foo";

constexpr auto basic_string =
    R"foo({
    "type": "string"
})foo";

constexpr auto basic_object =
    R"foo({
    "type": "object",
    "properties" : {
        "number" : { "type" : "number"},
        "bool"   : { "type" : "boolean"},
        "string" : { "type" : "string"}
    }
})foo";

constexpr auto basic_array = R"foo({
    "type": "array",
    "items" : {
        "type" : "number"
    }
})foo";


constexpr auto descriptions = R"foo({
    "description" : "Each schema object can have a \"description\" property to display visible text",
    "type": "object",
    "ui:order" : ["name", "age", "imguiAwesome", "object"],
    "properties" : {
        "name" : {
            "type" : "string",
            "description" : "Please enter the name wish to be called"
        },
        "age" : {
            "type" : "number",
            "description" : "The age you will be at the end of this year"
        },
        "imguiAwesome" : {
            "type" : "boolean",
            "description" : "Check this box if you think ImGui is awesome."
        },
        "object" : {
            "type" : "object",
            "description" : "This is an object description. It appears at the top.",
            "properties" : {
                 "number" : { "type" : "number"}
            }
        }
    }
})foo";


constexpr auto ordering =
    R"foo({
    "type": "object",
    "description" : "By default, properties in an object will be displayed in alphabetical order. You can set the order the widgets appear by setting the ui:order property. If you do not list the widget, it will show up at the end.",
    "ui:order" : ["b", "c", "a"],
    "properties": {
        "a": { "type": "string", "ui:help" : "Hover over the label to show this tooltip"},
        "b": { "type": "string" },
        "c": { "type": "string" },
        "d": { "type": "string" }
    }
})foo";

constexpr auto titles =
    R"foo({
    "description" : "Each schema object can have a \"title\". This value will show up as the label ",
    "type": "object",
    "ui:order" : ["name", "age", "imguiAwesome", "array", "object"],
    "properties" : {
        "name" : {
            "type" : "string",
            "title" : "Name"
        },
        "age" : {
            "type" : "number",
            "title" : "Age"
        },
        "imguiAwesome" : {
            "type" : "boolean",
            "title" : "Is ImGui Awesome?"
        },
        "object" : {
            "type" : "object",
            "title" : "Super Special Object",
            "properties" : {
                 "number" : { "type" : "number"}
            }
        },
        "array" : {
            "type" : "array",
            "title" : "Super Special Array",
            "items" : {
                 "type" : "number"
            }
        }
    }
})foo";

constexpr auto help =
    R"foo({
    "description" : "You can use the \"ui:help\" : \"tool tip text\" to show tooltip. Hover over the object label",
    "type": "object",
    "ui:order" : ["name", "age", "imguiAwesome", "object"],
    "properties" : {
        "name" : {
            "type" : "string",
            "title" : "Name",
            "ui:help" : "Please enter the name wish to be called"
        },
        "age" : {
            "type" : "number",
            "title" : "Age",
            "ui:help" : "The age you will be at the end of this year"
        },
        "imguiAwesome" : {
            "type" : "boolean",
            "title" : "Is ImGui Awesome?",
            "ui:help" : "Check this box if you think ImGui is awesome."
        },
        "object" : {
            "type" : "object",
            "ui:help" : "This is an object",
            "properties" : {
                 "number" : { "type" : "number"}
            }
        }
    }
})foo";

constexpr auto number_widgets =
    R"foo({
    "type": "object",
    "properties": {
        "float": {
            "type": "number",
            "default": 0.0,
            "ui:step": 1,
            "ui:step_fast": 10,
            "description" : "Default floating point number. if ui:step is not provided, the increment buttons will not show. Default ui:step_fast is 10x ui:step"
        },
        "float_slider": {
            "type": "number",
            "default": 0.0,
            "maximum": 10.0,
            "minimum": 0.0,
            "ui:widget": "slider",
            "description" : "Slider widgets require both a minimum and maximum property to be set. Will default to drag widget if either of these are not provided"
        },
        "float_drag": {
            "type": "number",
            "default": 0.0,
            "maximum": 1.0,
            "minimum": 0.0,
            "ui:speed": 0.0010000000474974513,
            "ui:widget": "drag",
            "description" : "Drag widgets"
        },
        "int": {
            "type": "integer",
            "default": 0,
            "ui:step": 1,
            "ui:step_fast": 10,
            "description" : "Integer widgets can be used as well, by setting the type to \"integer\""
        },
        "int_drag": {
            "type": "integer",
            "default": 0,
            "maximum": 10,
            "minimum": 0,
            "ui:step": 1,
            "ui:step_fast": 10,
            "ui:widget": "drag"
        },
        "int_slider": {
            "type": "integer",
            "default": 0,
            "maximum": 10,
            "minimum": 0,
            "ui:step": 1,
            "ui:step_fast": 10,
            "ui:widget": "slider"
        }
    }
})foo";

constexpr auto boolean_widgets =
    R"foo({
    "description" : "Boolean widgets. This object makes use of the \"ui:column_size\" property.",
    "ui:column_size" : 50,
    "type": "object",
    "properties": {
        "checkbox": {
            "type": "boolean",
            "default": false,
            "title" : "Check Box",
            "description" : "The default is checkbox"
        },
        "enabledisable": {
            "type": "boolean",
            "default": false,
            "ui:widget": "enabledisable",
            "title" : "Enable/Disable",
            "description" : "But there are also different styles. See Enumerated Types for an alternative method"
        },
        "truefalse": {
            "type": "boolean",
            "default": false,
            "ui:widget": "truefalse",
            "title" : "True/False"
        },
        "yesno": {
            "type": "boolean",
            "default": true,
            "ui:widget": "yesno",
            "title" : "Yes/No"
        }
    }
})foo";

constexpr auto string_widgets =
    R"foo({
    "type": "object",
    "ui:order" : ["basic", "textarea", "color", "color_picker"],
    "properties": {
        "basic": {
            "type": "string"
        },
        "color": {
            "type": "string",
            "ui:widget": "color"
        },
        "color_picker": {
            "type": "string",
            "ui:widget": "color_picker"
        },
        "textarea": {
            "type": "string",
            "ui:options": {
                "rows": 5
            },
            "ui:widget": "textarea"
        }
    }
})foo";

constexpr auto array_widgets =
    R"foo({
    "type": "object",
    "ui:order" : ["fixed_length", "varying_size", "color_array", "color_array_alpha", "vec3"],
    "properties": {
        "fixed_length": {
            "type": "array",
            "items" : {
                "type": "string",
                "default": "hello world"
            },
            "minItems" : 3,
            "maxItems" : 3,
            "title" : "Fixed Length Array",
            "description" : "You can set the number of items in the array using the minItems and maxItems properties"
        },
        "varying_size": {
            "type": "array",
            "items" : {
                "type": "string",
                "default": "hello world"
            },
            "title" : "Varying Length Array",
            "description" : "If you dont set the minItems/maxItems, you can grow or shrink the array using the buttons."
        },
        "color_array": {
            "type": "array",
            "items" : {
                "type": "number"
            },
            "default" : [1,0.5,0.7],
            "maxItems" : 3,
            "minItems" : 3,
            "ui:widget" : "color",
            "title" : "Color Array",
            "description" : "Setting items == number, will give values ranging from 0-1. If minItems == maxItems == 3, then you will get a RGB color picker."
        },
        "color_array_alpha": {
            "type": "array",
            "items" : {
                "type": "integer"
            },
            "default" : [128, 200, 96, 255],
            "maxItems" : 4,
            "minItems" : 4,
            "ui:widget" : "color",
            "title" : "Color Array Alpha",
            "description" : "Setting items == integer, will give values ranging from 0-255. If minItems == maxItems == 4, then you will get a RGBA color picker."
        },
        "vec3": {
            "type": "array",
            "items" : {
                "type": "number"
            },
            "minItems" : 3,
            "maxItems" : 3,
            "default" : [128.0, 200.0, 96.0],
            "ui:widget" : "vec",
            "title" : "Vector3",
            "description" : "Items are placed horizontally with labels X, Y, Z, W depending on whether 2 <= minItems <= 4"
        }
    }
})foo";


constexpr auto object_widgets =
    R"foo({
    "type": "object",
    "ui:order" : ["regular", "header", "collapsing"],
    "properties": {
        "regular": {
            "type": "object",
            "title" : "Regular Object",
            "properties" :
            {
                "string" : { "type" : "string" },
                "number" : { "type" : "number" },
                "boolean" : { "type" : "boolean" }
            },
            "description" : "This is a regular object type. If ui:resizable is true, then you will be able to resize the label field"
        },
        "header": {
            "type": "object",
            "title" : "Header Widget",
            "ui:widget" : "header",
            "properties" :
            {
                "string" : { "type" : "string" },
                "number" : { "type" : "number" },
                "boolean" : { "type" : "boolean" }
            },
            "description" : "Objects with ui:widget=header, will show up as headers"
        },
        "collapsing": {
            "type": "object",
            "title" : "Collapsing Widget",
            "ui:widget" : "collapsing",
            "properties" :
            {
                "string" : { "type" : "string" },
                "number" : { "type" : "number" },
                "boolean" : { "type" : "boolean" }
            },
            "description" : "Objects with ui:widget=collapsing, will show up as collapsable headers"
        }
    }
})foo";



constexpr auto constants =
    R"foo({
    "type": "object",
    "description" : "You can hide constant values in the form by setting the 'default', 'ui:hidden', and 'ui:disabled' properties.",
    "properties": {
        "constant_value": {
            "type": "string",
            "default" : "This is the default value",
            "ui:hidden" : true,
            "ui:disabled" : true
        },
        "b": { "type": "string" },
        "c": { "type": "string" }
    }
})foo";


constexpr auto enumerated_types =
    R"foo({
    "type": "object",
    "description" : "Enumerated types allow you to choose from one option. It requires the \"enum\" property. Additinally, if provided, the \"enumNames\" property will be used for displaying",
    "properties": {
        "strings": {
            "type": "string",
            "enum" : ["wiz", "src", "war"]
        },
        "strings_with_names": {
            "type": "string",
            "title" : "String with Names",
            "default" : "rogue",
            "description" : "When using \"ui:widget\" : \"buttons\", You can set \"ui:options/columns\" to be a number to set the total number of columns for the buttons ",
            "enum" : ["wiz", "sorc", "war", "fight", "barb", "art", "rogue", "monk", "pal"],
            "enumNames" : ["Wizard", "Sorcerer", "Warlock", "Fighter", "Barbarian", "Artificer", "Rogue", "Monk", "Paladin"],
            "ui:widget" : "button",
            "ui:options" : {
               "columns" : 3
            }
        },
        "numbers": {
            "type": "number",
            "title" : "Numbers",
            "enum" : [1,2,3]
        },
        "numbers_with_names": {
            "type": "number",
            "title" : "Numbers With Names",
            "enum" : [1,2,3],
            "enumNames" : ["first", "second", "third"]
        },
        "booleans": {
            "type": "boolean",
            "title" : "boolean",
            "enum" : [false, true]
        },
        "booleans_with_names": {
            "type": "boolean",
            "title" : "Boolean with Names",
            "enum" : [false, true],
            "enumNames" : ["False", "True"]
        }
    }
})foo";

constexpr auto references =
    R"foo({
    "$defs": {
        "number": {
            "default": 0.75,
            "type": "number"
        },
        "normalized": {
            "maximum": 1.0,
            "minimum": 0.0,
            "default": 0.33,
            "ui:widget" : "slider"
        },
        "enum_list" : ["Hello", "this", "is", "a", "list"]
    },
    "description" : "References allow you to reuse parts of your schema.\n\nThe drawSchemaWidget( ) function does not natively support the $ref property.\n\nIf you have a schema object, J,  you can use the jsonExpandAllReferences(J) function\nto expand the references before you pass it into the drawSchemaWidget()",
    "ui:order" : ["single_ref", "multi_ref", "enum_list"],
    "properties" : {
        "single_ref" : {
            "description" : "The $ref string property points to a path within the JSON document",
            "$ref": "#/$defs/number"
        },
        "multi_ref" : {
            "description" : "You can also use multiple references as a string array. Earlier items are overridden by later values in the array",
            "$ref": ["#/$defs/number", "#/$defs/normalized"]
        },
        "enum_list" : {
            "description" : "Any property can set as a reference to a definition",
            "type" : "string",
            "enum" : { "$ref" : "#/$defs/enum_list"}
        }
    },
    "type": "object"
})foo";

constexpr auto DnD =
    R"foo({
    "$defs" : {
        "class_list" : ["Wizard", "Sorcerer", "Warlock", "Fighter", "Barbarian", "Artificer", "Rogue", "Monk", "Paladin"],
        "race_list" : ["Human", "Dwarf", "Elf", "Halfling"],
        "stat" : {
            "type" : "integer",
            "ui:step" : 1,
            "minimum" : 1,
            "maximum" : 20,
            "default" : 10
        },
        "character" : {
            "type" : "object",
            "ui:order" : ["name", "race", "class", "alignment_1", "alignment_2", "stats"],
            "properties" : {
                "name" : {"type" : "string"},
                "race" : {
                    "type" : "string",
                    "enum" : { "$ref" : "#/$defs/race_list" }
                },
                "class" : {
                    "type" : "string",
                    "enum" : { "$ref" : "#/$defs/class_list" }
                },
                "alignment_1" : {
                    "type" : "string",
                    "enum" : ["Lawful", "Neutral", "Chaotic"],
                    "ui:widget" : "button"
                },
                "alignment_2" : {
                    "type" : "string",
                    "enum" : ["Good", "Neutral", "Evil"],
                    "ui:widget" : "button"
                },
                "stats" : {
                    "type" : "object",
                    "properties" : {
                        "str" : { "$ref" : "#/$defs/stat" },
                        "con" : { "$ref" : "#/$defs/stat" },
                        "dex" : { "$ref" : "#/$defs/stat" },
                        "int" : { "$ref" : "#/$defs/stat" },
                        "wis" : { "$ref" : "#/$defs/stat" },
                        "cha" : { "$ref" : "#/$defs/stat" }
                    }
                }
            }
        }
    },
    "type": "object",
    "properties": {
        "party" : {
            "type" : "array",
            "items" : {
                "$ref" : "#/$defs/character"
            }
        }
    }
})foo";

constexpr auto PBR =
    R"foo({
    "$defs": {
        "normalized_number": {
            "type" : "number",
            "maximum": 1.0,
            "minimum": 0.0,
            "default": 0.0,
            "ui:widget" : "slider"
        }
    },
    "ui:order" : ["albedo", "metallic", "roughness", "emissive", "emissiveFactor", "unlit"],
    "properties" : {
        "metallic"  : { "$ref": "#/$defs/normalized_number" },
        "roughness" : { "$ref": "#/$defs/normalized_number" },
        "albedo"    : { "type" : "string", "ui:widget" : "color" },
        "emissive"  : { "type" : "string", "ui:widget" : "color" },
        "emissiveFactor" : {
            "type" : "number",
            "minimum" : 0.0,
            "ui:speed" : 0.01,
            "ui:widget" : "drag"
        },
        "unlit" : {
            "type" : "boolean"
        }
    },
    "type": "object"
})foo";


constexpr auto oneOf = R"foo({
    "description" : "You can use the oneOf property to specify a set of alternatives.",
    "type": "object",
    "oneOf" : [
        {
            "type" : "object",
            "title" : "Sphere",
            "properties" : {
                "_type": {
                    "type": "string",
                    "default" : "sphere",
                    "ui:hidden" : true,
                    "ui:disabled" : true
                },
                "radius" : {
                    "type" : "number"
                }
            }
        },
        {
            "type" : "object",
            "title" : "Box",
            "properties" : {
                "_type": {
                    "type": "string",
                    "default" : "box",
                    "ui:hidden" : true,
                    "ui:disabled" : true
                },
                "length" : {
                    "type" : "number"
                },
                "width" : {
                    "type" : "number"
                },
                "height" : {
                    "type" : "number"
                }
            }
        },
        {
            "type" : "object",
            "title" : "Cone",
            "properties" : {
                "_type": {
                    "type": "string",
                    "default" : "cone",
                    "ui:hidden" : true,
                    "ui:disabled" : true
                },
                "radius" : {
                    "type" : "number"
                },
                "height" : {
                    "type" : "number"
                }
            }
        }
    ]
})foo";


constexpr auto optional_properties = R"foo({
    "type" : "object",
    "title" : "Optional Properties",
    "description" : "The 'required' property is a list of all properties that must be available. If it is empty, then all properties will become optional. If the key does not exist, then all properties are required",
    "required" : [],
    "properties" : {
        "name" : {
            "type" : "string"
        },
        "email" : {
            "type" : "string"
        },
        "phoneNumber" : {
            "type" : "string"
        }
    }
})foo";


constexpr auto custom_widgets =
    R"foo({
    "description" : "See the source code example.cpp",
    "properties" : {
        "custom_number":
        {
            "type" : "number",
            "ui:widget" : "my_custom_number_widget"
        }
    },
    "type": "object"
})foo";


#endif
