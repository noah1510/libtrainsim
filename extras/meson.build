extras_deps = []

if get_option('build_extras_statusDisplay') or get_option('build_full')
    subdir('statusDisplay')
    meson.override_dependency('trainsim-extras-statusDisplay', libtrainsim_statusDisplay_dep)
endif


libtrainsim_extras_dep = declare_dependency(
    dependencies : [
        dependency('trainsim-extras-statusDisplay', required: get_option('build_extras_statusDisplay'), version : '=' + meson.project_version()),
    ],
    version: meson.project_version()
)
