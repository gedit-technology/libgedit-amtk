subdir('xml')

FS.copyfile(
  'libgedit-amtk-sections.txt',
  'libgedit-amtk-@0@-sections.txt'.format(AMTK_API_VERSION)
)

gtkdoc_module_name = 'libgedit-amtk-@0@'.format(AMTK_API_VERSION)
html_dir = get_option('prefix') / GNOME.gtkdoc_html_dir(gtkdoc_module_name)

glib_docpath = dependency('glib-2.0').get_variable(pkgconfig: 'prefix') / 'share/gtk-doc/html/glib'
gobject_docpath = dependency('gobject-2.0').get_variable(pkgconfig: 'prefix') / 'share/gtk-doc/html/gobject'
gio_docpath = dependency('gio-2.0').get_variable(pkgconfig: 'prefix') / 'share/gtk-doc/html/gio'
gtk_docpath = dependency('gtk+-3.0').get_variable(pkgconfig: 'prefix') / 'share/gtk-doc/html/gtk3'

GNOME.gtkdoc(
  gtkdoc_module_name,
  main_xml: 'libgedit-amtk-docs.xml',
  src_dir: include_directories('../../amtk/'),
  dependencies: AMTK_LIB_DEP,
  scan_args: ['--rebuild-types'],
  gobject_typesfile: 'libgedit-amtk-@0@.types'.format(AMTK_API_VERSION),
  fixxref_args: [
    '--html-dir=@0@'.format(html_dir),
    '--extra-dir=@0@'.format(glib_docpath),
    '--extra-dir=@0@'.format(gobject_docpath),
    '--extra-dir=@0@'.format(gio_docpath),
    '--extra-dir=@0@'.format(gtk_docpath)
  ],
  content_files: [
    'libgedit-amtk-intro.xml',
    'gradual-porting.xml',
  ],
  install: true
)
