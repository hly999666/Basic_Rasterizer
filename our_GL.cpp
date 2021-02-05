
#ifndef OUR_GL_H
#include "our_GL.hpp"
#endif
#ifndef GEOMETRY_H
#include "geometry.h"
#endif
 
#include <vector>

Vec3f barycentric(Vec2i *pts, Vec2i P) { 
    Vec3f v1(pts[2][0]-pts[0][0], pts[1][0]-pts[0][0], pts[0][0]-P[0]);
    Vec3f v2(pts[2][1]-pts[0][1], pts[1][1]-pts[0][1], pts[0][1]-P[1]);
    Vec3f u = _cross(v1,v2);
    /* `pts` and `P` has integer value as coordinates,
       so `abs(u[2])` < 1 means `u[2]` is 0, that means
       triangle is degenerate, so no pixel should be filled,return rejected case */
    if (std::abs(u[2])<1) return Vec3f(-1,1,1);
    return Vec3f(1.0f-(u.x+u.y)/u.z, u.y/u.z, u.x/u.z); 
} 
  void line(int x0, int y0, int x1, int y1, TGAImage &image, TGAColor color) { 
    bool steep = false; 
    if (std::abs(x0-x1)<std::abs(y0-y1)) { 
        std::swap(x0, y0); 
        std::swap(x1, y1); 
        steep = true; 
    } 
    if (x0>x1) { 
        std::swap(x0, x1); 
        std::swap(y0, y1); 
    } 
    int dx = x1-x0; 
    int dy = y1-y0; 
    int derror2 = std::abs(dy)*2; 
    int error2 = 0; 
    int y = y0; 
    for (int x=x0; x<=x1; x++) { 
        if (steep) { 
            image.set(y, x, color); 
        } else { 
            image.set(x, y, color); 
        } 
        error2 += derror2; 
        if (error2 > dx) { 
            y += (y1>y0?1:-1); 
            error2 -= dx*2; 
        } 
    } 
} 
inline void  line(Vec2i v0,Vec2i v1,TGAImage &image, TGAColor color){
    line(v0.x,v0.y,v1.x,v1.y,image,color);
}

void triangle(glm::vec4 *_pts4,IShader& shader,TGAImage &image,std::vector<double>& zbuffer,gl_enviroment& envir) { 
    int width=envir.width;
    int height=envir.height;
   Vec3f pts[3];
   for(int i=0;i<3;i++){
       pts[i][0]=_pts4[i][0]/_pts4[i][3];
       pts[i][1]=_pts4[i][1]/_pts4[i][3];
       pts[i][2]=_pts4[i][2]/_pts4[i][3];      
   }
   
    Vec2i bboxmin(image.get_width()-1,  image.get_height()-1); 
    Vec2i bboxmax(0, 0); 
    Vec2i clamp(image.get_width()-1, image.get_height()-1); 
    for (int i=0; i<3; i++) { 
        for (int j=0; j<2; j++) { 
            bboxmin[j] = std::max(0, std::min(bboxmin[j],(int) pts[i][j])); 
            bboxmax[j] = std::min(clamp[j], std::max(bboxmax[j],(int) pts[i][j])); 
        } 
    } 
    Vec2i _pts[3];
    for (int i=0; i<3; i++)_pts[i]=Vec2i(pts[i][0],pts[i][1]);
    Vec2i P; double z=0.0;
    for (P.x=bboxmin.x; P.x<=bboxmax.x; P.x++) { 
        for (P.y=bboxmin.y; P.y<=bboxmax.y; P.y++) { 
            Vec3f bc_screen  = barycentric(_pts, Vec2i(P.x,P.y)); 
            if (bc_screen.x<0 || bc_screen.y<0 || bc_screen.z<0) continue; 
              z = 0.0;
            //compute z
            vec2 uv;uv.x=0.0;uv.y=0.0;
            double intensity_interpolated=0;
            for (int i=0; i<3; i++){
                z+=(double)pts[i][2]*(double)bc_screen[i];
            /*     uv.x+=(double)_uv[i][0]*(double)bc_screen[i];
                uv.y+=(double)_uv[i][1]*(double)bc_screen[i];
                intensity_interpolated+=(double)v_intensity[i]*(double)bc_screen[i]; */
            }
            intensity_interpolated=std::max(0.0,(double)intensity_interpolated);
              if (P.x>=width||P.y>=height||P.x<0||P.y<0) continue;
              if (zbuffer[int(P.x+P.y*width)]<z) {
               
                 
        /*         auto tex_color=color_map->get(uv.x,uv.y);
                double factor=1.0;
                factor=((double)color.bgra[0]/255.0);
                auto filled_color=tex_color*factor; */
                 TGAColor color;
               bool discard = shader.fragment(bc_screen, color);
               if (!discard) {
                   zbuffer[int(P.x+P.y*width)]=z;
                  image.set(P.x, P.y, color);
               }
                //image.set(P.x, P.y,TGAColor(255*intensity_interpolated,255*intensity_interpolated,255*intensity_interpolated,255));
            } 
              //image.set(P.x, P.y, color);
        } 
    } 
} 