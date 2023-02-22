cxx = meson.get_compiler('cpp')

sources = [
    'src/track_data.cpp',
    'src/train_properties.cpp',
    'src/track_configuration.cpp',
    'src/types.cpp',
    'src/input_axis.cpp',
    'src/simulator_config.cpp',
    'src/helper.cpp',
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

core_deps = [
    dependency('threads'),
    dependency('unit-system', required: true, fallback: 'unit-system'),
    dependency('simplegl', required: true, fallback: 'simplegl', version : '>=0.1.0'),
]

libtrainsim_core = library(
    'trainsim-core', 
    sources, 
    version : meson.project_version(), 
    soversion : '0',
    include_directories : incdir,
    dependencies: core_deps,
    install : true,
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
