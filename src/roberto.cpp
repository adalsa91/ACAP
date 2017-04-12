#include "CImg.h"
#include <iostream>
#include <chrono>
#include <cmath>

using namespace cimg_library;
using namespace std::chrono;

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cout << "You need to supply a image to this program." << std::endl;
        return -1;
    }


    CImg<double> image("image.png");
    
    CImg<> gradient_x = CImg<>(3,3).fill(
        0, 0, 0,
        0, 1, 0,
        0, 0, -1
    );
    CImg<> gradient_y = CImg<>(3,3).fill(
        0, 0, 0,
        0, 0, 1,
        0, -1, 0
    );

    high_resolution_clock::time_point start_time = high_resolution_clock::now();
    
    CImg<double> result = (image.get_convolve(gradient_x).abs() + image.get_convolve(gradient_y).abs()).normalize(0,255);

    high_resolution_clock::time_point end_time = high_resolution_clock::now();

    duration<double> time_span = duration_cast<duration<double>>(end_time - start_time);

    std::cout << "It took me " << time_span.count() << " seconds." << std::endl;
    
    /*CImgDisplay main_disp(result,"Roberto");
    while (!main_disp.is_closed()) {
        main_disp.wait();
    }*/

    result.save("result.png");
}
