CC = gcc
CFLAGS = -fPIC -g -O2 -Wall -Wextra
LD = gcc
LDFLAGS = -shared
RM = rm -f

INCS = -I./include
LIBS = -lm

SRCDIR = ./src
OBJDIR = ./obj

TARGET = libsvoxpico

SRCS = $(wildcard $(SRCDIR)/*.c)
OBJS = $(SRCS:$(SRCDIR)/%.c=$(OBJDIR)/%.o) 

.PHONY: all
all: $(TARGET)

$(TARGET): $(OBJS)
	ar rcs $(OBJDIR)/$@.a $(OBJS)
	$(CC) $(LDFLAGS) $(LIBS) $(OBJS) -o $(OBJDIR)/$@.so

$(OBJS): $(OBJDIR)/%.o : $(SRCDIR)/%.c
	@echo Compiling $< into $@
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) $(INCS) -c $< -o $@

.PHONY: clean
clean:
	$(RM) $(OBJDIR)/*
	@rmdir $(OBJDIR)
