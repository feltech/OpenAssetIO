add_executable(perftest
  perftest.cpp)

set_default_target_properties(perftest)
install(TARGETS perftest EXPORT ${PROJECT_NAME}_EXPORTED_TARGETS)

target_link_libraries(perftest
  PRIVATE
  openassetio-core)
