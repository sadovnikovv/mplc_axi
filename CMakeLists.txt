project(AXI)
cmake_minimum_required(VERSION 3.3)
SET(CMAKE_CXX_FLAGS "-std=c++11")
add_definitions(-DLINUX -D_GNUC_)
if(WIN32)
    set(PATH_TO_MPLC_LIBRARY
            "/mnt/usb/files/c++/linux") # ПОМЕНЯТЬ НА СВОЙ

    set(PATH_TO_MPLC_INCLUDES
            /mnt/usb/files/c++) # ПОМЕНЯТЬ НА СВОЙ
else()
    set(CMAKE_POSITION_INDEPENDENT_CODE ON)
    set(PATH_TO_MPLC_LIBRARY
            "/mnt/usb/files/c++/linux") # ПОМЕНЯТЬ НА СВОЙ

    set(PATH_TO_MPLC_INCLUDES
            /mnt/usb/files/c++) # ПОМЕНЯТЬ НА СВОЙ
#link_directories(${PATH_TO_MPLC_LIBRARY})
endif()

include_directories(
        ${PATH_TO_MPLC_INCLUDES}/include/core
        ${PATH_TO_MPLC_INCLUDES}/include
        ${PATH_TO_MPLC_INCLUDES}/lib
        ${PATH_TO_MPLC_INCLUDES}/lib/opcua)

#set(MPLCLIBS masterplc opcua mplcshare) # mplcshare)
#set(SYSLIBS pthread rt dl)
#foreach (MPLCLIB ${MPLCLIBS})
#        add_library(${MPLCLIB} SHARED IMPORTED)
#        set_property(TARGET ${MPLCLIB} PROPERTY
#                    IMPORTED_LOCATION ${MPLCLIB}.so)
#endforeach ()

set(INCLUDES
		gpio_protocol.h
		types.h
      		stdafx.h)

set(SOURCES
        gpio_protocol.cpp
        mplc_gpio.cpp)
add_library(mplc_gpio SHARED ${SOURCES} ${INCLUDES})




#target_link_libraries(mplc_gpio ${MPLCLIBS} ${SYSLIBS})

#target_link_libraries(mplc_gpio ${MPLCLIBS} ${SYSLIBS})



SET_TARGET_PROPERTIES(mplc_gpio PROPERTIES PREFIX "")
