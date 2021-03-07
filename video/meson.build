cc = meson.get_compiler('cpp')

sources = [
    'src/video.cpp',
    'src/frame.cpp',
    'src/generic.cpp',
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
enable_opencv = get_option('video_backend_opencv') and opencv_dep.found()

#try to find sdl2 dependnecies and enable sources
enable_sdl = get_option('video_backend_sdl')
if enable_sdl
    video_deps += dependency('sdl2', required : true, fallback: ['sdl2','sdl2_dep'])
    
    message('compiling with sdl2 enabled')
endif


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
    ffmpeg_dep += [
        dependency(
            'lib@0@'.format(lib[0]),
            required: false,
            #fallback : ['ffmpeg', 'lib@0@_dep'.format(lib[0])],
            version : '>=@0@'.format(lib[1]))
    ]
endforeach

#set enable_ffmpeg to the correct value
ffmpeg_found = true
foreach dep:ffmpeg_dep
    if not dep.found()
        ffmpeg_found = false
    endif
endforeach
enable_ffmpeg = get_option('video_backend_ffmpeg') and ffmpeg_found and (enable_sdl)

#enable opencv if needed
if enable_opencv
    video_deps += opencv_dep
    sources += 'src/opencv/frame.cpp'
    sources += 'src/opencv/backend.cpp'
    message('compiling with opencv enabled')
endif

#enable ffmpeg if needed
if enable_ffmpeg
    video_deps += ffmpeg_dep
    sources += 'src/ffmpeg/frame.cpp'
    sources += 'src/ffmpeg/backend.cpp'
    message('compiling with ffmpeg enabled')
endif

#create the video configuration file
video_conf_data = configuration_data()
video_conf_data.set('HAS_OPENCV_SUPPORT', enable_opencv)
video_conf_data.set('HAS_FFMPEG_SUPPORT', enable_ffmpeg)
video_conf_data.set('HAS_SDL_SUPPORT', enable_sdl)
video_conf_data.set('ENDABLE_VSYNC', get_option('video_use_vsync'))
configure_file(output : 'video_config.hpp', configuration : video_conf_data)

#include all headers including the video configuration
incdir = include_directories('include','.','..')

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

install_subdir('include', install_dir : 'include/libtrainsim')

pkg = import('pkgconfig')
pkg.generate(libtrainsim_video)

if get_option('force_build_tests') or (get_option('build_tests') and not meson.is_subproject())
    subdir('tests')
endif
