# ImJSchema 

An ImGui library to build Form UIs from Json Schemas. Based on [React JSON Schema Forms](https://rjsf-team.github.io/react-jsonschema-form/).


## Dependencies

* ImGui
* nlohmann::json

## Usage 

There is only one function that you really need to use. the `drawSchemaWidget`. 
It requires 3 json objects:


```c++
namespace IJS = ImJSchema;

// the schema which that you wish to draw
const auto schema = IJS::json::parse(R"foo(
            {
                "type": "number"
            })foo");


// The value of the widgets will be stored
// in this json object
static ISJ::json value = {};

// The cache used for storing temporary widget data
// such as which index a dropdown menu is currently using
static ISJ::json cache = {};

if(IJS::drawSchemaWidget("object",
                         value,
                         schema,
                         cache))
{
    std::cout << value.dump(4) << std::endl;
}
```

## Examples 

See [examples.cpp](example.cpp).

## Screen Shots

### Booleans
![Boolean](img/booleans.png)

### Numbers
![Numbers](img/numbers.png)

### Strings
![Strings](img/strings.png)

### Enumerated Types
![Strings](img/enums.png)

### Arrays
![Strings](img/arrays.png)


## References and Definitions

The specification for Json Schemas support references in the form of:

```json
{
    "myproperty" : {
        "$ref" : "#/$defs/mydefinition"
    },
    "$defs" : {
        "mydefinition" : {
            "type" : "number"
        }
    }
}
```

This library doesn't support references directly, but helper functions are provided to 
expand a json object which contains references.

```c++
auto schema = IJS::json::parse( R"foo(
{
    "myproperty" : {
        "$ref" : "#/$defs/mydefinition"
    },
    "$defs" : {
        "mydefinition" : {
            "type" : "number"
        }
    }
}
)foo");
IJS::jsonExpandAllReferences(schema);

// or if $defs is in another object:
IJS::jsonExpandAllReferences(schema, definitionsObject);

// you can now call the draw widget function
if(IJS::drawSchemaWidget("object",
                            value,
                            schema,
                            cache))
{
    std::cout << value.dump(4) << std::endl;
}
```