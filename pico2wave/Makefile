CC = g++
CFLAGS = -fPIC -g -O2 -Wall -Wextra
RM = rm -f

INCS = -I./include -I../compat/include -I../libsvoxpico/include
LIBS = -L../libsvoxpico/obj -lsvoxpico -lexpat

SRCDIR = ./src
OBJDIR = ./obj
BINDIR = ./bin

TARGET = pico2wave

SRCS = $(wildcard $(SRCDIR)/*.cpp)
OBJS = $(SRCS:$(SRCDIR)/%.cpp=$(OBJDIR)/%.o) 

.PHONY: all
all: 
	@mkdir -p $(BINDIR)
	$(CC) $(SRCS) $(INCS) $(LIBS) -fpermissive -o $(BINDIR)/$(TARGET)
	$(CC) $(SRCS) $(INCS) -static $(LIBS) -fpermissive -o $(BINDIR)/$(TARGET)-static

.PHONY: clean
clean:
	$(RM) $(BINDIR)/$(TARGET)
	$(RM) $(BINDIR)/$(TARGET)-static
	@rmdir $(BINDIR)
