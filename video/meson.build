cc = meson.get_compiler('cpp')

sources = [
    'src/video.cpp',
]

incdir = include_directories('include')

video_deps = [
    dependency('threads', required: true),
    dependency('opencv4', required: true),
]

libtrainsim_video = library(
    'libtrainsim-video', 
    sources, 
    version : meson.project_version(), 
    soversion : '0',
    include_directories : incdir,
    dependencies: video_deps,
)

libtrainsim_video_dep = declare_dependency(
    include_directories : incdir,
    link_with : libtrainsim_video,
    dependencies : video_deps,
    version: meson.project_version()
)

install_subdir('include', install_dir : 'include')

pkg = import('pkgconfig')
pkg.generate(libtrainsim_video)
