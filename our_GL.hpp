#include <iostream>
#ifndef TGAIMAGE_H
#include "tgaimage.h"
#endif
#ifndef GEOMETRY_H
#include "geometry.h"
#endif
#ifndef GLM_FORCE_SWIZZLE
#define GLM_FORCE_SWIZZLE
#endif
#include <glm/glm.hpp>
#include <glm/mat4x4.hpp> 
#include <glm/gtc/matrix_transform.hpp>
#ifndef OUR_GL_H
#define OUR_GL_H

 
class gl_enviroment{
    public:
      glm::mat4 Model;
      glm::mat4 View;
      glm::mat4 Viewport;
      glm::mat4 Projection;
      int width;
      int height;
      int depth;
public:
     gl_enviroment(int w,int h,int d){
      width=w;height=h;depth=d;
  }
   void setViewport(int x, int y, int w, int h,int d){
     Viewport=viewport (x,y,w,h,d);
   };
   void setProjection(float coeff){
     Projection=projection(coeff);
   };
   void setLookat(Vec3f eye, Vec3f center, Vec3f up){
    View=lookat(eye,center,up);
   };
};
struct IShader {
    virtual ~IShader()=default;
    virtual glm::vec4 vertex(int iface, int nthvert,gl_enviroment&) = 0;
    virtual bool fragment(Vec3f bar, TGAColor &color) = 0;
}; 
Vec3f barycentric(Vec2i *pts, Vec2i P);
void line(int x0, int y0, int x1, int y1, TGAImage &image, TGAColor color) ;
inline void  line(Vec2i v0,Vec2i v1,TGAImage &image, TGAColor color);
void triangle(Vec3f *pts,Vec2f* _uv, float* v_intensity,IShader* shader,TGAImage &image,std::vector<double> &zbuffer ,TGAColor _color,gl_enviroment& envir);
void triangle(glm::vec4*pts, IShader &shader, TGAImage &image,std::vector<double> &zbuffer,gl_enviroment& envir);    
#endif