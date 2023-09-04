cmake_minimum_required(VERSION 3.10)



################################################################################
# The Project Name will the the name of the folder. Change it if you
# dont like it.
################################################################################
project( ImJSchema  CXX )
################################################################################




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
if(PROJECT_IS_TOP_LEVEL)
    set(SNE_BUILD_EXECUTABLES ON CACHE FILEPATH "description")
else()
    set(SNE_BUILD_EXECUTABLES OFF CACHE FILEPATH "description")
endif()

if( SNE_BUILD_EXECUTABLES)

find_package(SDL2 REQUIRED)
#find_package(fmt REQUIRED)
find_package(nlohmann_json REQUIRED)
################################################################################


################################################################################
# Build the executable
#
################################################################################
set(outName              json_schema_ui)   # name of the library
set(srcFiles             json_schema_ui.cpp     )          # all the source files for this library
set(PublicLinkedTargets  ""             )
set(PrivateLinkedTargets  ImJSchema::ImJSchema SDL2::SDL2 nlohmann_json::nlohmann_json)

#-------------------------------------------------------------------------------
add_executable( ${outName} ${srcFiles} )

target_include_directories( ${outName}
                            PUBLIC
                               "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/third_party/imgui>"
                               "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/third_party/imgui/backends>"
                               "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/third_party/imnodes>"
                           )

target_link_libraries( ${outName}  PUBLIC  ${PublicLinkedTargets}  )
target_link_libraries( ${outName}  PRIVATE ${PrivateLinkedTargets} )

target_include_directories( ${outName} PUBLIC third_party/imgui   )
target_include_directories( ${outName} PUBLIC third_party/imnodes )
target_include_directories( ${outName} PUBLIC include )
################################################################################
add_subdirectory(test)

endif()
