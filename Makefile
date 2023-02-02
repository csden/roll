
GCC   ?= gcc
PNAME ?= roll

clean:
	if [ -f $(PNAME) ]; then \
		rm $(PNAME); \
	fi

build:
	$(GCC) src/roll.c -lpulse-simple -lpulse -lm -o roll

all: clean build
