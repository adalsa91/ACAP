#include "CImg.h"
#include <iostream>
#include <chrono>
#include <vector>
#include <cmath>

using namespace cimg_library;
using namespace std::chrono;

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cout << "You need to supply a image to this program." << std::endl;
        return -1;
    }


    CImg<double> image(argv[1]);
    int width = image.width();
    int height = image.height();
    int spectrum = image.spectrum();

    double **image_data = new double*[height*spectrum];
    for (int i = 0; i < height*spectrum; i++)
        image_data[i] = &( image.data()[i*width] );

    double kernel_x[3][3] = {
        0, 0, 0,
        0, 1, 0,
        0, 0, -1
    };

    double kernel_y[3][3] = {
        0, 0, 0,
        0, 0, 1,
        0, -1, 0
    };

    double **result = new double*[(height-2)*spectrum];
    for (int i = 0; i < (height-2)*spectrum; i++){
        result[i] = new double[(width-2)];
        std::cout << i << " - " <<std::endl;
}

    for (int s = 0; s < spectrum; s++) {
        for (int i = 1; i < height-1; i++) {
            for (int j = 1; j < width-1; j++) {
                double  g_x = 0, g_y = 0;

                for (int k = -1; k <= 1; k++) {
                    for (int l = -1; l <= 1; l++){
                        g_x += image_data[i+s*height+k][j+l] * kernel_x[k+1][l+1];
                        g_y += image_data[i+s*height+k][j+l] * kernel_y[k+1][l+1];
                    }
                }

                double r = abs(g_x) + abs(g_y);
                result[(i-1) + (s*height)][j-1] = (r < 255) ? r : 255;
            }
        }
    }

    CImg <double> image_result(&result[0][0], width, height, 1, spectrum, false);

    image_result.save("resultasd.png");

    delete [] image_data;
    for (int i = 0; i < (height-2)*spectrum; i++)
        delete [] result[i];
    delete [] result;

    return(0);
}
