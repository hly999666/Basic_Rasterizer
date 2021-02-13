#ifndef  RENDERER_H
#define RENDERER_H

#ifndef TGAIMAGE_H
#include "tgaimage.h"
#endif
#ifndef MODEL_H
#include "model.h"
#endif
#ifndef GEOMETRY_H
#include "geometry.h"
#endif

#ifndef __GEOMETRY_A_H__
#include "geometry_a.h"
#endif

#ifndef MY_MATH_HELPER_H
#include "my_math_helper.hpp"
#endif
#ifndef MODEL_H
#include "cur_model.h"
#endif
#ifndef OUR_GL_H
#include "our_GL.hpp"
#endif
#ifndef SHADER_H
#include "Shader.hpp"
#endif
#include<vector>
#include<iostream>
#include<chrono>
class AORenderer{
    public:
    Model* model;
    const gl_enviroment& envir;
    AORenderer(Model*_m,const gl_enviroment&  e):model(_m),envir(e){};
    void renderAOMap(const int sample,TGAImage& result){
        auto clock=std::chrono::high_resolution_clock::now().time_since_epoch().count();
             srand(clock);
             glm::ivec2 ao_res(envir.width,envir.height);
 
            glm::ivec2 zbuffer_res(envir.width,envir.height);
            std::vector<double>zbuffer_1;
            zbuffer_1.resize(zbuffer_res.x*zbuffer_res.y);
             std::vector<double>zbuffer_2;
            zbuffer_2.resize(zbuffer_res.x*zbuffer_res.y);
          TGAImage  total(envir.width,envir.height, TGAImage::RGB);
            for(int iter=0;iter<sample;iter++){
                  std::cout<<"AO Sampling:"<<iter+1<<"/sample"<<   std::endl;
                  clear_zbuffer(zbuffer_1); clear_zbuffer(zbuffer_2);
                   auto sampleLight=samplingOverSphere();
                   sampleLight.y=abs(sampleLight.y);
                   glm::vec3 _up;
                     for (int i=0; i<3; i++) _up[i] = (float)rand()/(float)RAND_MAX;
                  gl_enviroment cur_envir(envir.width,envir.height,envir.depth);
                  cur_envir.zbuffer_resolution=zbuffer_res;
                   cur_envir.setLookat(Vec_3f(sampleLight*(float)envir.width),Vec_3f(envir.center), Vec_3f(_up));
                   cur_envir.setViewport(envir.width/8, envir.height/8, envir.width*3/4, envir.height*3/4,envir.depth);
                   cur_envir.setProjection(0.0);
                     TGAImage z_map(envir.width, envir.height, TGAImage::RGB);
              #pragma omp parallel for
                for (int i=0; i<model->nfaces(); i++) {
                      ZShader shader(cur_envir);
                     glm::vec4 screen_coords[3];
                         for (int j=0; j<3; j++) {
                                 screen_coords[j] = shader.vertex(i, j,cur_envir);
                             }
                          triangle(screen_coords, shader, z_map,zbuffer_1, cur_envir);
                   }
                 /*   result=z_map;
                   return; */

        
              TGAImage ao_map(envir.width, envir.height, TGAImage::GRAYSCALE);
            #pragma omp parallel for
                for (int i=0; i<model->nfaces(); i++) {
                     AOShader shader(cur_envir,&zbuffer_1,&ao_map,true);
                     glm::vec4 screen_coords[3];
                         for (int j=0; j<3; j++) {
                                 screen_coords[j] = shader.vertex(i, j,cur_envir);
                             }
                          triangle(screen_coords, shader, z_map,zbuffer_2, cur_envir);
                   }
       for (int x=0; x<envir.width; x++) {
            for (int y=0; y<envir.height; y++) {
                float tmp = total.get(x,y)[0];
                float now=(tmp*(iter-1)+ao_map.get(x,y)[0])/(float)iter+.5f;
                total.set(x, y, TGAColor(now,now,now));
            }
        }
            }
 
                   result=total;
    }
    void renderModelWithAO( TGAImage& ao_map,TGAImage& result){
                     
            glm::ivec2 zbuffer_res(envir.width,envir.height);
            std::vector<double>zbuffer_1;
            zbuffer_1.resize(zbuffer_res.x*zbuffer_res.y);
             
            #pragma omp parallel for
                for (int i=0; i<model->nfaces(); i++) {
                    AOShader shader(envir,&zbuffer_1,&ao_map,false);
                     glm::vec4 screen_coords[3];
                         for (int j=0; j<3; j++) {
                                 screen_coords[j] = shader.vertex(i, j,envir);
                             }
                          triangle(screen_coords, shader, result,zbuffer_1, envir);
                   }
             
    }
};
#endif