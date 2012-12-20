#------------------------------------------------------------------------------#
# This makefile was generated by 'cbp2make' tool rev.137                       #
# Modified by Gergely Imreh
# TODO: simplify, clean up
#------------------------------------------------------------------------------#


WORKDIR = %cd%

CC = gcc.exe
CXX = g++.exe
AR = ar.exe
LD = g++.exe
WINDRES = windres.exe

INC =  -I .
CFLAGS =  -Wall -fexceptions
RESINC = 
LIBDIR = 
LIB = meIDSmain.lib meIDSTools.lib
LDFLAGS = 

INC_DEBUG =  $(INC)
CFLAGS_DEBUG =  $(CFLAGS) -g
RESINC_DEBUG =  $(RESINC)
RCFLAGS_DEBUG =  $(RCFLAGS)
LIBDIR_DEBUG =  $(LIBDIR) -L .
LIB_DEBUG = $(LIB)
LDFLAGS_DEBUG =  $(LDFLAGS)
OBJDIR_DEBUG = obj\\Debug
DEP_DEBUG = 
OUT_DEBUG = bin\\Debug\\test.exe

INC_RELEASE =  $(INC)
CFLAGS_RELEASE =  $(CFLAGS) -DRELEASE
RESINC_RELEASE =  $(RESINC)
RCFLAGS_RELEASE =  $(RCFLAGS)
LIBDIR_RELEASE =  $(LIBDIR)
LIB_RELEASE = $(LIB)
LDFLAGS_RELEASE =  $(LDFLAGS)
OBJDIR_RELEASE = .objs
DEP_RELEASE = 
OUT_RELEASE = bin\\Release\\test.exe

OBJ_DEBUG = $(OBJDIR_DEBUG)\\TestConsoleGeneric.o $(OBJDIR_DEBUG)\\main.o $(OBJDIR_DEBUG)\\src_timing\\CtrlPtStruct.o $(OBJDIR_DEBUG)\\src_timing\\WaveStruct.o $(OBJDIR_DEBUG)\\src_timing\\algorithm.o $(OBJDIR_DEBUG)\\stdafx.o

OBJ_RELEASE = $(OBJDIR_RELEASE)\\TestConsoleGeneric.o $(OBJDIR_RELEASE)\\main.o $(OBJDIR_RELEASE)\\src_timing\\CtrlPtStruct.o $(OBJDIR_RELEASE)\\src_timing\\WaveStruct.o $(OBJDIR_RELEASE)\\src_timing\\algorithm.o $(OBJDIR_RELEASE)\\stdafx.o

all: test debug release

test:
	@echo $(CXX) $(CFLAGS_DEBUG) $(INC_DEBUG) -c TestConsoleGeneric.cpp -o $(OBJDIR_DEBUG)\\TestConsoleGeneric.o


clean: clean_debug clean_release

before_debug: 
	cmd /c if not exist bin\\Debug md bin\\Debug
	cmd /c if not exist $(OBJDIR_DEBUG) md $(OBJDIR_DEBUG)
	cmd /c if not exist $(OBJDIR_DEBUG)\\src_timing md $(OBJDIR_DEBUG)\\src_timing

after_debug: 

debug: before_debug out_debug after_debug

out_debug: $(OBJ_DEBUG) $(DEP_DEBUG)
	$(LD) $(LDFLAGS_DEBUG) $(LIBDIR_DEBUG) $(OBJ_DEBUG) $(LIB_DEBUG) -o $(OUT_DEBUG)

$(OBJDIR_DEBUG)\\TestConsoleGeneric.o: TestConsoleGeneric.cpp
	$(CXX) $(CFLAGS_DEBUG) $(INC_DEBUG) -c TestConsoleGeneric.cpp -o $(OBJDIR_DEBUG)\\TestConsoleGeneric.o

$(OBJDIR_DEBUG)\\main.o: main.cpp
	$(CXX) $(CFLAGS_DEBUG) $(INC_DEBUG) -c main.cpp -o $(OBJDIR_DEBUG)\\main.o

