cmake_minimum_required(VERSION 3.0.0)
project(hp_cloud_player VERSION 0.1.0)

include(CTest)
enable_testing()

add_library(hp_cloud_player hp_cloud_player.cpp 
            smultipart_parser.cpp
            scookies.cpp     shttpproxies.cpp
            scurlclient.cpp  shttputil.cpp
            scloud_vm.cpp          scurlholder.cpp  smultipart_parser.cpp
            scloud_xml_parser.cpp  shttperror.cpp
            )

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
