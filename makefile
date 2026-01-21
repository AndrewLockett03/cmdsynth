CXX=g++

TARGET=cmdsynth

TARGET_DEL=cmdsynth

SRCS=cmdsynth.cpp LowPassFilter.cpp main.cpp

OBJS = $(SRCS:.cpp=.o)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm $(TARGET_DEL) $(OBJS)
