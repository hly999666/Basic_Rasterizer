#include <iostream>
 #include <limits>
#include<chrono>
#include <fstream>
#include <random>
#include <string>
#include <vector>
#include<thread> 
#include<atomic> 
#include <omp.h>
 
#include <opencv2/opencv.hpp>
#include <glm/glm.hpp>
//#include "lib/vec3.hpp"
//#include "lib/general_helper.hpp"
#include "tgaimage.h"
#include "geometry.h"
#include "model.h"
Model *model = NULL;
const int nx  = 256;
const int ny = 256;
const int width  = nx;
const int height = ny;
const TGAColor white = TGAColor(255, 255, 255, 255);
const TGAColor red   = TGAColor(255, 0,   0,   255);
const TGAColor green   = TGAColor(0, 255,   0,   255);
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
void drawModelWireframe(Model* _model,TGAImage &image_1){
      #pragma omp parallel for
      for (int i=0; i<_model->nfaces(); i++) {
        std::vector<int> face = _model->face(i);
        for (int j=0; j<3; j++) {
            Vec3f v0 = _model->vert(face[j]);
            Vec3f v1 = _model->vert(face[(j+1)%3]);
            int x0 = (v0.x+1.)*width/2.;
            int y0 = (v0.y+1.)*height/2.;
            int x1 = (v1.x+1.)*width/2.;
            int y1 = (v1.y+1.)*height/2.;
            line(x0, y0, x1, y1, image_1, white);
        }
    }
}

void test_tree_map(TGAImage &image_1){
        using namespace glm;
        float pd=(double)nx/4.0;
    vec2 pos_a[4];
    for(int x=0;x<nx;x++){
        for(int y=0;y<ny;y++){
          vec3 p(x,0.0,y);
          vec2 n = floor( vec2(p.x,p.z)*(1.0f/pd));
         
          vec2 f = fract( vec2(p.x,p.z)*(1.0f/pd));
       
          vec2 grid_point=step(f,vec2(0.5));
          int count=0;
    for( int j=0; j<=1; j++ ){
       for( int i=0; i<=1; i++ )
    {  
        vec2 index=vec2( float(i), float(j) ) ;
        vec2  g =index -grid_point ;
        vec2 global= n + g;
         
        vec2  r = f-g;
       pos_a[count]=global;count++;
    }
    }

          image_1.set(x,y, TGAColor(255*f.x, 255*f.y, 0, 255)); 

}
}
} 


Vec3f barycentric(Vec2i *pts, Vec2i P) { 
    Vec3f v1(pts[2][0]-pts[0][0], pts[1][0]-pts[0][0], pts[0][0]-P[0]);
    Vec3f v2(pts[2][1]-pts[0][1], pts[1][1]-pts[0][1], pts[0][1]-P[1]);
    Vec3f u = _cross(v1,v2);
    /* `pts` and `P` has integer value as coordinates,
       so `abs(u[2])` < 1 means `u[2]` is 0, that means
       triangle is degenerate, so no pixel should be filled,return rejected case */
    if (std::abs(u[2])<1) return Vec3f(-1,1,1);
    return Vec3f(1.f-(u.x+u.y)/u.z, u.y/u.z, u.x/u.z); 
} 
 
void triangle(Vec2i *pts, TGAImage &image, TGAColor color) { 
    Vec2i bboxmin(image.get_width()-1,  image.get_height()-1); 
    Vec2i bboxmax(0, 0); 
    Vec2i clamp(image.get_width()-1, image.get_height()-1); 
    for (int i=0; i<3; i++) { 
        for (int j=0; j<2; j++) { 
            bboxmin[j] = std::max(0,        std::min(bboxmin[j], pts[i][j])); 
            bboxmax[j] = std::min(clamp[j], std::max(bboxmax[j], pts[i][j])); 
        } 
    } 
    Vec2i P; 
    for (P.x=bboxmin.x; P.x<=bboxmax.x; P.x++) { 
        for (P.y=bboxmin.y; P.y<=bboxmax.y; P.y++) { 
            Vec3f bc_screen  = barycentric(pts, P); 
            if (bc_screen.x<0 || bc_screen.y<0 || bc_screen.z<0) continue; 
            image.set(P.x, P.y, color); 
        } 
    } 
} 

 

int main(int argc, char **argv) {
    if (2==argc) {
        model = new Model(argv[1]);
    } else {
        model = new Model("./model/african_head.obj");
    }
      
      TGAImage image_1(ny, nx, TGAImage::RGB);
    
     //drawModelWireframe(model,image_1);

Vec2i t0[3] = {Vec2i(10, 70),   Vec2i(50, 160),  Vec2i(70, 80)}; 
Vec2i t1[3] = {Vec2i(180, 50),  Vec2i(150, 1),   Vec2i(70, 180)}; 
Vec2i t2[3] = {Vec2i(180, 150), Vec2i(120, 160), Vec2i(130, 180)}; 
triangle(t0, image_1, red); 
triangle(t1, image_1, white); 
triangle(t2, image_1, green);

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