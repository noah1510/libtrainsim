cc = meson.get_compiler('cpp')

sources = [
    'src/physics.cpp',
]

incdir = include_directories('include','..')

physics_deps = [
    dependency('libtrainsim-core', required: true),
]

libtrainsim_physics = library(
    'trainsim-physics', 
    sources, 
    version : meson.project_version(), 
    soversion : '0',
    include_directories : incdir,
    dependencies: physics_deps,
    install : true,
)

libtrainsim_physics_dep = declare_dependency(
    include_directories : incdir,
    link_with : libtrainsim_physics,
    dependencies : physics_deps,
    version: meson.project_version(),
)

install_subdir('include', install_dir : 'include/libtrainsim', strip_directory : true)

pkg = import('pkgconfig')
pkg.generate(libtrainsim_physics)
