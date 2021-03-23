# Define project name
project(test_server)


# If the package has been found, several variables will
# be set, you can find the full list with descriptions
# in the OpenCVConfig.cmake file.
# Print some message showing some of them



include_directories(
    ${LIBEVENT_INCLUDE_DIR}
 )

#link_directories(
#    H:/study/libevent-2.1.12-stable-build/lib/Release
#)

message("status:"${LIBEVENT_INCLUDE_DIR})
message("1111:"${LIBEVENT_SHARED_LIBRARIES})
message("2222:"${LIBEVENT_LIBRARIES})


add_executable(${PROJECT_NAME} test_server.cpp)

#  core        - the core functons of libevent
#  extra       - extra functions, contains http, dns and rpc
#  pthreads    - multiple threads for libevent, not exists on Windows
#  openssl     - openssl support for libevent
target_link_libraries(${PROJECT_NAME}
    ${LIBEVENT_LIBRARIES}
    #libevent::core
    #event
    #event_extra
    #event_openssl

)

