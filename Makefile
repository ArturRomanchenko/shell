# Compiler
CC = gcc
# Compiler flags
CFLAGS = -Wall -Wextra -Wpedantic -std=c99

# Source files
SRCS = shell.c utils.c
# Object files directory
OBJDIR = build
# Object files
OBJS = $(addprefix $(OBJDIR)/, $(SRCS:.c=.o))
# Executable name
EXEC = shell

# Default target
all: $(EXEC)

# Compile source files into object files
$(OBJDIR)/%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Link object files into executable
$(EXEC): $(OBJS)
	$(CC) $(OBJS) -o $(EXEC)

# Clean up
clean:
	rm -f $(OBJS) $(EXEC)
