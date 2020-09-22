cc = meson.get_compiler('cpp')

sources = [
    'src/video.cpp',
]

incdir = include_directories('include')

video_deps = [
    dependency('threads', required: true),
]

opencv_dep = dependency('opencv4', required : false)
if not opencv_dep.found()
    opencv_dep = dependency('opencv3', required : false)
    if not opencv_dep.found()
        opencv_dep = dependency('opencv2', required : false, version: '>=2.4.0')
        if not opencv_dep.found()
            opencv_dep = dependency('opencv', required : true, version: '>=2.4.0')
        endif
    endif
endif

video_deps += opencv_dep

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

if get_option('build_tests') and not meson.is_subproject()
    subdir('tests')
endif
