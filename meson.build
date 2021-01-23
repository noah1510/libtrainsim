project(
    'libtrainsim',
    'cpp',
    default_options : ['cpp_std=c++17'],
    version : '0.4.0'
)

#set the configuration
conf_data = configuration_data()
conf_data.set('HAS_CORE_SUPPORT', true)

if get_option('build_video') or get_option('build_full')
    conf_data.set('HAS_VIDEO_SUPPORT', true)
endif

if get_option('build_control') or get_option('build_full')
    conf_data.set('HAS_CONTROL_SUPPORT', true)
endif

if get_option('build_physics') or get_option('build_full')
    conf_data.set('HAS_PHYSICS_SUPPORT', true)
endif

configure_file(output : 'libtrainsim_config.hpp', configuration : conf_data)

#compile modules
subdir('core')
meson.override_dependency('libtrainsim-core', libtrainsim_core_dep)

if get_option('build_video') or get_option('build_full')
    subdir('video')
    meson.override_dependency('libtrainsim-video', libtrainsim_video_dep)
endif

if get_option('build_control') or get_option('build_full')
    subdir('control')
    meson.override_dependency('libtrainsim-control', libtrainsim_control_dep)
endif

if get_option('build_physics') or get_option('build_full')
    subdir('physics')
    meson.override_dependency('libtrainsim-physics', libtrainsim_physics_dep)
endif

#declare the dependency libtrainsim_full
if get_option('build_full')
    libtrainsim_full_dep = declare_dependency(
        dependencies : [
            dependency('libtrainsim-core', required: true, version : '=' + meson.project_version()),
            dependency('libtrainsim-video', required: true, version : '=' + meson.project_version()),
            dependency('libtrainsim-control', required: true, version : '=' + meson.project_version()),
            dependency('libtrainsim-physics', required: true, version : '=' + meson.project_version()),
        ],
        version: meson.project_version()
    )
    meson.override_dependency('libtrainsim-full', libtrainsim_full_dep)
endif
