// used libraries
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <chrono>
#include <cmath>
#include <iostream>
#include <SDL2/SDL.h>

// define constants
#define AMOUNT 1            // AMOUNT of points per SECTION
#define SECTIONS 25         // SECTIONS x SECTIONS field
#define WIDTH 100          // width of surface
#define HEIGHT 100         // height of surface

// template
template <typename T> int sign(T val) {
    return (T(0) < val) - (val < T(0));
}

// declare functions
double get_dist2(int& x1, int& y1, int& x2, int& y2);
double get_dist(double x1, double y1, double x2, double y2);
int* create_worley_points();
int noise_generator(int* nodeArray, int x, int y);
void draw(SDL_Surface* surface);
void WipeSurface(SDL_Surface *surface);
void draw_line(SDL_Surface* surface, uint8_t* pixelArray, int x1, int y1, int x2, int y2, uint8_t r, uint8_t g, uint8_t b);
void draw_point(SDL_Surface* surface, uint8_t* pixelArray, int p_x, int p_y, int p_r, uint8_t r, uint8_t g, uint8_t b);

// allocate space for array
// this array will be used to create the next frame and then be copied to the screen 
// uint8_t* pixelArray[WIDTH * HEIGHT * 4] {0};

// main function
int main(int argc, char const *argv[])
{
    // variables and constants
	// int width = 1000;
	// int height = 1000;

	// initialize SDL as video only
	SDL_Init(SDL_INIT_VIDEO);
    SDL_Window* window = SDL_CreateWindow("Worley Noise", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WIDTH, HEIGHT, SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL);
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
                // WipeSurface(screen);

                // draw the image
                draw(screen);

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

// test efficiency improvement of not actually giving values, but rather adresses of values
double get_dist2(int& x1, int& y1, int& x2, int& y2) {
    return std::sqrt(std::pow(std::abs(x2 - x1),2)+std::pow(std::abs(y2 - y1),2));
}

// pythagorean theorem to calculate the distance between two 2d points
double get_dist(double x1, double y1, double x2, double y2) {
    return std::sqrt(std::pow(std::abs(x2 - x1),2)+std::pow(std::abs(y2 - y1),2));
}

// create points within the sections to build the noise around
int* create_worley_points() {
    // creates an array to store all the 2d point coordinates
    int* nodeArray = new int[SECTIONS*SECTIONS * AMOUNT*2]{ 0 };

    // creates x and y coordinates of points
    for (int x = 0; x < SECTIONS; x++) {
        for (int y = 0; y < SECTIONS; y++) {
            for (int i = 0; i < AMOUNT; i++) {
                nodeArray[x*SECTIONS*AMOUNT*2+y*AMOUNT*2+i*2] = rand() % (WIDTH/SECTIONS) + x*(WIDTH/SECTIONS);
                nodeArray[x*SECTIONS*AMOUNT*2+y*AMOUNT*2+i*2+1] = rand() % (HEIGHT/SECTIONS) + y*(HEIGHT/SECTIONS);
            }
        }
    }

    return nodeArray;
}

int noise_generator(int* nodeArray, int x, int y) {
    // get the section the pixel is in
    int x_a = x / (WIDTH/SECTIONS);
    int y_a = y / (HEIGHT/SECTIONS);

    // set it absurdly high, so it will always be overwritten
    int min_dist = WIDTH + HEIGHT;

    bool show = true;

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

    double result = min_dist / get_dist(0, 0, (WIDTH*1.0)/(SECTIONS*AMOUNT), (HEIGHT*1.0)/(SECTIONS*AMOUNT));

    if (result > 1.0) {
        result = 1.0;
    }

	return (1.0- (result)) * 255;
}



void draw(SDL_Surface* surface) {
    srand((unsigned) time(NULL));
    SDL_LockSurface(surface);
    uint8_t* pixelArray = (uint8_t*)surface->pixels;
    int value;

    // generate worley nodes
    int* nodeArray = create_worley_points();

    std::cout << surface->h * surface->pitch << std::endl;

    // draw noise
	for (int x = 0; x < WIDTH; x++) {
		for (int y = 0; y < HEIGHT; y++) {
            value = noise_generator(nodeArray, x, y);
            pixelArray[y*surface->pitch + x*surface->format->BytesPerPixel+0] = value;
            pixelArray[y*surface->pitch + x*surface->format->BytesPerPixel+1] = value;
            pixelArray[y*surface->pitch + x*surface->format->BytesPerPixel+2] = value;
		}
	}

    // // draw section lines
    // for (int i = 1; i < SECTIONS; i++) {
    //     draw_line(surface, pixelArray, 0, i*(HEIGHT/SECTIONS), WIDTH, i*(HEIGHT/SECTIONS), 0, 255, 0);
    // }
    // for (int i = 1; i < SECTIONS; i++) {
    //     draw_line(surface, pixelArray, i*(WIDTH/SECTIONS), 0, i*(WIDTH/SECTIONS), HEIGHT, 0, 255, 0);
    // }

    // // draw worley points
    // for (int i = 0; i < (SECTIONS*SECTIONS * AMOUNT); i++) {
    //     draw_point(surface, pixelArray, nodeArray[i*2], nodeArray[i*2+1], 5, 255, 255, 0);

    //     //std::cout << i << " --> " << nodeArray[i] << ":" << nodeArray[i+1] << std::endl;
    // }

    SDL_UnlockSurface(surface);
}

// copies black on all pixels
void WipeSurface(SDL_Surface *surface)
{
    SDL_LockSurface(surface);

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
            x = a * y + x1;

            pixelArray[y*surface->pitch + x*surface->format->BytesPerPixel+0] = b;
            pixelArray[y*surface->pitch + x*surface->format->BytesPerPixel+1] = g;
            pixelArray[y*surface->pitch + x*surface->format->BytesPerPixel+2] = r;
        }
    } else {
        // determine increment of the line function
        a = (y2-y1) / (double)(x2-x1);

        for (int i = 0; i != range; i += sign(range)) {
            x = x1 + i;
            y = a * x + y1;

            pixelArray[y*surface->pitch + x*surface->format->BytesPerPixel+0] = b;
            pixelArray[y*surface->pitch + x*surface->format->BytesPerPixel+1] = g;
            pixelArray[y*surface->pitch + x*surface->format->BytesPerPixel+2] = r;
        }
    }
}

// custom function to draw bigger points on surfaces
void draw_point(SDL_Surface* surface, uint8_t* pixelArray, int p_x, int p_y, int p_r, uint8_t r, uint8_t g, uint8_t b) {
    for (int x = 0; x < p_r*2-1; x++) {
        if (p_x-(p_r-1)+x >= 0 && p_x-(p_r-1)+x < WIDTH) {
            for (int y = 0; y < p_r*2-1; y++) {
                if (p_y-(p_r-1)+y >= 0 && p_y-(p_r-1)+y < HEIGHT) {
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