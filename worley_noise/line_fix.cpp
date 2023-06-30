// used libraries
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <chrono>
#include <cmath>
#include <iostream>
#include <SDL2/SDL.h>

// define constants
#define AMOUNT 1           // AMOUNT of points per SECTION
#define SECTIONS 10         // SECTIONS x SECTIONS field

// template
template <typename T> int sign(T val) {
    return (T(0) < val) - (val < T(0));
}

// declare functions
int* create_worley_points(int width, int height);
int noise_generator(int* nodeArray, int x, int y, int width, int height);
void draw(SDL_Surface* surface, int width, int height);
void WipeSurface(SDL_Surface *surface);
void draw_line(SDL_Surface* surface, uint8_t* pixelArray, int x1, int y1, int x2, int y2, uint8_t r, uint8_t g, uint8_t b);
void draw_point(SDL_Surface* surface, uint8_t* pixelArray, int p_x, int p_y, int p_r, int width, int height, uint8_t r, uint8_t g, uint8_t b);

// main function
int main(int argc, char const *argv[])
{
	// variables and constants
	int width = 1000;
	int height = 1000;

	// initialize SDL as video only
	SDL_Init(SDL_INIT_VIDEO);
    SDL_Window* window = SDL_CreateWindow("Worley Noise", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height, SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL);
    SDL_Surface* screen = SDL_GetWindowSurface(window);

    // set window position on screen
    SDL_SetWindowPosition(window, 0, 0);

    // runtime variables
	bool running = true;
    bool generating = true;
	SDL_Event e;

	while (running) {
		// check for inputs
		SDL_PollEvent(&e);

		// close the window
		if (e.type == SDL_QUIT) {
			running = false;
		}
        if (e.type == SDL_MOUSEBUTTONDOWN) {
            if (e.button.button == SDL_BUTTON_LEFT) {
                // confirm init
                std::cout << "Rendering started..." << std::endl;
                auto start = std::chrono::high_resolution_clock::now();

                // delete everything on screen
                WipeSurface(screen);

                // draw the image
                draw(screen, width, height);

                // calculate execution time
                auto stop = std::chrono::high_resolution_clock::now();
                auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);

                // confirm finish
                std::cout << "Rendering complete!" << std::endl;

                // display execution time
                std::cout << "Execution time: " << duration.count() << " ms" << std::endl;
            }
        }

        // update the shown surface
        SDL_UpdateWindowSurface(window);
	}

    // soft exit -> end all processes
	SDL_DestroyWindow(window);
	SDL_Quit();

	return EXIT_SUCCESS;
}

// pythagorean theorem to calculate the distance between two 2d points
double get_dist(double x1, double y1, double x2, double y2) {
    return std::sqrt(std::pow(std::abs(x2 - x1),2)+std::pow(std::abs(y2 - y1),2));
}

// create points within the sections to build the noise around
int* create_worley_points(int width, int height) {
    // creates an array to store all the 2d point coordinates
    int* nodeArray = new int[SECTIONS*SECTIONS * AMOUNT*2]{ 0 };

    // puts AMOUNT points in each section
    // creates x and y coordinates of points
    for (int x = 0; x < SECTIONS; x++) {
        for (int y = 0; y < SECTIONS; y++) {
            for (int i = 0; i < AMOUNT; i++) {
                nodeArray[x*SECTIONS*AMOUNT*2+y*AMOUNT*2+i*2] = rand() % (width/SECTIONS) + x*(width/SECTIONS);
                nodeArray[x*SECTIONS*AMOUNT*2+y*AMOUNT*2+i*2+1] = rand() % (height/SECTIONS) + y*(height/SECTIONS);
            }
        }
    }

    return nodeArray;
}

int noise_generator(int* nodeArray, int x, int y, int width, int height) {
    // get the section the pixel is in
    int x_a = x / (width/SECTIONS);
    int y_a = y / (height/SECTIONS);

    // set it absurdly high, so it will always be overwritten
    int min_dist = width + height;

    // check the 8 neighbouring and own sections nodes for min distance
    // leaves out sections that are not existing (world border)
    for (int a = 0; a < 3; a++) {
        if (x_a-1+a >= 0 && x_a-1+a < SECTIONS) {
            for (int b = 0; b < 3; b++) {
                if (y_a-1+b >= 0 && y_a-1+b < SECTIONS) {
                    for (int i = 0; i < AMOUNT; i++) {
                        if (min_dist > get_dist(x, y, nodeArray[SECTIONS*(x_a-1+a)*AMOUNT*2+(y_a-1+b)*AMOUNT*2+i*2], nodeArray[SECTIONS*(x_a-1+a)*AMOUNT*2+(y_a-1+b)*AMOUNT*2+i*2+1])) {
                            min_dist = get_dist(x, y, nodeArray[SECTIONS*(x_a-1+a)*AMOUNT*2+(y_a-1+b)*AMOUNT*2+i*2], nodeArray[SECTIONS*(x_a-1+a)*AMOUNT*2+(y_a-1+b)*AMOUNT*2+i*2+1]);
                        }
                    }
                }
            }
        }
    }

    //std::sqrt(std::pow(std::abs(nodeArray[i*2] - x),2)+std:pow(std::abs(nodeArray[i*2+1] - y),2))

    double result = min_dist / get_dist(0, 0, (width*1.0)/(SECTIONS*AMOUNT), (height*1.0)/(SECTIONS*AMOUNT));

    if (result > 1.0) {
        result = 1.0;
    }

	return (1.0- (result)) * 255;
}

