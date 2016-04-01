#OBJS specifies which files to compile as part of the project 
OBJS = SDL_Handler.cpp Player.cpp Map.cpp main.cpp 

#OBJ_NAME specifies the name of our exectuable 
OBJ_NAME = main 

#CC specifies which compiler we're using 
CC = g++

#COMPILER_FLAGS specifies the additional compilation options we're using 
# -w suppresses all warnings 

COMPILER_FLAGS = -w -std=c++11

#INC_DIR specifies the directory with all the header files to include
INC_DIR = Headers/

#LINKER_FLAGS specifies the libraries we're linking against 
LINKER_FLAGS = -lSDL2 

#This is the target that compiles our executable
all : $(OBJS) 
	$(CC) $(OBJS) -I$(INC_DIR) $(COMPILER_FLAGS) $(LINKER_FLAGS) -o $(OBJ_NAME) 