# target_use_itk
#
# This function has the following form:
#   sitk_target_use_itk(<target>
#                       <PRIVATE|PUBLIC|INTERFACE>
#                       [itk_module [itk_module [...]]])
#
#   It configures the target, with the require include directories and
# link libraries to use the list of itk modules provided. The keyword
# to indicate interface of the library is also required.

function(sitk_target_use_itk target_name interface_keyword)

  set(itk_modules ${ARGV})
  list(REMOVE_AT itk_modules 0 1)

  itk_module_config(_itk ${itk_modules})

  foreach(mod ${itk_modules})
    target_link_libraries( ${target_name}
      ${interface_keyword}
        ${_itk_LIBRARIES} )
    target_include_directories( ${target_name}
      ${interface_keyword}
        ${_itk_INCLUDE_DIRS} )
  endforeach()

endfunction()
