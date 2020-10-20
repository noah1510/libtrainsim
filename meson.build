project(
    'libtrainsim',
    'cpp',
    default_options : ['cpp_std=c++17', 'b_lto=true', 'b_pgo=generate', 'b_coverage=true'],
    version : '0.3.1'
)

subdir('core')
meson.override_dependency('libtrainsim-core', libtrainsim_core_dep)

if get_option('build_video')
    subdir('video')
    meson.override_dependency('libtrainsim-video', libtrainsim_video_dep)
endif

if get_option('build_control')
    subdir('control')
    meson.override_dependency('libtrainsim-control', libtrainsim_control_dep)
endif
