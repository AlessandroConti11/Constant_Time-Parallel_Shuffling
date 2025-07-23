# Executable name
EXECUTABLE = djb

# Compiler
CC = gcc
CFLAGS = -std=c11 -Wall -Werror -Wextra -O2 -fopenmp

# Source directories
SRC_DIRS = utility \
			insertion_series \
			constant-weight_words

# Source files
SRC = main.c \
      $(wildcard $(patsubst %, %/*.c, $(SRC_DIRS)))
#      insertionSeries.c \

# Object files
OBJ = $(SRC:.c=.o)

# Main rule
all: $(EXECUTABLE)

# Executable creation rule
$(EXECUTABLE): $(OBJ)
	$(CC) $(CFLAGS) $^ -o $@

# Rule for compiling .c files into .o
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Clean rule
clean:
	rm -f $(OBJ) $(EXECUTABLE)
