CXX = g++
CXXFLAGS = -std=c++11 -Wall
LDLIBS = -lprotobuf
TARGET = hearth

SOURCES = $(wildcard *.cc)
HEADERS = $(wildcard *.h)
OBJECTS = $(SOURCES:.cc=.o)

$(TARGET) : $(OBJECTS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJECTS) $(LDLIBS)

%.pb.cc : %.proto
	protoc --cpp_out=. $<

%.o : %.cc
	$(CXX) $(CXXFLAGS) -c $<

clean :
	rm *.o $(TARGET)

lint :
	clang-format-3.8 -i -style=google $(SOURCES) $(HEADERS)
