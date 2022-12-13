cmake_minimum_required (VERSION 3.20)
project (libgedit-amtk
  VERSION 5.6.1
  LANGUAGES C)

include (cmake/gsw.cmake)
GswInit ()

set (api_version 5)
set (GSW_LIB_NAME "libgedit-amtk-${api_version}")
set (GSW_LIB_NAME_WITHOUT_LIB_PREFIX "gedit-amtk-${api_version}")
set (GSW_NAMESPACE Amtk)

GswRegisterExposedDep ("gio-2.0" ">=" "2.56")
GswRegisterExposedDep ("gtk+-3.0" ">=" "3.22")
GswPkgConfigStandardSetupForLibrary ("Actions, Menus and Toolbars Kit for GTK applications")

GswConfigFileStandardSetup ()

add_subdirectory (amtk)
