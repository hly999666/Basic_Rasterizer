#ifndef __IMAGE_H__
#define __IMAGE_H__

#include <cstdint>
#include <fstream>
#include <vector>
#include <glm/glm.hpp>
#pragma pack(push,1)
struct TGA_Header {
    std::uint8_t  idlength{};
    std::uint8_t  colormaptype{};
    std::uint8_t  datatypecode{};
    std::uint16_t colormaporigin{};
    std::uint16_t colormaplength{};
    std::uint8_t  colormapdepth{};
    std::uint16_t x_origin{};
    std::uint16_t y_origin{};
    std::uint16_t width{};
    std::uint16_t height{};
    std::uint8_t  bitsperpixel{};
    std::uint8_t  imagedescriptor{};
};
#pragma pack(pop)

struct TGAColor {
    std::uint8_t bgra[4] = {0,0,0,0};
    std::uint8_t bytespp = {0};
     
    TGAColor() = default;
    TGAColor(const std::uint8_t R, const std::uint8_t G, const std::uint8_t B, const std::uint8_t A=255) : bgra{B,G,R,A}, bytespp(4) { }
    TGAColor(const std::uint8_t v) : bgra{v,0,0,0}, bytespp(1) { }

    TGAColor(const std::uint8_t *p, const std::uint8_t bpp) : bgra{0,0,0,0}, bytespp(bpp) {
        for (int i=0; i<bpp; i++)
            bgra[i] = p[i];
    }

    std::uint8_t& operator[](const int i) { return bgra[i]; }

    TGAColor operator *(const double intensity) const {
        TGAColor res = *this;
        double clamped = std::max(0., std::min(intensity, 1.));
        for (int i=0; i<4; i++) res.bgra[i] = bgra[i]*clamped;
        return res;
    }
    TGAColor operator *(const TGAColor& _b) const {
          glm::vec4 a(bgra[0],bgra[1],bgra[2],bgra[3]);
          glm::vec4 b(_b.bgra[0],_b.bgra[1],_b.bgra[2],_b.bgra[3]);
           a/=255.0;b/=255.0;
           auto color=a*b;color*=255.0;
           return TGAColor(color[0],color[1],color[2],color[3]);
    }
};

class TGAImage {
protected:
   

    bool   load_rle_data(std::ifstream &in);
    bool unload_rle_data(std::ofstream &out) const;
public:
 std::vector<std::uint8_t> data;
    int width;
    int height;
    int bytespp;
    enum Format { GRAYSCALE=1, RGB=3, RGBA=4 };

    TGAImage();
    TGAImage(const int w, const int h, const int bpp);
    bool  read_tga_file(const std::string filename);
    bool write_tga_file(const std::string filename, const bool vflip=true, const bool rle=true) const;
    void flip_horizontally();
    void flip_vertically();
    void scale(const int w, const int h);
    TGAColor get(const int x, const int y) const;
    TGAColor get(const float x, const float y)const{
        int _x=(int)width*x;
        int _y=(int)height*y;
        auto color=  get(_x,_y);
        return color;
    }
    void set(const int x, const int y, const TGAColor &c);
    int get_width() const;
    int get_height() const;
    int get_bytespp();
    std::uint8_t *buffer();
    void clear();
};

#endif //__IMAGE_H__

