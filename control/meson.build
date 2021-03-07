cc = meson.get_compiler('cpp')

sources = [
    'src/control.cpp',
]

incdir = include_directories('include','..')

control_deps = [
    dependency('threads'),
    dependency('libtrainsim-core', required: true),
]

video_dep = dependency('libtrainsim-video', required: false)
if video_dep.found()
    control_deps += video_dep
endif

libtrainsim_control = library(
    'libtrainsim-control', 
    sources, 
    version : meson.project_version(), 
    soversion : '0',
    include_directories : incdir,
    dependencies: control_deps,
)

libtrainsim_control_dep = declare_dependency(
    include_directories : incdir,
    link_with : libtrainsim_control,
    dependencies : control_deps,
    version: meson.project_version()
)

install_subdir('include', install_dir : 'include/libtrainsim')

pkg = import('pkgconfig')
pkg.generate(libtrainsim_control)
