cc = meson.get_compiler('cpp')

sources = [
#    'src/control.cpp',
]

incdir = include_directories('include')

core_deps = [
    dependency('threads'),
]

libtrainsim_core = library(
    'libtrainsim-core', 
    sources, 
    version : meson.project_version(), 
    soversion : '0',
    include_directories : incdir,
    dependencies: core_deps,
)

libtrainsim_core_dep = declare_dependency(
    include_directories : incdir,
    link_with : libtrainsim_core,
    dependencies : core_deps,
    version: meson.project_version()
)

install_subdir('include', install_dir : 'include/libtrainsim')

pkg = import('pkgconfig')
pkg.generate(libtrainsim_core)
