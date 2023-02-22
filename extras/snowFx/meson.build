sources = [
    'src/snowFx.cpp',
    'src/wiper.cpp',
]

snowFxDeps = [
    dependency('libtrainsim-core', required: true),
]

incdir = include_directories('include','.','..')

libtrainsim_extras_snowFx = library(
    'trainsim-extras-snowFx', 
    sources, 
    version : meson.project_version(), 
    soversion : '0',
    include_directories : incdir,
    dependencies: snowFxDeps,
    install : true,
)

libtrainsim_snowFx_dep = declare_dependency(
    include_directories : incdir,
    link_with : libtrainsim_extras_snowFx,
    dependencies : snowFxDeps,
    version: meson.project_version()
)

install_subdir('include', install_dir : 'include/libtrainsim', strip_directory : true)

pkg = import('pkgconfig')
pkg.generate(libtrainsim_extras_snowFx)
