add_executable(app
    main.cpp 
    client.cpp 
)

target_link_libraries(app PUBLIC Boost::boost pthread)

target_include_directories(app PUBLIC 
    ${Boost_INCLUDE_DIRS}
)