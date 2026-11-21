# Libraries
httplib_dep = dependency('cpp-httplib', required: true)
pugixml_dep = dependency('pugixml', required : true)
spdlog_dep = dependency('spdlog', required : true)

threads_dep = dependency('threads')

backend_inc = include_directories('include')
backend_src = files('src/IMetricCollector.cpp')
backend_deps = [
    httplib_dep,
    pugixml_dep,
    spdlog_dep,
    threads_dep
]
backend_dep = declare_dependency(
    sources: backend_src,
    include_directories: backend_inc,
    dependencies: backend_deps
)

executable('metrics-service',
  'src/main.cpp',
  dependencies: backend_dep
)

subdir('test')
