cc = meson.get_compiler('cpp')

sources = [
    'src/video.cpp',
    'src/frame.cpp',
]

video_deps = [
    dependency('threads', required: true),
]

#try to find opencv backend dependencies
opencv_dep = dependency('opencv4', required : false)
if not opencv_dep.found()
    opencv_dep = dependency('opencv3', required : false)
    if not opencv_dep.found()
        opencv_dep = dependency('opencv2', required : false, version: '>=2.4.0')
        if not opencv_dep.found()
            opencv_dep = dependency('opencv', required : false, version: '>=2.4.0')
        endif
    endif
endif

#set enable_opencv to the correct value
enable_opencv = not get_option('video_backend_opencv') and opencv_dep.found()

#enable opencv if needed
if enable_opencv
    video_deps += opencv_dep
    sources += 'src/opencv/frame.cpp'
    sources += 'src/opencv/video.cpp'
    message('compiling with opencv')
endif

#create the video configuration file
video_conf_data = configuration_data()
video_conf_data.set('HAS_OPENCV_SUPPORT', enable_opencv)
configure_file(output : 'video_config.hpp', configuration : video_conf_data)

incdir = include_directories('include','.')

#define the video component
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

if get_option('force_build_tests') or (get_option('build_tests') and not meson.is_subproject())
    subdir('tests')
endif
