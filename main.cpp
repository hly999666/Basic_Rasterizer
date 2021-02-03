#include <iostream>
 #include <limits>
#include<chrono>
#include <fstream>
#include <random>
#include <string>
#include <vector>
#include<memory> 
#include<thread> 
#include<atomic> 
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
Model *model = NULL;
const int nx  = 512;
const int ny = 512;
const int width  = nx;
const int height = ny;
const int depth  = 255;
double  zbuffer[width*height];
const TGAColor white = TGAColor(255, 255, 255, 255);
const TGAColor red   = TGAColor(255, 0,   0,   255);
const TGAColor green   = TGAColor(0, 255,   0,   255);
const Vec3f light_dir = Vec3f(1,-1,1).normalize();
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
 
void triangle(Vec3f *pts,Vec2f* _uv, float* v_intensity,TGAImage &image, TGAColor _color) { 
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
                uv.x+=(double)_uv[i][0]*(double)bc_screen[i];
                uv.y+=(double)_uv[i][1]*(double)bc_screen[i];
                intensity_interpolated+=(double)v_intensity[i]*(double)bc_screen[i];
            }
            intensity_interpolated=std::max(0.0,(double)intensity_interpolated);
              if (P.x>=width||P.y>=height||P.x<0||P.y<0) continue;
              if (zbuffer[int(P.x+P.y*width)]<z) {
                zbuffer[int(P.x+P.y*width)]=z;
                 
        /*         auto tex_color=color_map->get(uv.x,uv.y);
                double factor=1.0;
                factor=((double)color.bgra[0]/255.0);
                auto filled_color=tex_color*factor; */
                image.set(P.x, P.y,TGAColor(255*intensity_interpolated,255*intensity_interpolated,255*intensity_interpolated,255));
            } 
              //image.set(P.x, P.y, color);
        } 
    } 
} 
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
void drawModelFilled(Model* model,TGAImage &image){
        Matrix ModelView  = lookat(camera, lookAtPos, Vec3f(0,1,0));
        Matrix Projection = Matrix::identity(4);
        Matrix ViewPort   = viewport(width/8, height/8, width*3/4, height*3/4,depth);
        Projection[3][2] = -1.f/(camera-lookAtPos).norm();


    Matrix mt = (ViewPort*Projection*ModelView);
   #pragma omp parallel for
    for (int i=0; i<model->nfaces(); i++) { 
 
    Vec3f screen_coords[3]; 
    Vec3f world_coords[3]; 
    Vec2f uv[3];
     float v_intensity[3];
    for (int j=0; j<3; j++) { 
        Vec3f v =  model->vert(i,j); 
        screen_coords[j] = m2v(mt*v2m(v)); 
        uv[j]= model->uv(i,j);
        world_coords[j]=v;
        v_intensity[j] = model->normal(i, j).normalize()*light_dir;
    } 
     Vec3f n = (world_coords[2]-world_coords[0])^(world_coords[1]-world_coords[0]); 
    n.normalize(); 
    float intensity = n*light_dir; 
    //triangle(screen_coords,uv, v_intensity,image, TGAColor(255,255,255, 255)); 
    triangle(screen_coords,uv, v_intensity,image, TGAColor(255,255,255, 255)); 
} 
}

 

int main(int argc, char **argv) {
  
    if (2==argc) {
        model = new Model(argv[1]);
    } else {
        model = new Model("./model/african_head.obj");
    } 
 
      color_map->read_tga_file("./model/texture/african_head_diffuse.tga");
     color_map->flip_vertically();
      TGAImage image_1(ny, nx, TGAImage::RGB);

      for(double&i:zbuffer)i=-100000.0;
     drawModelFilled(model,image_1);

 

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