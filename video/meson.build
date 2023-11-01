sources = [
    'src/video.cpp',
    'src/video_reader.cpp',
    'src/simulatorRenderWidget.cpp',
]

video_deps = [
    dependency('threads', required: true),
    dependency('libtrainsim-core', required: true),
    dependency('simplegl', required: true, fallback: 'simplegl', version : '>=0.1.0'),
]

#handle normal builds 

CC = meson.get_compiler('c')

#try to find ffmpeg dependencies
ffmpeg_dep = []
av_libs = [
    ['avcodec', '55.28.1'],
    ['avformat',  '54.0.0'],
    ['avutil', '51.0.0'],
    ['avdevice', '54.0.0'],
    ['avfilter', '3.0.0'],
    ['swscale', '2.0.0'],
    ['swresample', '0.0.0'],
    ['postproc', '52.0.0'],
]

foreach lib : av_libs
    video_deps += [
        dependency(
            'lib@0@'.format(lib[0]),
            required: true,
            #fallback : ['ffmpeg', 'lib@0@_dep'.format(lib[0])],
            version : '>=@0@'.format(lib[1]))
    ]
endforeach

#create the video configuration file
video_conf_data = configuration_data()
video_conf_data.set('HAS_FFMPEG_SUPPORT', true)
configure_file(output : 'video_config.hpp', configuration : video_conf_data, install: true, install_dir: 'include/libtrainsim')

#include all headers including the video configuration
incdir = include_directories('include','.','..')

#define the video component
libtrainsim_video = library(
    'trainsim-video', 
    sources, 
    version : meson.project_version(), 
    soversion : '0',
    include_directories : incdir,
    dependencies: video_deps,
    install : true,
)

libtrainsim_video_dep = declare_dependency(
    include_directories : incdir,
    link_with : libtrainsim_video,
    dependencies : video_deps,
    version: meson.project_version()
)

install_subdir('include', install_dir : 'include/libtrainsim', strip_directory : true)

pkg = import('pkgconfig')
pkg.generate(libtrainsim_video)
