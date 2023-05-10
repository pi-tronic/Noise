# Noise...

...of any kind

### <u>Run</u>

To build the code you need to execute following command in your directory:

> g++ -o build/{output filename} {input directory}/{input filename}.cpp -lSDL2 -lSDL2main

Then type the following to run it.

> ./build/{output filename}

---
## Implemented Noise

### <u>Worley Noise</u>

- create random points
- value of every pixel is calculated from it's distance to the closest point

You can optimize the algorithm by splitting the screen into sections and the place n (best is 1) point per section. \
Then your algorithm to calculate the distance only needs to check the distances with the points in the own section and the 8 surrounding sections. \
Otherwise you would need to check every single point...

![Worley Noise](screenshots/worley_noise_01.png | width=200)