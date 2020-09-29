project(
    'libtrainsim', 
    'cpp',
    default_options : [
        'cpp_std=c++17'
    ],
    version: '0.3.0'
)

subdir('core')
meson.override_dependency('libtrainsim-core', libtrainsim_core_dep)

subdir('video')
meson.override_dependency('libtrainsim-video', libtrainsim_video_dep)

subdir('control')
meson.override_dependency('libtrainsim-control', libtrainsim_control_dep)
