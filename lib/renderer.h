#pragma once
#include <iostream>
#include <cstdint>
#include <vector>
#include <fstream>
#include <variant>
#include <climits>
#include <algorithm>


struct pixel{
    uint8_t r,g,b,a;
};

struct point{
    int x,y;
};

struct Circle {point a;int r;pixel color;};
struct Rectangle{point a;point b;pixel color;};
struct Triangle{point a;point b;point c;pixel color;};

using Shapes  = std::variant<Circle,Rectangle,Triangle>;

int sign(int v);
pixel blend(pixel dest, pixel src);
void setpixel(std::vector<pixel>& fb, int w, int h, int x, int y, pixel color);
void draw_line(std::vector<pixel>& fb, int w, int h, point a, point b, pixel color);
void fill_rect(std::vector<pixel>& fb, int w, int h, point a, point b, pixel color);
void fill_triangle(std::vector<pixel>& fb, int w, int h, point a, point b, point c, pixel color);
void fill_circle_aa(std::vector<pixel>& fb, int w, int h, point center, int r, pixel color);
void save_ppm(const std::vector<pixel>& fb, int w, int h, const char* path);
void render(std::vector<pixel>& fb, int w, int h, const std::vector<Shapes>& scene);