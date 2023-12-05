add_executable(SlideShow SlideShow.cpp)
target_link_directories(SlideShow PRIVATE ${SDK_LIBRARY_DIR})
target_link_libraries(SlideShow PRIVATE ${SDK_LIBRARIES})

add_executable(Canvas Canvas.cpp)
target_link_directories(Canvas PRIVATE ${SDK_LIBRARY_DIR})
target_link_libraries(Canvas PRIVATE ${SDK_LIBRARIES})