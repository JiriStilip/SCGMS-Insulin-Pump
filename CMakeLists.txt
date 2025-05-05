CMAKE_MINIMUM_REQUIRED(VERSION 3.5)

PROJECT(scgms_rpi CXX C ASM)

SET(LINKER_SCRIPT "${CMAKE_CURRENT_SOURCE_DIR}/sources/Rpi/raspberrypi.ld")

set(CMAKE_INCLUDE_CURRENT_DIR ON)

SET(CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH};${CMAKE_CURRENT_SOURCE_DIR}")
INCLUDE(arm_none_eabi)

set( CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O0" )
set( CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g" )
set( CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wno-unknown-pragmas -fcompare-debug-second -Wno-unused-parameter -Wno-cast-function-type" )
set( CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wextra --specs=nosys.specs  -specs=nano.specs -nostartfiles" )
set( CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -march=armv6zk" )
set( CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -mtune=arm1176jzf-s" )

set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --specs=nosys.specs -nostartfiles -specs=nano.specs" )
set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O0" )
set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g" )
set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-unknown-pragmas -fcompare-debug-second -Wno-unused-parameter -Wno-cast-function-type" )
set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -march=armv6zk" )
set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mtune=arm1176jzf-s" )
set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-exceptions" )

set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -L /usr/lib/gcc/arm-none-eabi/6.3.1 -lgcc")
SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -T ${LINKER_SCRIPT}")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -L /usr/lib/arm-none-eabi-new/gcc-arm-none-eabi-10.3-2021.10/arm-none-eabi/lib")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -lstdc++ -Wl,--strip-all")

INCLUDE_DIRECTORIES("${CMAKE_CURRENT_SOURCE_DIR}/sources/FreeRTOS/portable/GCC/RaspberryPi/")
INCLUDE_DIRECTORIES("${CMAKE_CURRENT_SOURCE_DIR}/sources/FreeRTOS/freertos/")
INCLUDE_DIRECTORIES("${CMAKE_CURRENT_SOURCE_DIR}/sources/FreeRTOS/")
INCLUDE_DIRECTORIES("${CMAKE_CURRENT_SOURCE_DIR}/sources/Rpi/Drivers/")
INCLUDE_DIRECTORIES("${CMAKE_CURRENT_SOURCE_DIR}/sources/Rpi/")
INCLUDE_DIRECTORIES("${CMAKE_CURRENT_SOURCE_DIR}/sources/SmartCGMS/scgms/")
INCLUDE_DIRECTORIES("${CMAKE_CURRENT_SOURCE_DIR}/sources/SmartCGMS/")
INCLUDE_DIRECTORIES("${CMAKE_CURRENT_SOURCE_DIR}/sources/filters/")
INCLUDE_DIRECTORIES("${CMAKE_CURRENT_SOURCE_DIR}/sources/")

INCLUDE_DIRECTORIES("${CMAKE_CURRENT_SOURCE_DIR}/sources/FreeRTOS-POSIX/include")
INCLUDE_DIRECTORIES("${CMAKE_CURRENT_SOURCE_DIR}/sources/FreeRTOS-POSIX/include/private")
INCLUDE_DIRECTORIES("${CMAKE_CURRENT_SOURCE_DIR}/sources/FreeRTOS-POSIX/FreeRTOS-Plus-POSIX/include")
INCLUDE_DIRECTORIES("${CMAKE_CURRENT_SOURCE_DIR}/sources/FreeRTOS-POSIX/FreeRTOS-Plus-POSIX/include/portable")
INCLUDE_DIRECTORIES("${CMAKE_CURRENT_SOURCE_DIR}/sources/FreeRTOS-POSIX/FreeRTOS-Plus-POSIX/include/portable/Rpi")


FILE(GLOB_RECURSE freertos_src  
    "sources/FreeRTOS/*.c"
    "sources/FreeRTOS/*.h"
)

FILE(GLOB_RECURSE posix_src  
    "sources/FreeRTOS-POSIX/*.c"
    "sources/FreeRTOS-POSIX/*.h"
)

FILE(GLOB_RECURSE scgms
     "sources/SmartCGMS/*.h"
     "sources/SmartCGMS/*.cpp"
     "sources/SmartCGMS/*.c"
)

FILE(GLOB_RECURSE filters
     "sources/filters/*.h"
     "sources/filters/*.cpp"
     "sources/filters/*.c"
)

FILE(GLOB_RECURSE drivers  
    "sources/Rpi/*.c"
    "sources/Rpi/*.s"
    "sources/Rpi/*.h"
    "sources/Rpi/*.cpp"
)

ADD_EXECUTABLE(rpiscgms "sources/main.c" ${freertos_src} ${drivers}  ${posix_src} ${scgms} ${filters})
set_property(TARGET rpiscgms PROPERTY CXX_STANDARD 17)

target_compile_definitions(rpiscgms PRIVATE FREERTOS=1)
target_compile_definitions(rpiscgms PRIVATE RPI=1)
target_compile_definitions(rpiscgms PRIVATE EMBEDDED=1)


