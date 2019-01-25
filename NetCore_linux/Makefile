CXX = g++
PROJECT_NAME = netcore

#目录配置

WORK_PATH:=./
CODE_PATH:=./../code/
OUTPUT_PATH:=$(WORK_PATH)

#所有的代码文件
CPP_SRCS+=$(wildcard $(WORK_PATH)*.cpp )
CPP_SRCS+=$(wildcard $(CODE_PATH)*.cpp )

#编译参数
CXXFLAGS := -m64 -pg -g -Wall -D_GNU_SOURCE -funroll-loops -MMD -D_REENTRANT -std=c++11 -fPIC -D__LINUX__

PROJECT_INC+=$(WORK_PATH)

#编译参数
INCLUDE_PATH := -I$(PROJECT_INC) -I$(CODE_PATH) -I/opt/jsoncpp/include/
LIBS:= -pthread -L/opt/jsoncpp/lib -ljsoncpp

#生成参数
APP_NAME:=$(PROJECT_NAME)
APP = $(OUTPUT_PATH)$(APP_NAME)

all: $(APP) 

$(APP) : $(CPP_SRCS:.cpp=.o)
	$(CXX) -pg -o $@ $^ $(CXXFLAGS) $(LIBS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDE_PATH) -c -pg -o $@ $<

clean:
	-$(RM) $(CPP_SRCS:.cpp=.o) $(CPP_SRCS:.cpp=.d)
	-$(RM) -f $(APP)

.PHONY: so clean config