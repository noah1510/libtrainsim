sources = [
    'src/video.cpp',
    'src/frame.cpp',
    'src/imguiHandler.cpp',
    'src/tabPage.cpp',
    'src/video_reader.cpp',
    'src/shader.cpp',
    'src/texture.cpp',
    'src/dimensions.cpp',
    'src/window.cpp',
    'src/video_settings.cpp',
    'src/defaultShader.cpp',
    'src/popup.cpp',
]

cmake = import('cmake')

nfde_opts = cmake.subproject_options()
nfde_opts.add_cmake_defines({'BUILD_SHARED_LIBS': get_option('default_library') != 'static'})

nfde_proj = cmake.subproject('nativefiledialog-extended', options:nfde_opts)

video_deps = [
    dependency('threads', required: true),
    dependency('libtrainsim-core', required: true),
    nfde_proj.dependency('nfd'),
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

video_deps += dependency('sdl2', required : true, fallback: ['sdl2','sdl2_dep'])
video_deps += dependency('SDL2_image', required: true)
video_deps += dependency('glm', required : true, fallback : ['glm','glm_dep'])
imgui_subp = subproject('imgui', default_options : [
    'dx9=disabled', 
    'dx10=disabled', 
    'dx11=disabled', 
    'dx12=disabled',
    'vulkan=disabled',
    'metal=disabled',
    'sdl_renderer=disabled',
    'webgpu=disabled',
    'allegro5=disabled',
    
    'win=disabled',
    'osx=disabled',
    'glfw=disabled',
    
    'opengl=enabled',
    'sdl2=enabled',
    'default_library=static'
])
video_deps += imgui_subp.get_variable('imgui_dep')
#video_deps += dependency('imgui', required : true, fallback : ['imgui','imgui_dep'])
video_deps += dependency('gl', required : true)
video_deps += dependency('epoxy', required: true)
video_deps += CC.find_library('dl', required : false)

#create the video configuration file
video_conf_data = configuration_data()
video_conf_data.set('HAS_FFMPEG_SUPPORT', true)
video_conf_data.set('HAS_SDL_SUPPORT', true)
video_conf_data.set('ENABLE_VSYNC', get_option('video_use_vsync'))
video_conf_data.set('EXP_ENABLE_MULTI_VIEWPORT', get_option('experimental_multi_viewport'))
video_conf_data.set('LIBTRAINSIM_PREFORMANCE_VIDEO', get_option('video_performance_mode'))
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
