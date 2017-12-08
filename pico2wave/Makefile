CC = g++
CFLAGS = -fPIC -g -O2 -Wall -Wextra -DDEBUG
RM = rm -f

INCS = -I./include -I../compat/include -I../libsvoxpico/include
LIBS = -L../libsvoxpico/obj -lsvoxpico -lexpat -lFLAC -logg -lvorbis -lvorbisenc -lvorbisfile -lsndfile

SRCDIR = ./src
OBJDIR = ./obj
BINDIR = ./bin

TARGET = pico2wave

SRCS = $(wildcard $(SRCDIR)/*.cpp)
OBJS = $(SRCS:$(SRCDIR)/%.cpp=$(OBJDIR)/%.o) 

.PHONY: all
all: 
	@mkdir -p $(BINDIR)
	$(CC) -DDEBUG $(SRCS) $(INCS) $(LIBS) -fpermissive -o $(BINDIR)/$(TARGET)

.PHONY: clean
clean:
	$(RM) $(BINDIR)/$(TARGET)
	@rmdir $(BINDIR)
