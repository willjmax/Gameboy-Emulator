CXX = g++
CXXFLAGS = -Wall -std=c++23 -g
LDFLAGS = -lSDL3

TARGET = gameboy
OBJS = main.o gameboy.o cpu.o memory.o instruction.o

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJS) $(LDFLAGS)

main.o: main.cpp gameboy.h
	$(CXX) $(CXXFLAGS) -c main.cpp

gameboy.o: gameboy.cpp gameboy.h cpu.h memory.h
	$(CXX) $(CXXFLAGS) -c gameboy.cpp

cpu.o: cpu.cpp cpu.h memory.h
	$(CXX) $(CXXFLAGS) -c cpu.cpp

memory.o: memory.cpp memory.h
	$(CXX) $(CXXFLAGS) -c memory.cpp

instruction.o: instruction.cpp instruction.h
	$(CXX) $(CXXFLAGS) -c instruction.cpp

clean:
	rm -f $(OBJS) $(TARGET)
