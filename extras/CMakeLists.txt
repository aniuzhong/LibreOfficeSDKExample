add_executable(show_slides show_slides.cpp)
target_link_directories(show_slides PRIVATE ${SDK_LIBRARY_DIR})
target_link_libraries(show_slides PRIVATE ${SDK_LIBRARIES})