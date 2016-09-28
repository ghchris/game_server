
#set the final compile target
TARGET = pdk_poker

PREFIX_BIN =

ifeq (0,${debug}) 
	CPPFLAGS = -DNDEBUG -g2 -O2 -Wall
else
	CPPFLAGS = -ggdb -Wall -D_XPOKER_TEST_ -D_ENDABLE_TIME_TRACER_
endif  

#set the c compile
CC=gcc
CFLAGS= -g2 -Wall

#set the cpp compile
CXX=g++  

CPPFLAGS += -std=c++11 -DTIXML_USE_STL -I/usr/local/include/ 
CPPFLAGS += -I/usr/local/mysql/include/

LIBS = -Wl,-dn -lassistx2_core -lassistx2_db -lassistx2_json -lassistx2_cache -Wl,-dy 
LIBS += -lgflags -lglog -lprotobuf -lcpp_redis -ltinyxml -lboost_thread -lboost_system -lboost_date_time -lsnappy 
LIBS += -lmemcached -lmysqlclient -lboost_regex -ljson_spirit 

LINKFLAGS = -L/usr/local/mysql/lib
LINKFLAGS += -L/usr/local/lib


#set the include dirs
INCLUDEDIRS = ./ ./src/ ./db/ ./scene/ ./datastruct/
#INCLUDEDIRS += /home/poker/lib/mysql-connector-c-6.0.2-linux-sles10-x86-64bit/include/
INCLUDES = $(foreach tmp, $(INCLUDEDIRS), -I $(tmp))

#set my source dirs
MYSOURCEDIRS = ./ ./src/ ./db/ ./scene/ ./datastruct/

SOURCEDIRS = $(MYSOURCEDIRS)


C_SOURCES = $(foreach tmp, $(SOURCEDIRS), $(wildcard $(tmp)*.c))
C_OBJS = $(patsubst %.c, %.o, $(C_SOURCES))


#CPP_SOURCES = $(foreach tmp, $(SOURCEDIRS), $(wildcard $(tmp)*.cpp))
#CPP_OBJS = $(patsubst %.cpp, %.o, $(CPP_SOURCES))

CPP_SOURCES = $(foreach tmp, $(SOURCEDIRS), $(wildcard $(tmp)*.cpp) $(wildcard $(tmp)*.cc))
CPP_OBJS = $(patsubst %.cpp, %.o, $(patsubst %.cc,%.o,$(CPP_SOURCES)) )

all:compile
.PHONY :all

.c.o:
	$(CC) -c -o $*.o $(CFLAGS) $(INCLUDES) $*.c
.cpp.o:
	$(CXX) -c -o $*.o $(CPPFLAGS) $(INCLUDES) $*.cpp

.cc.o:
	$(CXX) -c -o $*.o $(CPPFLAGS) $(INCLUDES) $*.cc

compile: $(CPP_OBJS) $(C_OBJS) $(OTHERS_C_OBJS) $(OTHERS_CPP_OBJS)
	$(CXX) $(LINKFLAGS) -o $(TARGET) $^ $(LIBS)

clean:
	rm -f $(CPP_OBJS) $(C_OBJS)
	rm -f $(TARGET)

install: $(TARGET)
	cp $(TARGET) $(PREFIX_BIN)

uninstall:
	rm -f $(PREFIX)/$(PREFIX_BIN)

rebuild: clean