$(OBJDIR_DEBUG)\\src_timing\\CtrlPtStruct.o: src_timing\\CtrlPtStruct.c
	$(CC) $(CFLAGS_DEBUG) $(INC_DEBUG) -c src_timing\\CtrlPtStruct.c -o $(OBJDIR_DEBUG)\\src_timing\\CtrlPtStruct.o

$(OBJDIR_DEBUG)\\src_timing\\WaveStruct.o: src_timing\\WaveStruct.c
	$(CC) $(CFLAGS_DEBUG) $(INC_DEBUG) -c src_timing\\WaveStruct.c -o $(OBJDIR_DEBUG)\\src_timing\\WaveStruct.o

$(OBJDIR_DEBUG)\\src_timing\\algorithm.o: src_timing\\algorithm.c
	$(CC) $(CFLAGS_DEBUG) $(INC_DEBUG) -c src_timing\\algorithm.c -o $(OBJDIR_DEBUG)\\src_timing\\algorithm.o

$(OBJDIR_DEBUG)\\stdafx.o: stdafx.cpp
	$(CXX) $(CFLAGS_DEBUG) $(INC_DEBUG) -c stdafx.cpp -o $(OBJDIR_DEBUG)\\stdafx.o

clean_debug: 
	cmd /c del /f $(OBJ_DEBUG) $(OUT_DEBUG)
	cmd /c rd bin\\Debug
	cmd /c rd $(OBJDIR_DEBUG)
	cmd /c rd $(OBJDIR_DEBUG)\\src_timing

before_release: 
	cmd /c if not exist bin\\Release md bin\\Release
	cmd /c if not exist $(OBJDIR_RELEASE) md $(OBJDIR_RELEASE)
	cmd /c if not exist $(OBJDIR_RELEASE)\\src_timing md $(OBJDIR_RELEASE)\\src_timing

after_release: 

release: before_release out_release after_release

out_release: $(OBJ_RELEASE) $(DEP_RELEASE)
	$(LD) $(LDFLAGS_RELEASE) $(LIBDIR_RELEASE) $(OBJ_RELEASE) $(LIB_RELEASE) -o $(OUT_RELEASE)

$(OBJDIR_RELEASE)\\TestConsoleGeneric.o: TestConsoleGeneric.cpp
	$(CXX) $(CFLAGS_RELEASE) $(INC_RELEASE) -c TestConsoleGeneric.cpp -o $(OBJDIR_RELEASE)\\TestConsoleGeneric.o

$(OBJDIR_RELEASE)\\main.o: main.cpp
	$(CXX) $(CFLAGS_RELEASE) $(INC_RELEASE) -c main.cpp -o $(OBJDIR_RELEASE)\\main.o

$(OBJDIR_RELEASE)\\src_timing\\CtrlPtStruct.o: src_timing\\CtrlPtStruct.c
	$(CC) $(CFLAGS_RELEASE) $(INC_RELEASE) -c src_timing\\CtrlPtStruct.c -o $(OBJDIR_RELEASE)\\src_timing\\CtrlPtStruct.o

$(OBJDIR_RELEASE)\\src_timing\\WaveStruct.o: src_timing\\WaveStruct.c
	$(CC) $(CFLAGS_RELEASE) $(INC_RELEASE) -c src_timing\\WaveStruct.c -o $(OBJDIR_RELEASE)\\src_timing\\WaveStruct.o

$(OBJDIR_RELEASE)\\src_timing\\algorithm.o: src_timing\\algorithm.c
	$(CC) $(CFLAGS_RELEASE) $(INC_RELEASE) -c src_timing\\algorithm.c -o $(OBJDIR_RELEASE)\\src_timing\\algorithm.o

$(OBJDIR_RELEASE)\\stdafx.o: stdafx.cpp
	$(CXX) $(CFLAGS_RELEASE) $(INC_RELEASE) -c stdafx.cpp -o $(OBJDIR_RELEASE)\\stdafx.o

clean_release: 
	cmd /c del /f $(OBJ_RELEASE) $(OUT_RELEASE)
	cmd /c rd bin\\Release
	cmd /c rd $(OBJDIR_RELEASE)
	cmd /c rd $(OBJDIR_RELEASE)\\src_timing

.PHONY: before_debug after_debug clean_debug before_release after_release clean_release
