# "Gsw" as in "G swilmet" or "G software", as a namespace.

# Some Gsw variables:
# - GSW_LIB_NAME: e.g., libfoo-3
# - GSW_LIB_NAME_WITHOUT_LIB_PREFIX: e.g., foo-3
# - GSW_NAMESPACE: e.g., Foo

macro (GswInit)
  find_package (PkgConfig REQUIRED)
  include (GNUInstallDirs)
  set (CMAKE_EXPORT_COMPILE_COMMANDS TRUE)
endmacro ()

function (GswRegisterExposedDep name comparison_operator version)
  # "For file" means: for creating the *.pc file.
  # "For check" means: for pkg_check_modules().

  if ("${comparison_operator}" STREQUAL "")
    set (base_for_file "${name}")
    set (base_for_check "${name}")
  else ()
    # With spaces:
    set (base_for_file "${name} ${comparison_operator} ${version}")

    # Without spaces:
    set (base_for_check "${name}${comparison_operator}${version}")
  endif ()

  # Comma-separated list.
  if (NOT DEFINED GSW_PKG_CONFIG_EXPOSED_DEPS_FOR_FILE)
    set (GSW_PKG_CONFIG_EXPOSED_DEPS_FOR_FILE
      "${base_for_file}"
      PARENT_SCOPE)
  else ()
    set (GSW_PKG_CONFIG_EXPOSED_DEPS_FOR_FILE
      "${GSW_PKG_CONFIG_EXPOSED_DEPS_FOR_FILE}, ${base_for_file}"
      PARENT_SCOPE)
  endif ()

  # Semi-colon-separated list, to use as an unquoted arg to expand it to several
  # args.
  if (NOT DEFINED GSW_PKG_CONFIG_EXPOSED_DEPS_FOR_CHECK)
    set (GSW_PKG_CONFIG_EXPOSED_DEPS_FOR_CHECK
      "${base_for_check}"
      PARENT_SCOPE)
  else ()
    set (GSW_PKG_CONFIG_EXPOSED_DEPS_FOR_CHECK
      "${GSW_PKG_CONFIG_EXPOSED_DEPS_FOR_CHECK};${base_for_check}"
      PARENT_SCOPE)
  endif ()
endfunction ()

function (GswAddPkgConfigFile library_short_description)
  set (GSW_LIB_SHORT_DESCRIPTION "${library_short_description}")
  configure_file (
    "${PROJECT_SOURCE_DIR}/cmake/pkg-config-template.pc.in"
    "${PROJECT_BINARY_DIR}/${GSW_LIB_NAME}.pc"
    @ONLY)
  install (FILES "${PROJECT_BINARY_DIR}/${GSW_LIB_NAME}.pc"
    DESTINATION "${CMAKE_INSTALL_LIBDIR}/pkgconfig")
endfunction ()

function (GswPkgConfigStandardSetupForLibrary library_short_description)
  pkg_check_modules (GSW_PKG_CONFIG_DEPS REQUIRED ${GSW_PKG_CONFIG_EXPOSED_DEPS_FOR_CHECK})
  GswAddPkgConfigFile ("${library_short_description}")
endfunction ()

function (GswConfigFileStandardSetup)
  string (TOLOWER "${GSW_NAMESPACE}" lowercase_namespace)
  set (output_dir "${PROJECT_BINARY_DIR}/config-h")
  configure_file (
    "${PROJECT_SOURCE_DIR}/${lowercase_namespace}-config.h.in"
    "${output_dir}/${lowercase_namespace}-config.h")

  set (GSW_DIRS_TO_INCLUDE ${GSW_DIRS_TO_INCLUDE} "${output_dir}" PARENT_SCOPE)
endfunction ()

function (GswGetAbsolutePaths files_list output_list)
  foreach (file ${files_list})
    set (absolute_file "${CMAKE_CURRENT_SOURCE_DIR}/${file}")
    list (APPEND absolute_files_list "${absolute_file}")
  endforeach ()

  set (${output_list} "${absolute_files_list}" PARENT_SCOPE)
endfunction ()

function (GswFindGlibMkenumsProgram result)
  pkg_get_variable (my_result glib-2.0 glib_mkenums)

  if ("${my_result}" STREQUAL "")
    message (FATAL_ERROR "glib-mkenums program not found.")
  endif ()

  set (${result} "${my_result}" PARENT_SCOPE)
endfunction ()

function (GswAddGlibMkenumsCommand glib_mkenums_executable
  template output_dir output_filename absolute_paths_to_headers)
  set (output "${output_dir}/${output_filename}")

  add_custom_command (OUTPUT "${output}"
    COMMAND ${CMAKE_COMMAND}
    ARGS -E make_directory "${output_dir}"
    COMMAND "${glib_mkenums_executable}"
    ARGS --template "${template}" --output "${output}" ${absolute_paths_to_headers}
    DEPENDS "${glib_mkenums_executable}" "${template}" ${absolute_paths_to_headers}
    COMMENT "Generating ${output_filename}")
endfunction ()

