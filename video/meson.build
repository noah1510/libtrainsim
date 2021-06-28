sources = [
    'src/video.cpp',
    'src/frame.cpp',
    'src/generic.cpp',
    'src/genericRenderer.cpp',
    'src/genericWindowManager.cpp'
]

video_deps = [
    dependency('threads', required: true),
]

enable_opencv = false
enable_ffmpeg = false
enable_sdl = false

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
    sources += 'src/ffmpeg/backend.cpp'
    
    sources += 'src/backends/ffmpegRenderer.cpp'
    sources += 'src/backends/genericWindowManager.cpp'
    sources += 'src/backends/ffmpegFrame.cpp'
    message('compiling with ffmpeg enabled')
    
else
    #handle normal builds 

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
        sources += 'src/backends/openCVFrame.cpp'
        sources += 'src/backends/openCVRenderer.cpp'
        sources += 'src/backends/openCVWindowManager.cpp'
        sources += 'src/opencv/backend.cpp'
        message('compiling with opencv enabled')
    endif

    #enable ffmpeg if needed
    if enable_ffmpeg
        video_deps += ffmpeg_dep
        sources += 'src/ffmpeg/backend.cpp'
        
        sources += 'src/backends/ffmpegRenderer.cpp'
        sources += 'src/backends/ffmpegFrame.cpp'
        sources += 'src/backends/SDL2WindowManager.cpp'
        message('compiling with ffmpeg enabled')
    endif

    #enable sdl2 if needed
    if enable_sdl
        video_deps += dependency('sdl2', required : true, fallback: ['sdl2','sdl2_dep'])
        
        message('compiling with sdl2 enabled')
    endif

endif

#create the video configuration file
video_conf_data = configuration_data()
video_conf_data.set('HAS_OPENCV_SUPPORT', enable_opencv)
video_conf_data.set('HAS_FFMPEG_SUPPORT', enable_ffmpeg)
video_conf_data.set('HAS_SDL_SUPPORT', enable_sdl)
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

if get_option('force_build_tests') or (get_option('build_tests') and not meson.is_subproject())
    subdir('tests')
endif
