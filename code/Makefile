CXX = g++
PROJECT_NAME = libnetcore.so

#目录配置

WORK_PATH:=./
OUTPUT_PATH:=$(WORK_PATH)

#所有的代码文件
CPP_SRCS+=$(wildcard $(WORK_PATH)*.cpp)

#编译参数
CXXFLAGS := -m64 -pg -g -Wall -D_GNU_SOURCE -funroll-loops -MMD -D_REENTRANT -std=c++11 -fPIC -D__LINUX__

PROJECT_INC+=$(WORK_PATH)

#编译参数
INCLUDE_PATH := -I$(PROJECT_INC)
LIBS:= -pthread

#生成参数
APP_NAME:=$(PROJECT_NAME)
APP = $(OUTPUT_PATH)$(APP_NAME)

all: $(APP) 

#$(APP) : $(CPP_SRCS:.cpp=.o)
#	$(CXX) -pg -o $@ $^ $(CXXFLAGS) $(LIBS)
$(APP) : $(CPP_SRCS:.cpp=.o)
	ar -o $@ $^ 
	
%.o: %.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDE_PATH) -c -pg -o $@ $<

clean:
	-$(RM) $(CPP_SRCS:.cpp=.o) $(CPP_SRCS:.cpp=.d)
	-$(RM) -f $(APP)

.PHONY: so clean config