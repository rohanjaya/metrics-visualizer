catch_dep = dependency('catch2-with-main', default_options: ['tests=false'])
test_deps = [
    backend_dep,
    catch_dep
]

backend_test = executable('backend-test',
  'IMetricCollector_test.cpp',
  dependencies: test_deps
)

test('backend-test', backend_test, timeout: 30)
