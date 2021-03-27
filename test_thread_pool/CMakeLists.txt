# Define project name
project(test_thread_pool)


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

set(SRC
    test_thread_pool.cpp
    XThread.h
    XThread.cpp
    XThreadPool.h
    XThreadPool.cpp
    XTask.h
    XTask.cpp
    XFtpServerCMD.h
    XFtpServerCMD.cpp
    
)


add_executable(${PROJECT_NAME} ${SRC})

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

