include(sitkExternalData)
include(CMakeParseArguments)

set(SimpleITK_DATA_ROOT ${SimpleITK_SOURCE_DIR}/Testing/Data)

#-----------------------------------------------------------------------------
# SimpleITK wrapper for add_test that adds support for external data,
# and post comparison processes.
#
# ARGUMENTS
#
# [ADD_TEST ARGUMENTS...]
#   The first arguments are passed to cmake's add_test function with
#   support for the DATA{} references to external data
# TRANSFORM_COMPARE <test transform> <baseline displacement> [tolerance]
function(sitk_add_test)
  set(options "")
  set(oneValueArgs "")
  set(multiValueArgs TRANSFORM_COMPARE)
  cmake_parse_arguments("_" "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN} )

  # Add test with data in the SimpleITKData group.
  ExternalData_add_test(SimpleITKData ${__UNPARSED_ARGUMENTS})

  if("NAME" STREQUAL "${ARGV0}")
    set(_iat_testname ${ARGV1})
  else()
    set(_iat_testname ${ARGV0})
  endif()

  if ( NOT "${__TRANSFORM_COMPARE}" STREQUAL "" )
    ExternalData_add_test(SimpleITKData NAME "${_iat_testname}Compare"
      COMMAND
        $<TARGET_FILE:sitkTransformCompareDriver>
        ${__TRANSFORM_COMPARE}
      )

    set_property(TEST "${_iat_testname}Compare" APPEND PROPERTY DEPENDS ${_iat_testname})
  endif()
endfunction()



if ( WRAP_PYTHON )

  #
  # This is a function which set up the environment for executing python examples and tests
  #
  function(sitk_add_python_test name)

    set(command "${VIRTUAL_PYTHON_EXECUTABLE}")

    # add extra command which may be needed on some systems
    if(CMAKE_OSX_ARCHITECTURES)
      list(GET CMAKE_OSX_ARCHITECTURES 0 test_arch)
      set(command arch -${test_arch} ${command})
    endif()

    sitk_add_test(NAME Python.${name}
      COMMAND "${ITK_TEST_DRIVER}"
      --add-before-env SITK_NOSHOW "YES"
      ${command}
      ${ARGN}
      )
  endfunction()
endif()


if ( WRAP_LUA )

  #
  # This is a function which set up the enviroment for executing lua examples and tests
  #
  function(sitk_add_lua_test name)

    set(command "$<TARGET_FILE:SimpleITKLua>")

    # add extra command which may be needed on some systems
    if(CMAKE_OSX_ARCHITECTURES)
      list(GET CMAKE_OSX_ARCHITECTURES 0 test_arch)
      set(command arch -${test_arch} ${command})
    endif()

    sitk_add_test(NAME Lua.${name}
      COMMAND "${ITK_TEST_DRIVER}"
      ${command}
      ${ARGN}
      )
  endfunction()
endif ( WRAP_LUA )


if ( WRAP_RUBY )

  #
  # This is a function which set up the enviroment for executing ruby examples and tests
  #
  function(sitk_add_ruby_test name)

    set(command "${RUBY_EXECUTABLE}")

    # add extra command which may be needed on some systems
    if(CMAKE_OSX_ARCHITECTURES)
      list(GET CMAKE_OSX_ARCHITECTURES 0 test_arch)
      set(command arch -${test_arch} ${command})
    endif()

    sitk_add_test(NAME Ruby.${name}
      COMMAND "${ITK_TEST_DRIVER}"
      --add-before-env RUBYLIB "${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/$<CONFIGURATION>"
      --add-before-env RUBYLIB "${CMAKE_LIBRARY_OUTPUT_DIRECTORY}"
      ${command}
      ${ARGN}
      )
  endfunction()
endif ( WRAP_RUBY )


if ( WRAP_TCL )

  #
  # This is a function which set up the enviroment for executing TCL examples and tests
  #
  function(sitk_add_tcl_test name)

    set(command "$<TARGET_FILE:SimpleITKTclsh>")

    # add extra command which may be needed on some systems
    if(CMAKE_OSX_ARCHITECTURES)
      list(GET CMAKE_OSX_ARCHITECTURES 0 test_arch)
      set(command arch -${test_arch} ${command})
    endif()

    sitk_add_test(NAME Tcl.${name}
      COMMAND "${ITK_TEST_DRIVER}"
      ${command}
      ${ARGN}
      )
  endfunction()
endif ( WRAP_TCL )
