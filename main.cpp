#include <iostream>
#include <limits>
#include <chrono>
#include <fstream>
#include <random>
#include <string>
#include <vector>
#include <memory> 
#include <thread> 
#include <atomic> 
#include <omp.h>

#include <opencv2/opencv.hpp>
#include <glm/glm.hpp>
//#include "lib/vec3.hpp"
//#include "lib/general_helper.hpp"
#include "tgaimage.h"
#ifndef GEOMETRY_H
#include "geometry.h"
#endif
#ifndef MODEL_H
#include "model.h"
#endif
#ifndef OUR_GL_H
#include "our_GL.hpp"
#endif
Model *model = NULL;
const int nx  = 512;
const int ny = 512;
const int width  = nx;
const int height = ny;
const int depth  = 255;
 
const Vec3f light_dir = Vec3f(1,1,1).normalize();
 Vec3f camera(1,1,3);
 Vec3f lookAtPos(0,0,0);
 std::shared_ptr<TGAImage> color_map{new TGAImage()};
void test_output(TGAImage& fb){
   for(int i=0;i<nx;i++){
    
        for(int j=0;j<ny;j++){
          double r=(double)i/nx;
          double g=(double)j/ny;
          double b=0.5;
          fb.set(i, j, TGAColor(255*r, 255*g, 255*b, 255)); 
     
}


}
}

struct GouraudShader : public IShader {
    Vec3f varying_intensity;  
    virtual glm::vec4 vertex(int iface, int nthvert,gl_enviroment& envir) {
        auto mt = (envir.Viewport*envir.Projection*envir.View) ;
        varying_intensity[nthvert] = std::max(0.f, model->normal(iface, nthvert)*light_dir);  
        auto  gl_Vertex = embed4(model->vert(iface, nthvert));  
        return mt*gl_Vertex;  
    }
    virtual bool fragment(Vec3f bar, TGAColor &color) {
        float intensity = varying_intensity*bar;   
        color = TGAColor(255, 255, 255)*intensity; 
        return false;                        
     
    }
};
class ComplexShader_1 : public IShader {
    public:
    Vec3f          varying_intensity;  
    glm::mat3 varying_uv;        // same as above
    glm::mat4  uniform_M;   //  Projection*ModelView
    glm::mat4 uniform_MIT; // (Projection*ModelView).invert_transpose()
    virtual glm::vec4 vertex(int iface, int nthvert,gl_enviroment& envir) {
        auto now_uv=model->uv(iface, nthvert);
        glm::mat3 t_uv;
         t_uv[0][nthvert]=now_uv.x;
         t_uv[1][nthvert]=now_uv.y;
         t_uv[2][nthvert]=0.0;
          varying_uv=glm::transpose(t_uv);
        //note like glsl glm all mat are colum-main,need transpose
        //varying_uv=glm::transpose(varying_uv);
        varying_intensity[nthvert] = std::max(0.f, model->normal(iface, nthvert)*light_dir); 
        auto  gl_Vertex = embed4(model->vert(iface, nthvert));  
      
           auto mt = (envir.Viewport*envir.Projection*envir.View) ;
        return mt*gl_Vertex;
    }
    
    virtual bool fragment(Vec3f bar, TGAColor &color) {
        //float intensity = varying_intensity*bar; 
        glm::vec3 _bar(bar.x,bar.y,bar.z);
       
        glm::vec3 vUV= varying_uv*_bar;
        Vec2f uv(vUV.x,vUV.y);
        auto tex_color_0=model->diffuse(uv);
         auto _n4=uniform_MIT*embed4(model->normal(uv));
          glm::vec3 n(_n4);n=glm::normalize(n);
    
        glm::vec3 l(uniform_M *embed4(light_dir)); l=glm::normalize( l);
         float intensity = glm::max(0.0f,glm::dot(n,l));
        color = tex_color_0*intensity;    
 
        return false;                           
    }
};

struct NaiveToonShader : public IShader{
      Vec3f varying_intensity;
    
