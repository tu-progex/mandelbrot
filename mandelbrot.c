#include <X11/Xlib.h>
#include <stdio.h>
#include <stdlib.h>
#include <complex.h>
#include <omp.h>

// #define OMPDYNAMIC

#define WIDTH 900
#define HEIGHT 900
#define MAX_ITER 30000

long mandelbrot(double complex c) {
    double complex z = 0;
    for (int i = 0; i < MAX_ITER; i++) {
        if (cabs(z) > 2.0)
            return i;
        z = z * z + c;
    }
    return MAX_ITER;
}

int main(int argc, char **argv) {
    int num_threads = omp_get_max_threads();
    if (argc > 1) {
        num_threads = atoi(argv[1]);
    }
    if (num_threads<=0) num_threads=1;
    omp_set_num_threads(num_threads);
    printf ("Number of threads: %d\n", num_threads);

    printf ("Schedule: %s\n",
#ifndef OMPDYNAMIC
            "static"
#else
            "dynamic"
#endif
    );

    Display *display;
    Window window;
    XEvent event;
    int screen;

    display = XOpenDisplay(NULL);
    if (display == NULL) {
        fprintf(stderr, "Failed to open display.\n");
        exit(1);
    }

    screen = DefaultScreen(display);
    window = XCreateSimpleWindow(display, RootWindow(display, screen),
                                 10, 10, WIDTH, HEIGHT, 1,
                                 BlackPixel(display, screen),
                                 WhitePixel(display, screen));
    XSelectInput(display, window, ExposureMask | KeyPressMask);
    XMapWindow(display, window);

    GC gc = XCreateGC(display, window, 0, NULL);

    while (1) {
        XNextEvent(display, &event);

        if (event.type == Expose) {
            double start_time = omp_get_wtime();
#ifndef OMPDYNAMIC
            #pragma omp parallel for
#else
            #pragma omp parallel for schedule(dynamic,WIDTH/(num_threads*4))
#endif
            for (int x=0; x<WIDTH; x++) {
               for (int y=0; y<HEIGHT; y++) {
                    double complex c = (x - WIDTH/1.5) * 3.0/WIDTH + 
                                        (y - HEIGHT/2.0) * 3.0/HEIGHT * I;
                    long color = mandelbrot(c);
                    #pragma omp critical
                    {
                        if (color == MAX_ITER) {
                            XSetForeground(display, gc, BlackPixel(display, screen));
                        } else {
                            XSetForeground(display, gc, color * 0xFFFFFF / MAX_ITER);
                        }
                        XDrawPoint(display, window, gc, x, y);
                    }
               }
            }
            double end_time = omp_get_wtime();
            printf ("Time: %f sec.\n", end_time-start_time);
        }

        if (event.type == KeyPress)
            break;
    }

    XCloseDisplay(display);
    return 0;
}