sources = [
    'src/videoDecode/videoDecoderBase.cpp',

    'src/renderWidget/displayFragShader.cpp',
]

video_deps = [
    dependency('threads', required: true),
    dependency('libtrainsim-core', required: true),
    dependency('simplegl', required: true, fallback: 'simplegl', version : '>=0.1.0'),
]

#handle normal builds 

CC = meson.get_compiler('c')

has_libav = false
if get_option('video_decode_libav').allowed()
    #try to find ffmpeg dependencies
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

    all_libav_found = true
    foreach lib : av_libs
        dep_i = dependency(
            'lib@0@'.format(lib[0]),
            required: get_option('video_decode_libav').enabled(),
            version : '>=@0@'.format(lib[1])
        )

        video_deps += [dep_i]
        if not dep_i.found()
            all_libav_found = false
        endif
    endforeach

    has_libav = all_libav_found
endif

has_gstreamer = false
if get_option('video_decode_gstreamer').allowed()
    gst_dep = dependency(
        'gstreamer-1.0',
        required: get_option('video_decode_gstreamer').enabled(),
        version : '>=1.0.0',
    )
    video_deps += [gst_dep]
    has_gstreamer = gst_dep.found()
endif

if not has_libav and not has_gstreamer
    error('No video decoder backend found! Install either ffmpeg or gstreamermm.')
endif

if has_libav
    sources += ['src/videoDecode/videoDecoderLibav.cpp']
endif

if has_gstreamer
    sources += ['src/videoDecode/videoDecoderGstreamer.cpp']
endif

#create the video configuration file
video_conf_data = configuration_data()
video_conf_data.set('HAS_FFMPEG_SUPPORT', has_libav)
video_conf_data.set('HAS_GSTREAMER_SUPPORT', has_gstreamer)
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
    cpp_pch: 'include/pch/libtrainsim_video_pch.hpp',
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
