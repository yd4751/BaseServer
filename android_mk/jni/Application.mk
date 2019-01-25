APP_ABI := armeabi-v7a
#APP_STL := gnustl_static

APP_STL := gnustl_static #GNU STL  
APP_CPPFLAGS := -fexceptions -frtti #允许异常功能，及运行时类型识别  
APP_CPPFLAGS +=-std=c++11 #允许使用c++11的函数等功能  
APP_CPPFLAGS +=-fpermissive -D__LINUX__ -Wno-switch -Wno-header-guard
APP_CPPFLAGS += -lpthread

#LOCAL_C_INCLUDES :=$(LOCAL_PATH) E:\SoftInstall\AndroidDev\NDK\android-ndk-r16b\sources\cxx-stl\gnu-libstdc++\4.9\include
