OBJS = main.cpp
CC = g++
COMPILER_FLAGS = -w

LINKER_FLAGS = -lSDL2
OBJ_NAME = main 

ADDITIONAL_LIB = -lSDL2_image
all : $(OBJS)
	$(CC) $(OBJS) $(COMPILER_FLAGS) $(LINKER_FLAGS) $(ADDITIONAL_LIB) -o $(OBJ_NAME)
