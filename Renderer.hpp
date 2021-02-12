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
class AORenderer{
    public:
    Model* model;
    const gl_enviroment& envir;
    AORenderer(Model*_m,const gl_enviroment&  e):model(_m),envir(e){};
    void renderAOMap(const int sample,TGAImage& result){
             glm::ivec2 ao_res(1024,1024);
            std::vector<unsigned int> ao_stat;
            ao_stat.resize(ao_res.x*ao_res.y);
            for(auto& i:ao_stat)i=0;
            glm::ivec2 zbuffer_res(1024,1024);
            std::vector<double>zbuffer;
            zbuffer.resize(zbuffer_res.x*zbuffer_res.y);
          
            for(int i=0;i<sample;i++){
                   auto sampleLight=samplingOverSphere();
                   sampleLight.y=abs(sampleLight.y);
                    clear_zbuffer(zbuffer);
            }
    }
    void renderModelWithAO(TGAImage& result){

    }
};
#endif