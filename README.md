OpenMP program to draw the Mandelbrot set
mandel: Parallelized using #pragma omp for
mandel-dyn: Parallelized using #pragma omp schedule(dynamic,...)

The X11 library is required for compilation. On Ubuntu, you can install the necessary libraries with:
sudo apt install make libx11-dev

After installing the required libraries, you can compile the program by running:
make
