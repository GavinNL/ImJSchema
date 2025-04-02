message("*****************************************************")
message("EXTRA TARGETS:")
message("*****************************************************")

add_library( ${PROJECT_NAME}_coverage INTERFACE)
add_library( ${PROJECT_NAME}::coverage ALIAS ${PROJECT_NAME}_coverage)

add_library(${PROJECT_NAME}_warnings INTERFACE)
add_library(${PROJECT_NAME}::warnings ALIAS ${PROJECT_NAME}_warnings)

add_library(${PROJECT_NAME}_warnings_error INTERFACE)
add_library(${PROJECT_NAME}::error ALIAS ${PROJECT_NAME}_warnings_error)
target_compile_options(${PROJECT_NAME}_warnings_error INTERFACE -Werror)

if(CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX)

    target_compile_options(${PROJECT_NAME}_coverage
                                INTERFACE
                                    --coverage -g -O0 -fprofile-arcs -ftest-coverage)

    target_link_libraries( ${PROJECT_NAME}_coverage
                            INTERFACE --coverage -g -O0 -fprofile-arcs -ftest-coverage)



    add_custom_target(coverage
        COMMAND rm -rf coverage
        COMMAND mkdir -p coverage
        #COMMAND ${CMAKE_MAKE_PROGRAM} test
        #COMMAND gcovr . -r ${CMAKE_SOURCE_DIR} --html-details --html -o coverage/index.html -e ${CMAKE_SOURCE_DIR}/test/third_party;
        COMMAND gcovr . -r ${CMAKE_SOURCE_DIR} --xml -o coverage/report.xml -e ${CMAKE_SOURCE_DIR}/third_party;
        COMMAND gcovr . -r ${CMAKE_SOURCE_DIR} -o coverage/report.txt -e ${CMAKE_SOURCE_DIR}/third_party;
        COMMAND cat coverage/report.txt

        #COMMAND lcov --no-external --capture --directory ${CMAKE_BINARY_DIR} --output-file coverage2.info
        #COMMAND genhtml coverage.info --output-directory lcov-report
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    )

endif()


#==========
# from here:
#
# https://github.com/lefticus/cppbestpractices/blob/master/02-Use_the_Tools_Available.md

function(set_project_warnings project_name)
  option(${PROJECT_NAME}_WARNINGS_AS_ERRORS "Treat compiler warnings as errors" TRUE)

  set(MSVC_WARNINGS
      /W4 # Baseline reasonable warnings
      /w14242 # 'identfier': conversion from 'type1' to 'type1', possible loss
              # of data
      /w14254 # 'operator': conversion from 'type1:field_bits' to
              # 'type2:field_bits', possible loss of data
      /w14263 # 'function': member function does not override any base class
              # virtual member function
      /w14265 # 'classname': class has virtual functions, but destructor is not
              # virtual instances of this class may not be destructed correctly
      /w14287 # 'operator': unsigned/negative constant mismatch
      /we4289 # nonstandard extension used: 'variable': loop control variable
              # declared in the for-loop is used outside the for-loop scope
      /w14296 # 'operator': expression is always 'boolean_value'
      /w14311 # 'variable': pointer truncation from 'type1' to 'type2'
      /w14545 # expression before comma evaluates to a function which is missing
              # an argument list
      /w14546 # function call before comma missing argument list
      /w14547 # 'operator': operator before comma has no effect; expected
              # operator with side-effect
      /w14549 # 'operator': operator before comma has no effect; did you intend
              # 'operator'?
      /w14555 # expression has no effect; expected expression with side- effect
#      /w14619 # pragma warning: there is no warning number 'number'
      /w14640 # Enable warning on thread un-safe static member initialization
      /w14826 # Conversion from 'type1' to 'type_2' is sign-extended. This may
              # cause unexpected runtime behavior.
      /w14905 # wide string literal cast to 'LPSTR'
      /w14906 # string literal cast to 'LPWSTR'
      /w14928 # illegal copy-initialization; more than one user-defined
              # conversion has been implicitly applied

      # Disable the following errors
      /wd4101
      /wd4201

  )

  set(CLANG_WARNINGS
      -Wall
      -Wextra # reasonable and standard
      -Wshadow # warn the user if a variable declaration shadows one from a
               # parent context
      -Wnon-virtual-dtor # warn the user if a class with virtual functions has a
                         # non-virtual destructor. This helps catch hard to
                         # track down memory errors
      -Wold-style-cast # warn for c-style casts
      -Wcast-align # warn for potential performance problem casts
      -Wunused # warn on anything being unused
      -Woverloaded-virtual # warn if you overload (not override) a virtual
                           # function
      -Wpedantic # warn if non-standard C++ is used
      -Wconversion # warn on type conversions that may lose data
      -Wsign-conversion # warn on sign conversions
      -Wnull-dereference # warn if a null dereference is detected
      -Wdouble-promotion # warn if float is implicit promoted to double
      -Wformat=2 # warn on security issues around functions that format output
                 # (ie printf)
      -Wno-gnu-zero-variadic-macro-arguments
  )

  if (${PROJECT_NAME}_WARNINGS_AS_ERRORS)
      set(CLANG_WARNINGS ${CLANG_WARNINGS} -Werror)
      set(MSVC_WARNINGS ${MSVC_WARNINGS} /WX )
  endif()

  set(GCC_WARNINGS
      ${CLANG_WARNINGS}
      -Wmisleading-indentation # warn if identation implies blocks where blocks
                               # do not exist
      -Wduplicated-cond # warn if if / else chain has duplicated conditions
      -Wduplicated-branches # warn if if / else branches have duplicated code
      -Wlogical-op # warn about logical operations being used where bitwise were
                   # probably wanted
#      -Wuseless-cast # warn if you perform a cast to the same type
      -Wno-class-memaccess
      -Wno-stringop-overflow
      -Wsuggest-override
      -Wno-gnu-zero-variadic-macro-arguments
  )

  if(MSVC)
    set(_PROJECT_WARNINGS ${MSVC_WARNINGS})
  elseif(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    set(_PROJECT_WARNINGS ${CLANG_WARNINGS})
  else()
    set(_PROJECT_WARNINGS ${GCC_WARNINGS})
  endif()

  get_target_property(type ${project_name} TYPE)

  if (${type} STREQUAL "INTERFACE_LIBRARY")
      target_compile_options(${project_name} INTERFACE ${_PROJECT_WARNINGS})
  else()
      target_compile_options(${project_name} PRIVATE ${_PROJECT_WARNINGS})
  endif()


endfunction()
#==========

set_project_warnings(${PROJECT_NAME}_warnings)

message("New Target: ${PROJECT_NAME}::coverage")
message("New Target: ${PROJECT_NAME}::warnings")
message("New Target: ${PROJECT_NAME}::error")
message("*****************************************************\n\n\n")

