add_executable(Organigram Organigram.cpp)
target_link_directories(Organigram PRIVATE ${SDK_LIBRARY_DIR})
target_link_libraries(Organigram PRIVATE ${SDK_LIBRARIES})
