LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)
LOCAL_MODULE := libnetcore
LOCAL_CPPFLAGS += -D__cplusplus=201103L
LOCAL_CPPFLAGS += -D__LINUX__

LOCAL_SRC_FILES := \
	CAcceptor.cpp \
	CArray.cpp \
	CBaseConnection.cpp \
	CBaseSocket.cpp \
	CBaseWorker.cpp \
	CBuffer.cpp \
	CCommuniter.cpp \
	CCommuniterGroups.cpp \
	CCommuniterMgr.cpp \
	CConnection.cpp \
	CEasyLog.cpp \
	CMessage.cpp \
	CNetCoreInterface.cpp \
	CNetWork.cpp \
	CNodeManager.cpp \
	CSingleton.cpp \
	CSocketInterface.cpp \
	CTimer.cpp \
	CTools.cpp \
	NetWorkSerilize.cpp
	

include $(BUILD_SHARED_LIBRARY)