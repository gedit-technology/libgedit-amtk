amtk_public_headers = [
  'amtk.h',
  'amtk-action-info-central-store.h',
  'amtk-action-info.h',
  'amtk-action-info-store.h',
  'amtk-action-map.h',
  'amtk-application-window.h',
  'amtk-factory.h',
  'amtk-gmenu.h',
  'amtk-init.h',
  'amtk-menu-item.h',
  'amtk-menu-shell.h',
  'amtk-shortcuts.h',
  'amtk-utils.h',
]

amtk_public_c_files = [
  'amtk-action-info.c',
  'amtk-action-info-central-store.c',
  'amtk-action-info-store.c',
  'amtk-action-map.c',
  'amtk-application-window.c',
  'amtk-factory.c',
  'amtk-gmenu.c',
  'amtk-init.c',
  'amtk-menu-item.c',
  'amtk-menu-shell.c',
  'amtk-shortcuts.c',
  'amtk-utils.c',
]

headers_install_dir = get_option('includedir') / '@0@-@1@/amtk/'.format(meson.project_name(), AMTK_API_VERSION)
install_headers(
  amtk_public_headers,
  install_dir: headers_install_dir
)

amtk_enum_types = GNOME.mkenums_simple(
  'amtk-enum-types',
  decorator: 'G_MODULE_EXPORT',
  header_prefix: '#include <gmodule.h>',
  sources: amtk_public_headers,
  install_header: true,
  install_dir: headers_install_dir
)

amtk_static_lib = static_library(
  'libgedit-amtk-static',
  [amtk_public_c_files,
   amtk_enum_types],
  pic: true, # amtk_static_lib is potentially linked in a shared library.
  include_directories: ROOT_INCLUDE_DIR,
  dependencies: AMTK_DEPS,
  c_args: '-DAMTK_COMPILATION'
)

# For unit tests, to be able to test private functions.
AMTK_STATIC_DEP = declare_dependency(
  include_directories: ROOT_INCLUDE_DIR,
  link_with: amtk_static_lib,
  sources: amtk_enum_types[1],
  dependencies: AMTK_DEPS
)

symbol_map = 'symbol.map'
amtk_lib_link_args = c_compiler.get_supported_link_arguments([
  '-Wl,--version-script,@0@/@1@'.format(meson.current_source_dir(), symbol_map),
])

amtk_lib = library(
  'gedit-amtk-@0@'.format(AMTK_API_VERSION),
  dependencies: AMTK_DEPS,
  link_args: amtk_lib_link_args,
  link_depends: symbol_map,
  # link_whole is not supported with MSVC, so we use extract_all_objects().
  objects: amtk_static_lib.extract_all_objects(recursive: false),
  soversion: 0,
  install: true
)

AMTK_LIB_DEP = declare_dependency(
  include_directories: ROOT_INCLUDE_DIR,
  link_with: amtk_lib,
  sources: amtk_enum_types[1],
  dependencies: AMTK_DEPS
)

PKG_CONFIG.generate(amtk_lib,
  filebase: '@0@-@1@'.format(meson.project_name(), AMTK_API_VERSION),
  name: meson.project_name(),
  description: 'Actions, Menus and Toolbars Kit',
  subdirs: '@0@-@1@'.format(meson.project_name(), AMTK_API_VERSION),
  libraries: AMTK_DEPS,
)

if get_option('gobject_introspection')
  GNOME.generate_gir(
    amtk_lib,
    export_packages: '@0@-@1@'.format(meson.project_name(), AMTK_API_VERSION),
    header: 'amtk/amtk.h',
    identifier_prefix: 'Amtk',
    include_directories: ROOT_INCLUDE_DIR,
    includes: ['Gtk-3.0'],
    install: true,
    namespace: 'Amtk',
    nsversion: AMTK_API_VERSION,
    sources: [
      amtk_public_headers,
      amtk_public_c_files,
      amtk_enum_types
    ],
    # Support for deps being built as subprojects:
    dependencies: AMTK_DEPS,
  )
endif