void draw(SDL_Surface* surface, int width, int height) {
    // srand((unsigned) time(NULL));
    SDL_LockSurface(surface);
    uint8_t* pixelArray = (uint8_t*)surface->pixels;
    // int value;

    // generate worley nodes
    // int* nodeArray = create_worley_points(width, height);

    // draw noise
	// for (int x = 0; x < width; x++) {
	// 	for (int y = 0; y < height; y++) {
    //         value = noise_generator(nodeArray, x, y, width, height);
    //         pixelArray[y*surface->pitch + x*surface->format->BytesPerPixel+0] = value;
    //         pixelArray[y*surface->pitch + x*surface->format->BytesPerPixel+1] = value;
    //         pixelArray[y*surface->pitch + x*surface->format->BytesPerPixel+2] = value;
	// 	}
	// }

    // draw section lines
    for (int i = 1; i < SECTIONS; i++) {
        draw_line(surface, pixelArray, 0, i*(height/SECTIONS), width, i*(height/SECTIONS), 0, 255, 0);
    }
    for (int i = 1; i < SECTIONS; i++) {
        draw_line(surface, pixelArray, i*(width/SECTIONS), 0, i*(width/SECTIONS), height, 0, 255, 0);
    }

    // draw red line(s)
    draw_line(surface, pixelArray, 300, 300, 900, 400, 255, 0, 0);
    draw_line(surface, pixelArray, 300, 300, 400, 900, 255, 0, 0);
    draw_line(surface, pixelArray, 0, 0, 1000, 1000, 255, 0, 0);
    draw_line(surface, pixelArray, 500, 100, 400, 10, 255, 0, 0);

    // draw worley points
    // for (int i = 0; i < (SECTIONS*SECTIONS * AMOUNT); i++) {
    //     draw_point(surface, pixelArray, nodeArray[i*2], nodeArray[i*2+1], 5, width, height, 255, 255, 0);

    //     //std::cout << i << " --> " << nodeArray[i] << ":" << nodeArray[i+1] << std::endl;
    // }


    SDL_UnlockSurface(surface);
}

// copies black on all pixels
void WipeSurface(SDL_Surface *surface)
{
    /* This is fast for surfaces that don't require locking. */
    /* Once locked, surface->pixels is safe to access. */
    SDL_LockSurface(surface);

    /* This assumes that color value zero is black. Use
       SDL_MapRGBA() for more robust surface color mapping! */
    /* height times pitch is the size of the surface's whole buffer. */
    SDL_memset(surface->pixels, 0, surface->h * surface->pitch);

    SDL_UnlockSurface(surface);
}

// custom function to draw lines on surfaces
void draw_line(SDL_Surface* surface, uint8_t* pixelArray, int x1, int y1, int x2, int y2, uint8_t r, uint8_t g, uint8_t b) {
    int x, y;
    double a;
    int range = x2 - x1;
    
    if (range < y2 - y1) { 
        range = y2 - y1;

        // determine increment of the line function
        a = (x2-x1) / (double)(y2-y1);

        for (int i = 0; i != range; i += sign(range)) {
            y = y1 + i;
            x = a * i + x1;

            pixelArray[y*surface->pitch + x*surface->format->BytesPerPixel+0] = b;
            pixelArray[y*surface->pitch + x*surface->format->BytesPerPixel+1] = g;
            pixelArray[y*surface->pitch + x*surface->format->BytesPerPixel+2] = r;
        }
    } else {
        // determine increment of the line function
        a = (y2-y1) / (double)(x2-x1);

        for (int i = 0; i != range; i += sign(range)) {
            x = x1 + i;
            y = a * i + y1;

            pixelArray[y*surface->pitch + x*surface->format->BytesPerPixel+0] = b;
            pixelArray[y*surface->pitch + x*surface->format->BytesPerPixel+1] = g;
            pixelArray[y*surface->pitch + x*surface->format->BytesPerPixel+2] = r;
        }
    }

    //std::cout << "line drawn" << std::endl;
}

// custom function to draw bigger points on surfaces
void draw_point(SDL_Surface* surface, uint8_t* pixelArray, int p_x, int p_y, int p_r, int width, int height, uint8_t r, uint8_t g, uint8_t b) {
    for (int x = 0; x < p_r*2-1; x++) {
        if (p_x-(p_r-1)+x >= 0 && p_x-(p_r-1)+x < width) {
            for (int y = 0; y < p_r*2-1; y++) {
                if (p_y-(p_r-1)+y >= 0 && p_y-(p_r-1)+y < height) {
                    if (get_dist(p_x-(p_r-1)+x, p_y-(p_r-1)+y, p_x, p_y) < p_r-1) {
                        pixelArray[(p_y-(p_r-1)+y)*surface->pitch + (p_x-(p_r-1)+x)*surface->format->BytesPerPixel+0] = b;
                        pixelArray[(p_y-(p_r-1)+y)*surface->pitch + (p_x-(p_r-1)+x)*surface->format->BytesPerPixel+1] = g;
                        pixelArray[(p_y-(p_r-1)+y)*surface->pitch + (p_x-(p_r-1)+x)*surface->format->BytesPerPixel+2] = r;
                    }
                }
            }
        }
    }
}