CXX = g++
CXXFLAGS = -std=c++11 -Wall
LDLIBS = -lprotobuf
TARGET = hearth

$(TARGET) : main.o card_info.pb.o tinyxml2.o
	$(CXX) $(CXXFLAGS) -o $(TARGET) main.o card_info.pb.o tinyxml2.o $(LDLIBS)

card_info.pb.cc : card_info.proto
	protoc --cpp_out=. card_info.proto

main.o : main.cc
	$(CXX) $(CXXFLAGS) -c -o main.o main.cc

card_info.pb.o : card_info.pb.cc
	$(CXX) $(CXXFLAGS) -c -o card_info.pb.o card_info.pb.cc

tinyxml2.o : tinyxml2.cc
	$(CXX) $(CXXFLAGS) -c -o tinyxml2.o tinyxml2.cc

clean :
	rm *.o $(TARGET)
