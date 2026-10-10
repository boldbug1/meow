#include "../lib/renderer.h"

void setpixel(std::vector<pixel>& fb, int w, int h, int x, int y, pixel color){
    if ((x < 0 || x > w-1) || (y < 0 || y > h-1)){
        return;
    }
    fb[y * w + x] = blend(fb[y * w + x], color);
}

void fill_rect(std::vector<pixel>& fb, int w, int h, point a, point b, pixel color){
    for(int y = a.y; y <= b.y; y++){
        for(int x = a.x; x <= b.x; x++){
            setpixel(fb, w, h, x, y, color);
        }
    }
}

int sign(int v) {
    if (v > 0) return 1;
    if (v < 0) return -1;
    return 0;
}

void draw_line(std::vector<pixel>& fb, int w, int h, point a, point b, pixel color) {
    int dx = std::abs(b.x - a.x);
    int dy = std::abs(b.y - a.y);

    int sx = (a.x < b.x) ? 1 : -1;
    int sy = (a.y < b.y) ? 1 : -1;

    int err = dx - dy;

    int x = a.x;
    int y = a.y;

    while (true) {
        setpixel(fb, w, h, x, y, color);

        if (x == b.x && y == b.y) {
            break;
        }

        int e2 = 2 * err;

        if (e2 > -dy) {
            err -= dy;
            x += sx;
        }

        if (e2 < dx) {
            err += dx;
            y += sy;
        }
    }
}

int edge_x(point a, point b, int y) {
    if (b.y == a.y) return a.x;   // horizontal edge: avoid divide by zero
    return a.x + (b.x - a.x) * (y - a.y) / (b.y - a.y);
}

void fill_triangle(std::vector<pixel>& fb, int w, int h, point a, point b, point c, pixel color) {
    point p[] = {a, b, c};
    std::sort(p, p + 3, [](point m, point n){ return m.y < n.y; });

    for (int y = p[0].y; y <= p[2].y; y++) {
        int x_long = edge_x(p[0], p[2], y);

        int x_short;
        if (y < p[1].y) {
            x_short = edge_x(p[0], p[1], y);
        } else {
            x_short = edge_x(p[1], p[2], y);
        }

        int x_start = std::min(x_long, x_short);
        int x_end   = std::max(x_long, x_short);

        for (int x = x_start; x <= x_end; x++) {
            setpixel(fb, w, h, x, y, color);
        }
    }
}

void save_ppm(const std::vector<pixel>& fb, int w, int h, const char* path){
    std::ofstream out(path, std::ios::binary);

    if(!out){
        std::cerr << "Error while opening file: save_ppm failed\n";
        return;
    }

    out << "P6\n" << w << " " << h << "\n255\n";

    for (const pixel& p : fb) {
        out.write(reinterpret_cast<const char*>(&p.r), 1);
        out.write(reinterpret_cast<const char*>(&p.g), 1);
        out.write(reinterpret_cast<const char*>(&p.b), 1);
    }
}

pixel blend(pixel dst, pixel src){
    pixel newcolor;
    newcolor.r = static_cast<uint8_t>((src.r * src.a + dst.r * (255 - src.a)) / 255);
    newcolor.g = static_cast<uint8_t>((src.g * src.a + dst.g * (255 - src.a)) / 255);
    newcolor.b = static_cast<uint8_t>((src.b * src.a + dst.b * (255 - src.a)) / 255);
    newcolor.a = 255;
    return newcolor;
}

void fill_circle_aa(std::vector<pixel>& fb, int w, int h, point center, int r, pixel color){
    for (int y = center.y - r - 1; y <= center.y + r + 1; y++) {
        for (int x = center.x - r - 1; x <= center.x + r + 1; x++) {
            int hits = 0;
            for(int i = 0; i < 4; i++){
                for(int j = 0; j < 4; j++){
                    float px = x + (i + 0.5f) / 4;
                    float py = y + (j + 0.5f) / 4;
                    float dx = px - center.x;
                    float dy = py - center.y;
                    if(dx*dx + dy*dy <= r*r) hits++;
                }
            }
            if (hits > 0){
                pixel c = color;
                c.a = static_cast<uint8_t>(color.a * hits / 16);
                setpixel(fb, w, h, x, y, c);
            }
        }
    }
}

void render(std::vector<pixel>& fb, int w, int h, const std::vector<Shapes>& scene){
    for(const Shapes& s : scene){
        switch (s.index()) {
            case 0: {
                const Circle& c = std::get<Circle>(s);
                fill_circle_aa(fb, w, h, c.a, c.r, c.color);
                break;
            }
            case 1: {
                const Rectangle& r = std::get<Rectangle>(s);
                fill_rect(fb, w, h, r.a, r.b, r.color);
                break;
            }
            case 2: {
                const Triangle& t = std::get<Triangle>(s);
                fill_triangle(fb, w, h, t.a, t.b, t.c, t.color);
                break;
            }
        }
    }
}