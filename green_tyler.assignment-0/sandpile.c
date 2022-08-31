#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

const unsigned int width = 23;
const unsigned int height = 23;
unsigned int fps = 30;

void checkTopple(int p[height][width], unsigned int y, unsigned int x) {

    if (p[y][x] <= 8) { //stable
        return;
    }
    
    // toppled
    int i, j;

    p[y][x] -= 8;

    for (i = -1; i < 2; i++) {
        for (j = -1; j < 2; j++) {
            int adjY = y + i;
            int adjX = x + j;

            if (i == 0 && j == 0) {
                continue;
            }

            if (adjX < width && adjX >= 0 && adjY < height && adjY >= 0) {
                if (p[adjY][adjX] != -1) {
                    p[adjY][adjX]++;
                }
            }
        }
    }

    for (i = -1; i < 2; i++) {
        for (j = -1; j < 2; j++) {
            int adjY = y + i;
            int adjX = x + j;

            if (i == 0 && j == 0) {
                continue;
            }

            if (adjX < width && adjX >= 0 && adjY < height && adjY >= 0) {
                if (p[adjY][adjX] != -1) {
                    checkTopple(p, adjY, adjX);
                }
            }
        }
    }

    return;
}

void printPlane(int p[height][width]) {
    unsigned int i, j;

    for (i = 0; i < height; i++) {
        for (j = 0; j < width; j++) {
            if (p[i][j] == -1) {
                printf("# ");
            } else {
                printf("%d ", p[i][j]);
            }
        }
        printf("\n");
    }
    printf("\n");
}

void nextIteration(int p[height][width]) {
    unsigned int centerX = width/2;
    unsigned int centerY = height/2;

    p[centerY][centerX]++; // drop one more grain onto the center
    checkTopple(p, centerY, centerX);
}

int main(int argc, char* argv[]) {
    unsigned int saltStart = 1;
    unsigned int argi;

    int plane[height][width];
    memset(plane, 0, sizeof(int) * height * width);

    if (argc > 1 && !strcmp(argv[1], "--fps")) {  // if fps provided
        fps = atoi(argv[2]);
        saltStart = 3;
    }
    printf("Window running with width=%d, height=%d, fps=%d.\n", width, height, fps);

    printf("Initializing board.\n");

    for (argi = saltStart; argi < argc; argi += 3) {
        unsigned int y = atoi(argv[argi]);
        unsigned int x = atoi(argv[argi + 1]);
        int h = atoi(argv[argi + 2]);

        if (y == height/2 && x == width/2 && h == -1) {
            printf("The center may not be a sink.\n");
            return 1;
        }

        plane[y][x] = h;
    }

    printf("Board initialized.\n");

    printf("Starting simulation.\n");

    while (1) {
        printPlane(plane);
        nextIteration(plane);

        usleep(1000000 / fps);
    }

    return 0;
}
