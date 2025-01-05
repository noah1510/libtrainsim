cxx = meson.get_compiler('cpp')

sources = [
    'src/train_properties.cpp',
    'src/track_configuration.cpp',
    'src/version.cpp',
    'src/simulator_config.cpp',
]

cxx_header_list = [
    'filesystem',
    'algorithm',
    'string',
    'vector',
    'sstream',
    'tuple',
    'iostream',
    'cstdint',
    'fstream',
    'cmath',
    'shared_mutex',
    'mutex',
    'future',
    'chrono',
    'ratio',
    'map',
    'optional',
]

foreach header_name : cxx_header_list
    cxx.check_header(header_name, required: true)
endforeach

incdir = include_directories('include','..')

simplegfx_project = subproject(
    'simplegfx',
    default_options: [
        'require_gl_support=enabled',
        'require_ui_support=enabled',
    ]
)

core_deps = [
    dependency('threads'),
    dependency('unit-system', required: true, fallback: 'unit-system'),
    dependency('sqlitecpp', required: true, fallback: 'sqlitecpp'),
    simplegfx_project.get_variable('simplegfx_dep'),
]

libtrainsim_core = library(
    'trainsim-core', 
    sources, 
    version : meson.project_version(), 
    soversion : '0',
    include_directories : incdir,
    dependencies: core_deps,
    install : true,
    cpp_pch : 'include/pch/libtrainsim_core_pch.hpp',
)

libtrainsim_core_dep = declare_dependency(
    include_directories : incdir,
    link_with : libtrainsim_core,
    dependencies : core_deps,
    version: meson.project_version(),
)

install_subdir('include', install_dir : 'include/libtrainsim', strip_directory : true)

pkg = import('pkgconfig')
pkg.generate(libtrainsim_core)

if get_option('force_build_tests') or (get_option('build_tests') and not meson.is_subproject())
    subdir('tests')
endif
