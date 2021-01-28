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

#include "lib/vec3.hpp"
#include "lib/general_helper.hpp"
#include "tgaimage.h"
const int ny=256;
const int nx=256;
void test_output(std::vector<vec3>& fb){
   for(int i=0;i<nx;i++){
        for(int j=0;j<ny;j++){
       fb[i*ny+j].e[0]=(double)i/nx;
       fb[i*ny+j].e[1]=(double)j/ny;
       fb[i*ny+j].e[2]=0.5;
}


}
}

const TGAColor white = TGAColor(255, 255, 255, 255);
const TGAColor red   = TGAColor(255, 0,   0,   255);
 
int main(int argc, char **argv) {


      TGAImage image_1(100, 100, TGAImage::RGB);
        image_1.set(52, 41, red);
        image_1.flip_vertically(); 
        image_1.write_tga_file("output.tga");
       return 0;

std::vector<vec3> framebuffer;
framebuffer.resize(nx*ny);
test_output(framebuffer);

    int key = 0;
    int pixel_d=1;
  while (key != 27) {
        auto buffer_1=convertFrameBuffer(framebuffer,pixel_d);
        cv::Mat image(nx, ny, CV_8UC3,buffer_1.data());
        image=image.t();
        image.convertTo(image, CV_8UC3, 1.0f);
        
        cv::imshow("image", image);
        key = cv::waitKey(10);
    }




  return 0;
}