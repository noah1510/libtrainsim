cc = meson.get_compiler('cpp')

sources = [
    'src/track_data.cpp',
    'src/train_properties.cpp',
    'src/track_configuration.cpp',
    'src/types.cpp',
]

incdir = include_directories('include','..')

core_deps = [
    dependency('threads'),
    dependency('nlohmann_json', required: true, fallback: ['nlohmann_json','nlohmann_json_dep']),
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
