cmake_minimum_required(VERSION 3.12)
project(IMUtest)

set(BUILD_DIR .)
message(${BUILD_DIR})

set(CMAKE_C_STANDARD 99)

include_directories(./Timer)
include_directories(./)
include_directories(./App)
include_directories(./CAN)
include_directories(./Math)
include_directories(./MPI204A)
include_directories(./Odom_Calib)
include_directories(./PGV150)
include_directories(./UART)
include_directories(./TCP)



add_executable(IMUtest main.c ./TCP/TCP.c ./PGV150/PGV150.c ./UART/UART.c ./CAN/can.c ./Math/Trigonometric.c ./App/Interpolation.c ./App/Movement.c ./MPI204A/MPI204A.c ./Odom_Calib/Odom_Calib.c ./Timer/Timer.c TCP/TCP.c TCP/TCP.h TCP/1200.c TCP/1200.h TCP/net.h TCP/buffer.h TCP/protocol.h Tools/Tools.c Tools/Tools.h include/include.h include/dataType.h include/include.h include/dataType.h include/user_function.h App/AGVControl.c AGV/agv.h App/motion_ctrl.c App/motion_ctrl.h App/AGVControl.h AGV/agv.c)

target_link_libraries(IMUtest pthread m rt)

#add_custom_target(IMUtest COMMAND make WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})