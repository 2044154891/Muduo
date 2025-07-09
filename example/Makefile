CXX = g++
CXXFLAGS = -std=c++11 -Wall -g
INCLUDES = -I/usr/include/Muduo
LIBS = -L/usr/lib -lMuduo -lpthread

TARGET = echo_server
SRCS = echo_server.cpp

all: $(TARGET)

$(TARGET): $(SRCS)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -o $@ $^ $(LIBS)

clean:
	rm -f $(TARGET) 