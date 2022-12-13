set (public_headers
  amtk.h
  amtk-action-info.h
  amtk-action-info-central-store.h
  amtk-action-info-store.h
  amtk-action-map.h
  amtk-application-window.h
  amtk-factory.h
  amtk-gmenu.h
  amtk-init.h
  amtk-menu-item.h
  amtk-menu-shell.h
  amtk-shortcuts.h
  amtk-utils.h)

set (public_c_files
  amtk-action-info.c
  amtk-action-info-central-store.c
  amtk-action-info-store.c
  amtk-action-map.c
  amtk-application-window.c
  amtk-factory.c
  amtk-gmenu.c
  amtk-init.c
  amtk-menu-item.c
  amtk-menu-shell.c
  amtk-shortcuts.c
  amtk-utils.c)

GswGlibMkenumsPublic ("${public_headers}")
GswInstallAllPublicHeaders ("${public_headers}")

set (all_source_files
  ${public_headers} ${GSW_GENERATED_PUBLIC_HEADERS}
  ${public_c_files} ${GSW_GENERATED_PUBLIC_C_FILES})

GswAddLibrary (${GSW_LIB_NAME_WITHOUT_LIB_PREFIX} "${all_source_files}" GSW_PKG_CONFIG_DEPS)
GswLibraryEnsureSingleHeaderExternalInclude (${GSW_LIB_NAME_WITHOUT_LIB_PREFIX})

target_include_directories (${GSW_LIB_NAME_WITHOUT_LIB_PREFIX}
  PRIVATE ${GSW_DIRS_TO_INCLUDE})

# For the #include <amtk/something.h>
target_include_directories (${GSW_LIB_NAME_WITHOUT_LIB_PREFIX}
  PRIVATE "${PROJECT_SOURCE_DIR}")
