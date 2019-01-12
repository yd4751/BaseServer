CXX = g++
PROJECT_NAME = server

#目录配置

WORK_PATH:=./
OUTPUT_PATH:=$(WORK_PATH)../bin/

#所有的代码文件
CPP_SRCS+=$(wildcard $(WORK_PATH)*.cpp)
CPP_SRCS+=$(wildcard $(WORK_PATH)Login/*.cpp)
CPP_SRCS+=$(wildcard $(WORK_PATH)RemoteServer/*.cpp)

#编译参数
CXXFLAGS := -m64 -pg -g -Wall -D_GNU_SOURCE -funroll-loops -MMD -D_REENTRANT -std=c++11 -fPIC  -fpermissive

PROJECT_INC+=$(WORK_PATH)

PARENT_PATH := ../
OTHER_INCLUDE := -I$(PARENT_PATH)CmdDefine -I$(PARENT_PATH)Core -I$(PARENT_PATH)include -I$(PARENT_PATH)Protocol -I$(PARENT_PATH)ProtocolParse \
	-I$(PARENT_PATH)Rpc -I$(PARENT_PATH)Tools
	
CPP_SRCS+=$(wildcard $(PARENT_PATH)Core/*.cpp)
CPP_SRCS+=$(wildcard $(PARENT_PATH)ProtocolParse/*.cpp)
CPP_SRCS+=$(wildcard $(PARENT_PATH)Rpc/*.cpp)
CPP_SRCS+=$(wildcard $(PARENT_PATH)Tools/*.cpp)

#编译参数
INCLUDE_PATH := -I$(PROJECT_INC) -I./RemoteServer -I./Login $(OTHER_INCLUDE)
LIBS:= -pthread -lprotobuf -lhiredis -ljson -L../bin -lnetcore

#生成参数
APP_NAME:=$(PROJECT_NAME)
APP = $(OUTPUT_PATH)$(APP_NAME)

all: $(APP) 

$(APP) : $(CPP_SRCS:.cpp=.o)
	$(CXX) -pg -o $@ $^ $(LIBS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDE_PATH) -c -pg -o $@ $<

clean:
	-$(RM) $(CPP_SRCS:.cpp=.o) $(CPP_SRCS:.cpp=.d)
	-$(RM) -f $(APP)

.PHONY: so clean config