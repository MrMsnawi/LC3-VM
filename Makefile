CXX = g++
CXXFLAGS = -std=c++11 -Wall -Wextra
TARGET = lc3-vm
SRCS = main.cpp lc3.cpp
OBJS = $(SRCS:.cpp=.o)

.PHONY: all clean fclean re run

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS)

fclean: clean
	rm -f $(TARGET)

re: fclean all

run: $(TARGET)
	./$(TARGET)
