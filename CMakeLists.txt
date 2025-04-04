cmake_minimum_required(VERSION 3.10)



################################################################################
# The Project Name will the the name of the folder. Change it if you
# dont like it.
################################################################################
project( ImJSchema  CXX )
################################################################################

if(PROJECT_IS_TOP_LEVEL)
    set(IMJSCHEMA_BUILD_EXECUTABLES ON  CACHE FILEPATH "Build the sample application")
else()
    set(IMJSCHEMA_BUILD_EXECUTABLES OFF CACHE FILEPATH "Build the sample application")
endif()


################################################################################
# Build the Interface library
#
################################################################################
add_library(ImJSchema INTERFACE)
add_library(ImJSchema::ImJSchema ALIAS ImJSchema)

target_include_directories(ImJSchema
                            INTERFACE
                               "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
)

target_link_libraries(ImJSchema
                        INTERFACE
                            )
target_compile_features( ImJSchema
                          INTERFACE
                              cxx_std_17)

################################################################################



################################################################################
if(IMJSCHEMA_BUILD_EXECUTABLES)

    find_package(nlohmann_json REQUIRED)
    find_package(imgui REQUIRED)

    ################################################################################

    include(cmake/CompilerWarnings.cmake)


    if( "${CC_FILE_NAME}" MATCHES "emcc" OR "${CXX_FILE_NAME}" MATCHES "emcc" OR EMSCRIPTEN)
        message("Compiling with emc")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -s USE_SDL=2")
        set(CMAKE_EXECUTABLE_SUFFIX_CXX ".html")
        set(SDL_TARGET "")
    else()

        find_package(SDL2 REQUIRED)
        set(SDL_TARGET SDL2::SDL2)

    endif()

    ################################################################################
    # Build the executable
    #
    ################################################################################
    add_executable( main main.cpp )
    target_include_directories( main
                                   SYSTEM PRIVATE
                                   "${imgui_INCLUDE_DIRS}"
                                    ${CMAKE_BINARY_DIR}/imgui_src/include
                                    ${CMAKE_BINARY_DIR}/imgui_src/res/bindings
                                    ${CMAKE_BINARY_DIR}/imgui_src/res/src
                                    ${CMAKE_BINARY_DIR}/imgui_src/res/misc/cpp)
    target_link_libraries( main
                            PRIVATE
                                ImJSchema::ImJSchema
                                nlohmann_json::nlohmann_json
                                ${SDL_TARGET}
                                ImJSchema::warnings
                                ImJSchema::error
                            )

    ################################################################################


    if(${EMSCRIPTEN})
        file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/index.html DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

        set_target_properties(main PROPERTIES
            OUTPUT_NAME "main"
            SUFFIX ".html"  # Generates an HTML wrapper
        )
    else()
        enable_testing()
        add_subdirectory(test)
    endif()


endif()

