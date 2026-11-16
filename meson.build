project('metrics-visualizer', 'cpp',
    default_options: ['cpp_std=c++17', 'warning_level=2']
)

# Libraries
httplib_dep = dependency('cpp-httplib')
pugixml_dep = dependency('pugixml', required : true)
spdlog_dep = dependency('spdlog', required : true)

executable('metrics-service',
  'backend/src/main.cpp',
  include_directories: include_directories('backend/include'),
  dependencies: [httplib_dep, pugixml_dep, spdlog_dep]
)