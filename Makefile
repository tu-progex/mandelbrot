CC=gcc
COPT=-O2 -lX11 -lm -fopenmp

.phony: all
all: mandel mandel-dyn

mandel: mandelbrot.c
	$(CC) -o $@ $< $(COPT)

mandel-dyn: mandelbrot.c
	$(CC) -o $@ $< -DOMPDYNAMIC $(COPT)