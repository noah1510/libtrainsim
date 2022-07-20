sources = [
    'src/video.cpp',
    'src/frame.cpp',
    'src/videoDecoder.cpp',
    'src/glad.c',
]

video_deps = [
    dependency('threads', required: true),
]

#check if emscripten is used
if target_machine.system() == 'emscripten'

    CC = meson.get_compiler('c')
    CXX = meson.get_compiler('cpp')

    enable_sdl = true
        
    message('compiling with sdl2 enabled')
    
    #try to find ffmpeg dependencies
    enable_ffmpeg = true
    
    ffmpeg_dep = []
    ffmpeg = subproject('ffmpeg', default_options: ['default_library=static','c_std=c99'])
    
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
    
    #foreach lib : av_libs
    #    ffmpeg_dep += ffmpeg.get_variable('lib@0@'.format(lib[0]))
    #endforeach
    
    #video_deps += ffmpeg_dep
    video_deps += ffmpeg.get_variable('declared_dep')
    sources += 'src/backends/videoDecoder.cpp'
    sources += 'src/backends/ffmpegFrame.cpp'
    message('compiling with ffmpeg enabled')
    
else
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
    
    video_deps += dependency('sdl2', required : true, fallback: ['sdl2','sdl2_dep'])
    video_deps += dependency('glm', required : true, fallback : ['glm','glm_dep'])
    video_deps += dependency('gl', required : true)
    video_deps += CC.find_library('dl', required : false)
    
    
endif

#create the video configuration file
video_conf_data = configuration_data()
video_conf_data.set('HAS_FFMPEG_SUPPORT', true)
video_conf_data.set('HAS_SDL_SUPPORT', true)
video_conf_data.set('ENDABLE_VSYNC', get_option('video_use_vsync'))
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
