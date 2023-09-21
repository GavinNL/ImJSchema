cmake_minimum_required(VERSION 3.10)



################################################################################
# The Project Name will the the name of the folder. Change it if you
# dont like it.
################################################################################
project( ImJSchema  CXX )
################################################################################

if(PROJECT_IS_TOP_LEVEL)
    set(IMJSCHEMA_BUILD_EXECUTABLES ON  CACHE FILEPATH "description")
else()
    set(IMJSCHEMA_BUILD_EXECUTABLES OFF CACHE FILEPATH "description")
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


include(cmake/CompilerWarnings.cmake)

myproject_set_project_warnings(ImJSchema TRUE "" "" "" "")
################################################################################
if( IMJSCHEMA_BUILD_EXECUTABLES)

    find_package(SDL2 REQUIRED)
    find_package(nlohmann_json REQUIRED)

    ################################################################################


    ################################################################################
    # Build the executable
    #
    ################################################################################
    set(outName              example)   # name of the library
    set(srcFiles             example.cpp     )          # all the source files for this library
    set(PublicLinkedTargets  ""             )
    set(PrivateLinkedTargets  ImJSchema::ImJSchema SDL2::SDL2 nlohmann_json::nlohmann_json)

    #-------------------------------------------------------------------------------
    add_executable( ${outName} ${srcFiles} )

    target_include_directories( ${outName}
                                PUBLIC
                                   "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/third_party/imgui>"
                                   "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/third_party/imgui/backends>"
                               )

    target_link_libraries( ${outName}  PUBLIC  ${PublicLinkedTargets}  )
    target_link_libraries( ${outName}  PRIVATE ${PrivateLinkedTargets} )

    target_include_directories( ${outName} PUBLIC third_party/imgui   )
    ################################################################################
    add_subdirectory(test)

endif()
