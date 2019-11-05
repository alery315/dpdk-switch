cmake_minimum_required(VERSION 3.14)
project(dpdk-switch C)

set(CMAKE_C_STANDARD 99)

add_executable(dpdk-switch main.c forwarding.c main.h receive.c transmit.c init.c config.c output_queue.c test.c log.c)


set(INC_DIR /home/alery/dpdk-stable-17.08.1/x86_64-native-linuxapp-gcc/include)
set(LINK_DIR /home/alery/dpdk-stable-17.08.1/x86_64-native-linuxapp-gcc/lib)

include_directories(${INC_DIR})
link_directories(${LINK_DIR})
link_libraries(rte)

target_link_libraries(dpdk-switch rte)