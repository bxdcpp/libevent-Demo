# cmake needs this line
cmake_minimum_required(VERSION 2.8)

# Define project name
project(libevent_demo)


# If the package has been found, several variables will
# be set, you can find the full list with descriptions
# in the OpenCVConfig.cmake file.
# Print some message showing some of them

set(libevent_DIR H:/study/libevent-install/lib/cmake)


find_package(Libevent 2.1 REQUIRED COMPONENTS core)

#find_package(Libevent REQUIRED)

if(WIN32)
add_definitions(
	#-D_DEBUG
	#-D_UNICODE#设置Unicode字符集
	#-DESLOG_NOT_LOG_ERR
	#-D_WINDOWS
	-D_CRT_SECURE_NO_WARNINGS)
endif()
set(libevnet_APPS
    test_server
)

#-----------------------------------------------------------------------------
# 添加App
# ---------------------------------------------------------------------------
foreach(app ${libevnet_APPS})
  message(STATUS "---------------------------Add app " ${app})
  add_subdirectory(${app})
endforeach()

