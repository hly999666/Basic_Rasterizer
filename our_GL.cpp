
#ifndef OUR_GL_H
#include "our_GL.hpp"
#endif
#ifndef GEOMETRY_H
#include "geometry.h"
#endif
 #include <cmath>
#include <limits>
#include <cstdlib>
#include <vector>
float nu=1e-2;
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
Vec3f barycentric(Vec2f A, Vec2f B, Vec2f C, Vec2f P) {
    Vec3f s[2];
    for (int i=2; i--; ) {
        s[i][0] = C[i]-A[i];
        s[i][1] = B[i]-A[i];
        s[i][2] = A[i]-P[i];
    }
    Vec3f u = _cross(s[0], s[1]);
    if (std::abs(u[2])>1e-2)  
        return Vec3f(1.f-(u.x+u.y)/u.z, u.y/u.z, u.x/u.z);
    return Vec3f(-1,1,1); 
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

void triangle(glm::vec4 *_pts4,IShader& shader,TGAImage &image,std::vector<double>& zbuffer, const gl_enviroment& envir) { 
    int width=envir.width;
    int height=envir.height;
   Vec3f pts[3];
   for(int i=0;i<3;i++){
       
       pts[i][0]=_pts4[i][0]/_pts4[i][3];
       pts[i][1]=_pts4[i][1]/_pts4[i][3];
       pts[i][2]=_pts4[i][2]/_pts4[i][3]; 
   }
   
    Vec2f bboxmin( std::numeric_limits<float>::max(),  std::numeric_limits<float>::max());
    Vec2f bboxmax(-std::numeric_limits<float>::max(), -std::numeric_limits<float>::max());
    Vec2f clamp(image.get_width()-1, image.get_height()-1);
    for (int i=0; i<3; i++) { 
        for (int j=0; j<2; j++) { 
            bboxmin[j] = std::max(0.f, std::min(bboxmin[j],  pts[i][j])); 
            bboxmax[j] = std::min(clamp[j], std::max(bboxmax[j],pts[i][j])); 
        } 
    } 
    Vec2i _ptsI[3];
    Vec2f _ptsf[3];
    for (int i=0; i<3; i++){
        _ptsf[i]=Vec2f(pts[i][0],pts[i][1]);
        _ptsf[i]=Vec2f(pts[i][0],pts[i][1]);
    }
    Vec2i P; double z=0.0;
    for (P.x=bboxmin.x; P.x<=bboxmax.x; P.x++) { 
        for (P.y=bboxmin.y; P.y<=bboxmax.y; P.y++) { 
            if (P.x>=width||P.y>=height||P.x<0||P.y<0) continue;
            Vec3f bc_screen  = barycentric(_ptsf[0], _ptsf[1],_ptsf[2],Vec2f(P.x,P.y)); 
         
            if (bc_screen.x<0.0|| bc_screen.y<0.0|| bc_screen.z<0.0) continue; 
        //perspective correct linear interpolation  
           auto bc_clip=bc_screen;
            bc_clip.x/=_pts4[0][3];
            bc_clip.y/=_pts4[1][3];
            bc_clip.z/=_pts4[2][3];
            double sum=  bc_clip.x+  bc_clip.y+  bc_clip.z;
            double factor_p=1.0/sum;
            bc_clip=bc_clip*factor_p;
            
           
             
            z = 0.0;
            for (int i=0; i<3; i++)z+=(double)pts[i][2]*(double)bc_clip[i];
              if (zbuffer[int(P.x+P.y*width)]<z) {
                TGAColor color;
                shader.gl_FragCoord=Vec3f((float)P.x,(float)P.y,(float)z);
               bool discard = shader.fragment(bc_clip, color);
               if (!discard) {
                   zbuffer[int(P.x+P.y*width)]=z;
                  image.set(P.x, P.y, color);
               }
        } 
    } 
} 
}