    virtual glm::vec4 vertex(int iface, int nthvert,gl_enviroment& envir) {
        auto mt = (envir.Viewport*envir.Projection*envir.View) ;
        varying_intensity[nthvert] = std::max(0.f, model->normal(iface, nthvert)*light_dir);  
        auto  gl_Vertex = embed4(model->vert(iface, nthvert));  
        return mt*gl_Vertex;  
    }
    virtual bool fragment(Vec3f bar, TGAColor &color) {
 
         float intensity = varying_intensity*bar;
        if (intensity>.85) intensity = 1;
        else if (intensity>.60) intensity = .80;
        else if (intensity>.45) intensity = .60;
        else if (intensity>.30) intensity = .45;
        else if (intensity>.15) intensity = .30;
        else intensity = 0;
        color = TGAColor(255, 155, 0)*intensity;
        return false;
    }
};
 

void drawModelWireframe(Model* _model,TGAImage &image_1){
      #pragma omp parallel for
      for (int i=0; i<_model->nfaces(); i++) {
       
        for (int j=0; j<3; j++) {
            Vec3f v0 = _model->vert(i,j);
            Vec3f v1 = _model->vert(i,(j+1)%3);
            int x0 = (v0.x+1.)*width/2.;
            int y0 = (v0.y+1.)*height/2.;
            int x1 = (v1.x+1.)*width/2.;
            int y1 = (v1.y+1.)*height/2.;
            line(x0, y0, x1, y1, image_1, white);
        }
    }
}
void drawModelFilled(Model* model,TGAImage &image,std::vector<double>& zbuffer,gl_enviroment& envir){
        envir.setLookat(camera, lookAtPos, Vec3f(0,1,0));
        envir.setViewport(width/8, height/8, width*3/4, height*3/4,depth);
        envir.setProjection((camera-lookAtPos).norm());
  // #pragma omp parallel for

      
    for (int i=0; i<model->nfaces(); i++) { 
 
    glm::vec4 screen_coords[3]; 
    Vec3f world_coords[3]; 
    Vec2f uv[3];
     float v_intensity[3];
     ComplexShader_1 shader;
        shader.uniform_M=envir.Projection*envir.View;
        shader.uniform_MIT=glm::transpose(glm::inverse(shader.uniform_M));
    for (int j=0; j<3; j++) { 
        Vec3f v =  model->vert(i,j); 
        
        screen_coords[j] =shader.vertex(i,j,envir); 
    /*     uv[j]= model->uv(i,j);
        world_coords[j]=v;
        v_intensity[j] = model->normal(i, j).normalize()*light_dir; */
    } 
/*      Vec3f n = (world_coords[2]-world_coords[0])^(world_coords[1]-world_coords[0]); 
    n.normalize(); 
    float intensity = n*light_dir;  */
    //triangle(screen_coords,uv, v_intensity,image, TGAColor(255,255,255, 255)); 
    triangle(screen_coords,shader,image, zbuffer,envir); 
} 
}

 

int main(int argc, char **argv) {
  
    if (2==argc) {
        model = new Model(argv[1]);
    } else {
        model = new Model("./model/african_head/african_head.obj");
    } 
 
/*       color_map->read_tga_file("./model/texture/african_head_diffuse.tga");
     color_map->flip_vertically(); */
      TGAImage image_1(ny, nx, TGAImage::RGB);
       std::vector<double>  zbuffer;zbuffer.resize(width*height);
      for(double&i:zbuffer)i=-8196.0;
      gl_enviroment envir(width,height,depth);

     drawModelFilled(model,image_1,zbuffer,envir);

 

   //image_1.flip_vertically();
  /* std::vector<vec3> framebuffer;
      framebuffer.resize(nx*ny);
      test_output(framebuffer); */

    int key = 0;
    int pixel_d=1;
    cv::Mat image(nx, ny, CV_8UC3,image_1.data.data());
    //image=image.t();
    image.convertTo(image, CV_8UC3);
    cv::Mat image_fliped;
    cv::flip(image, image_fliped, 0);
    //cv::flip(image, image_fliped, 1);
  while (key != 27) {
        //auto buffer_1=convertFrameBuffer(framebuffer,pixel_d);
        cv::imshow("image", image_fliped);
        
        key = cv::waitKey(10);
    }
  image_1.write_tga_file("output.tga");
  return 0;
}