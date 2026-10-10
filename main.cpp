#include <vector>
#include <fstream>
#include "lib/renderer.h"

int main(void) {
    int w = 1024;
    int h = 1024;

    std::vector<pixel> framebuffer(w * h);
    pixel black = {0,0,0,255};
    pixel white = {255,255,255,255};

    fill_rect(framebuffer,w,h,{0,0},{1024,1024},white);

    fill_triangle(framebuffer, w, h, {555,0}, {255,444}, {888,888}, black);

    save_ppm(framebuffer, w, h, "images/triangle_filled.ppm");
    return 0;
}