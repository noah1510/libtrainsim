sources = [
    'src/statusDisplay.cpp',
]

statusDisplayDeps = [
    dependency('libtrainsim-video', required: true),
]

incdir = include_directories('include','.','..')

libtrainsim_extras_statusDisplay = library(
    'trainsim-extras-statusDisplay', 
    sources, 
    version : meson.project_version(), 
    soversion : '0',
    include_directories : incdir,
    dependencies: statusDisplayDeps,
    install : true,
)

libtrainsim_statusDisplay_dep = declare_dependency(
    include_directories : incdir,
    link_with : libtrainsim_extras_statusDisplay,
    dependencies : statusDisplayDeps,
    version: meson.project_version()
)

install_subdir('include', install_dir : 'include/libtrainsim', strip_directory : true)

pkg = import('pkgconfig')
pkg.generate(libtrainsim_extras_statusDisplay)