function (GswGlibMkenumsPublic public_headers)
  GswFindGlibMkenumsProgram (glib_mkenums_executable)
  GswGetAbsolutePaths ("${public_headers}" absolute_paths_to_public_headers)
  string (TOLOWER "${GSW_NAMESPACE}" lowercase_namespace)
  set (output_dir "${PROJECT_BINARY_DIR}/glib-mkenums/${lowercase_namespace}")
  set (output_header "${lowercase_namespace}-enum-types.h")
  set (output_c_file "${lowercase_namespace}-enum-types.c")

  foreach (output_filename "${output_header}" "${output_c_file}")
    set (template "${PROJECT_SOURCE_DIR}/${lowercase_namespace}/${output_filename}.in")
    GswAddGlibMkenumsCommand ("${glib_mkenums_executable}"
      "${template}"
      "${output_dir}"
      "${output_filename}"
      "${absolute_paths_to_public_headers}")
  endforeach ()

  set (GSW_GENERATED_PUBLIC_HEADERS
    ${GSW_GENERATED_PUBLIC_HEADERS} "${output_dir}/${output_header}"
    PARENT_SCOPE)

  set (GSW_GENERATED_PUBLIC_C_FILES
    ${GSW_GENERATED_PUBLIC_C_FILES} "${output_dir}/${output_c_file}"
    PARENT_SCOPE)

  set (GSW_DIRS_TO_INCLUDE ${GSW_DIRS_TO_INCLUDE}
    "${output_dir}"
    "${PROJECT_SOURCE_DIR}/${lowercase_namespace}" # for building enum-types.c
    PARENT_SCOPE)
endfunction ()

function (GswInstallAllPublicHeaders public_headers)
  string (TOLOWER "${GSW_NAMESPACE}" lowercase_namespace)
  install (FILES ${public_headers} ${GSW_GENERATED_PUBLIC_HEADERS}
    DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/${GSW_LIB_NAME}/${lowercase_namespace}")
endfunction ()

# Try to mimic the AX_COMPILER_FLAGS Autotools macro.
#
# For the rationale (having such a long list of flags instead of just relying on
# -Wall and -Wextra) see the GCC manpage for the -Wall option:
#
# """
# Note that some warning flags are not implied by -Wall. Some of them warn about
# constructions that users generally do not consider questionable, but which
# occasionally you might wish to check for; others warn about constructions that
# are necessary or hard to avoid in some cases, and there is no simple way to
# modify the code to suppress the warning. Some of them are enabled by -Wextra
# but many of them must be enabled individually.
# """
function (GswCompilerFlags target_name)
  target_compile_options ("${target_name}" PRIVATE
    "-Wall"
    "-Wextra"
    "-fno-strict-aliasing"
    "-Wundef"
    "-Wwrite-strings"
    "-Wpointer-arith"
    "-Wmissing-declarations"
    "-Wredundant-decls"
    "-Wno-unused-parameter"
    "-Wno-missing-field-initializers"
    "-Wformat=2"
    "-Wcast-align"
    "-Wformat-nonliteral"
    "-Wformat-security"
    "-Wsign-compare"
    "-Wstrict-aliasing"
    "-Wshadow"
    "-Winline"
    "-Wpacked"
    "-Wmissing-format-attribute"
    "-Wmissing-noreturn"
    "-Winit-self"
    "-Wredundant-decls"
    "-Wmissing-include-dirs"
    "-Wunused-but-set-variable"
    "-Warray-bounds"
    "-Wreturn-type"
    "-Wswitch-enum"
    "-Wswitch-default"
    "-Wduplicated-cond"
    "-Wduplicated-branches"
    "-Wlogical-op"
    "-Wrestrict"
    "-Wnull-dereference"
    "-Wdouble-promotion"
    "-Wnested-externs"
    "-Wmissing-prototypes"
    "-Wstrict-prototypes"
    "-Wdeclaration-after-statement"
    "-Wimplicit-function-declaration"
    "-Wold-style-definition"
    "-Wjump-misses-init"
  )
endfunction ()

function (GswApplyPkgConfigDepsToTarget target_name pkg_dep)
  target_include_directories ("${target_name}" PRIVATE ${${pkg_dep}_INCLUDE_DIRS})
  target_compile_options ("${target_name}" PRIVATE ${${pkg_dep}_CFLAGS_OTHER})
  target_link_libraries ("${target_name}" ${${pkg_dep}_LDFLAGS})
endfunction ()

function (GswDefineGLogDomain target_name)
  target_compile_definitions ("${target_name}"
    PRIVATE "-DG_LOG_DOMAIN=\"${PROJECT_NAME}\"")
endfunction ()

function (GswLibraryEnsureSingleHeaderExternalInclude library_name)
  string (TOUPPER "${GSW_NAMESPACE}" uppercase_namespace)

  target_compile_definitions ("${library_name}"
    PRIVATE "-D${uppercase_namespace}_COMPILATION")
endfunction ()

function (GswAddExecutable executable_name sources pkg_dep)
  add_executable ("${executable_name}" ${sources})

  GswApplyPkgConfigDepsToTarget ("${executable_name}" "${pkg_dep}")
  GswCompilerFlags ("${executable_name}")
  GswDefineGLogDomain ("${executable_name}")

  install (TARGETS "${executable_name}"
    DESTINATION "${CMAKE_INSTALL_BINDIR}")
endfunction ()

function (GswAddExecutableSimple)
  GswAddExecutable ("${PROJECT_NAME}"
    "${GSW_EXECUTABLE_SOURCES}"
    GSW_PKG_CONFIG_DEPS)
endfunction ()

function (GswAddLibrary library_name sources pkg_dep)
  add_library ("${library_name}" SHARED ${sources})

  GswApplyPkgConfigDepsToTarget ("${library_name}" "${pkg_dep}")
  GswCompilerFlags ("${library_name}")
  GswDefineGLogDomain ("${library_name}")

  install (TARGETS "${library_name}"
    DESTINATION "${CMAKE_INSTALL_LIBDIR}")
endfunction ()

# Useful for printing a configuration summary.
function (GswYesOrNo condition result)
  if (${condition})
    set (${result} yes PARENT_SCOPE)
  else ()
    set (${result} no PARENT_SCOPE)
  endif ()
endfunction ()
