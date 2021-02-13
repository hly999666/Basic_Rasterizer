#include <iostream>
#ifndef TGAIMAGE_H
#include "tgaimage.h"
#endif
#ifndef GEOMETRY_H
#include "geometry.h"
#endif
#ifndef MY_MATH_HELPER_H
#include "my_math_helper.hpp"
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
      glm::vec3 eye;
      glm::vec3 center;
      glm::vec3 up;
      glm::ivec2 zbuffer_resolution;
      bool enableZTest{true};
      int width;
      int height;
      int depth;
public:
     gl_enviroment(int w,int h,int d,
     Vec3f e=Vec3f(1,1,4),
     Vec3f c=Vec3f(0,0,0),
     Vec3f u=Vec3f(0,1,0),
     glm::ivec2 z_res=glm::ivec2(1024,1024)
     )
     {
       eye=glm_vec3(e);
       center=glm_vec3(c);
        up=glm_vec3(u);
        zbuffer_resolution=z_res;
      width=w;height=h;depth=d;
  }
   void setViewport(int x, int y, int w, int h,int d){
     Viewport=viewport (x,y,w,h,d);
   };
   void setProjection(float coeff){
     Projection=projection(coeff);
   };
   void setLookat(Vec3f _eye, Vec3f _center, Vec3f _up){
    View=lookat(_eye,_center,_up);
    eye=glm_vec3(_eye);
    center=glm_vec3(_center);
    up=glm_vec3(_up);
   };
};
class  IShader {
     public:
     Vec3f gl_FragCoord;
    virtual ~IShader()=default;
    virtual glm::vec4 vertex(int iface, int nthvert,const gl_enviroment&) = 0;
    virtual bool fragment(Vec3f bar, TGAColor &color) = 0;
}; 
Vec3f barycentric(Vec2i *pts, Vec2i P);
void line(int x0, int y0, int x1, int y1, TGAImage &image, TGAColor color) ;
inline void  line(Vec2i v0,Vec2i v1,TGAImage &image, TGAColor color);
void triangle(Vec3f *pts,Vec2f* _uv, float* v_intensity,IShader* shader,TGAImage &image,std::vector<double> &zbuffer ,TGAColor _color,gl_enviroment& envir);
void triangle(glm::vec4*pts, IShader &shader, TGAImage &image,std::vector<double> &zbuffer,const gl_enviroment& envir);    
#endif