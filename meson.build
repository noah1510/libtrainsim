project(
    'libtrainsim', 
    'cpp',
    default_options : [
        'cpp_std=c++17'
    ],
    version: '0.1.0'
)

subdir('video')
subdir('control')

meson.override_dependency('libtrainsim-video', libtrainsim_video_dep)
meson.override_dependency('libtrainsim-control', libtrainsim_control_dep)

