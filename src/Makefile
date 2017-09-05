CXX = g++
CXXFLAGS = -std=c++11 -Wall
LDLIBS = -lprotobuf
TARGET = hearth

SOURCES = $(wildcard *.cc)
PROTO_SRCS = $(wildcard *.pb.cc)
LINT_SRCS = $(filter-out tinyxml2.cc $(PROTO_SRCS), $(SOURCES))

HEADERS = $(wildcard *.h)
PROTO_HDRS = $(wildcard *.pb.h)
LINT_HDRS = $(filter-out tinyxml2.h $(PROTO_HDRS), $(HEADERS))
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
	clang-format-3.8 -i -style=google $(LINT_HDRS) $(LINT_SRCS)
