add_executable(Desk Desk.cpp)
target_link_directories(Desk PRIVATE ${SDK_LIBRARY_DIR})
target_link_libraries(Desk PRIVATE ${SDK_LIBRARIES})
