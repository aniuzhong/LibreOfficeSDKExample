add_executable(srvnm srvnm.cpp)
target_link_directories(srvnm PRIVATE ${SDK_LIBRARY_DIR})
target_link_libraries(srvnm PRIVATE ${SDK_LIBRARIES})
