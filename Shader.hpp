
#ifndef SHADER_H
#define SHADER_H

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
Model *cur_model = nullptr;
vec3 _light_dir;
inline void initShaderEnvir(Model *_m,const vec3& l){
    cur_model=_m;
    _light_dir=l;
}
struct GouraudShader : public IShader {
    Vec3f varying_intensity;  
    virtual glm::vec4 vertex(int iface, int nthvert,const gl_enviroment& envir) {
        auto mt = (envir.Viewport*envir.Projection*envir.View) ;
        varying_intensity[nthvert] = std::max(0.0f, cur_model->normal(iface, nthvert)*_light_dir);  
        auto  gl_Vertex = embed4(cur_model->vert(iface, nthvert));  
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
    
    virtual glm::vec4 vertex(int iface, int nthvert,const gl_enviroment& envir) {
        auto now_uv=cur_model->uv(iface, nthvert);
        glm::mat3 t_uv;
        mat3_set_col(t_uv,nthvert,glm::vec3(now_uv.x,now_uv.y,1.0));
        varying_uv=glm::transpose(t_uv);
        //note like glsl glm all mat are colum-main,need transpose
        //varying_uv=glm::transpose(varying_uv);
        varying_intensity[nthvert] = std::max(0.0f, cur_model->normal(iface, nthvert)*_light_dir); 
        auto  gl_Vertex = embed4(cur_model->vert(iface, nthvert));  
        auto mt = (envir.Viewport*envir.Projection*envir.View) ;
        return mt*gl_Vertex;
    }
    
    virtual bool fragment(Vec3f bar, TGAColor &color) {
        //float intensity = varying_intensity*bar; 
        glm::vec3 _bar(bar.x,bar.y,bar.z);
       
        glm::vec3 vUV= varying_uv*_bar;
        Vec2f uv(vUV.x,vUV.y);
        auto tex_color_0=cur_model->diffuse(uv);
         auto _n4=uniform_MIT*embed4(cur_model->normal(uv));
          glm::vec3 n(_n4);n=glm::normalize(n);
    
        glm::vec3 l(uniform_M *embed4(_light_dir)); l=glm::normalize( l);
         float intensity = glm::max(0.0f,glm::dot(n,l));
        color = tex_color_0*intensity;    
 
        return false;                           
    }
};
struct NaiveToonShader : public IShader{
      Vec3f varying_intensity;
    
    virtual glm::vec4 vertex(int iface, int nthvert,const gl_enviroment& envir) {
        auto mt = (envir.Viewport*envir.Projection*envir.View) ;
        varying_intensity[nthvert] = std::max(0.0f, cur_model->normal(iface, nthvert)*_light_dir);  
        auto  gl_Vertex = embed4(cur_model->vert(iface, nthvert));  
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
class PhongShader_1 : public IShader {
    public:
    const gl_enviroment& _envir;
    Vec3f  varying_intensity;
    glm::vec3 l; 
    glm::mat3 varying_uv;         
    glm::mat3 varying_nrm; 
    glm::mat3 varying_pos_gobal;  
    glm::mat3 ndc_tri;    
    glm::mat4 uniform_M;   //  Projection*ModelView
    glm::mat4 uniform_MIT; // (Projection*ModelView).invert_transpose()
    glm::mat4 uniform_Mshadow; // transform framebuffer screen coordinates to shadowbuffer screen coordinates
    glm::mat4 Projection;
    glm::mat4 View;
    TGAImage* zmap{nullptr};
    mat<4,4> ModelView;
    mat<4,4> ProjectionMat;
    bool hasShadow{false};
    PhongShader_1(const gl_enviroment& envir,bool shadow=false,glm::mat4 shadowM=glm::mat4(1.0f),TGAImage* _zmap=nullptr):
    _envir(envir),
    hasShadow(shadow),
    Projection(envir.Projection),
    View(envir.View),
    uniform_Mshadow(shadowM),
    zmap{_zmap}
    {
        uniform_M=envir.Projection*envir.View;
        uniform_MIT=glm::transpose(glm::inverse(uniform_M));
        glm::vec4 light_loc=Projection*View*glm::vec4(_light_dir.x,_light_dir.y,_light_dir.z,0.0);
        l=glm::normalize(glm::vec3(light_loc.x,light_loc.y,light_loc.z));
    }
    glm::mat3 DarbouxFrame(const glm::vec3& bn)const {
 
         auto p0=col(ndc_tri,0);   auto p1=col(ndc_tri,1);   auto p2=col(ndc_tri,2);
         auto p0p1=p1-p0;
         auto p0p2=p2-p0;
         glm::mat3 matrixFramePostion=buildMat3FromColums(p0p1,p0p2,bn);
         auto A_I=glm::inverse(matrixFramePostion); 
         auto i=A_I*glm::vec3(varying_uv[0][1] - varying_uv[0][0], varying_uv[0][2] - varying_uv[0][0], 0);
         auto j=A_I*glm::vec3(varying_uv[1][1] - varying_uv[1][0], varying_uv[1][2] - varying_uv[1][0], 0);
         i=glm::normalize(i);      j=glm::normalize(j);
         //matrix order in example is the same in glm,
         glm::mat3 matrixFrameUV=glm::transpose(buildMat3FromColums(i,j,bn));
        
        return matrixFrameUV;
    } 
    virtual glm::vec4 vertex(int iface, int nthvert,const gl_enviroment& envir) {
        auto now_uv=cur_model->uv(iface, nthvert);
         //glm::mat3 t_uv;
         mat3_set_col(varying_uv,nthvert,glm::vec3(now_uv.x,now_uv.y,1.0));
        //note like glsl glm all mat are colum-main,need transpose
        //varying_uv=glm::transpose(t_uv);
         auto norm_from_verts=glm::normalize(glm_vec3(cur_model->normal(iface, nthvert)));
         glm::vec4 v_nrm4=uniform_MIT*glm::vec4(norm_from_verts.x,norm_from_verts.y,norm_from_verts.z,0.0);
         glm::vec3 v_nrm(v_nrm4.x,v_nrm4.y,v_nrm4.z);
         mat3_set_col (varying_nrm,nthvert,v_nrm);
   
     
        varying_intensity[nthvert] = std::max(0.0f, cur_model->normal(iface, nthvert)*_light_dir); 
        auto  gl_Vertex = embed4(cur_model->vert(iface, nthvert));  
      
        auto mt = (envir.Viewport*envir.Projection*envir.View) ;
        gl_Vertex =mt*gl_Vertex;
    
         mat3_set_col(ndc_tri,nthvert, projectV4toV3(gl_Vertex));
        
        return gl_Vertex;
    }
    
    virtual bool fragment(Vec3f bar, TGAColor &color) {
        //float intensity = varying_intensity*bar; 
        glm::vec3 _bar(bar.x,bar.y,bar.z);
        

        glm::vec3 vUV= glm::transpose(varying_uv)*_bar;
        glm::vec3 vPos=glm::transpose(ndc_tri)*_bar;
       float shadow=1.0;
       if(hasShadow){
                   glm::vec4 sb_p = uniform_Mshadow*glm::vec4(vPos.x,vPos.y,vPos.z,1.0); 
                    sb_p = sb_p/sb_p[3];
                   // int idx = int(sb_p[0]) + int(sb_p[1])*width;  
                    auto zmap_value=zmap->get((int)sb_p.x,(int)sb_p.y);
                    double d=zmap_value[0]/255.0*_envir.depth;
                    double bias=2.0;
                    shadow = .3+.7*((d-bias)<sb_p[2]); 
       }
        glm::vec3 normal_from_file = glm::normalize((glm::transpose(varying_nrm)*_bar)); // per-vertex normal interpolation
        Vec2f uv(vUV.x,vUV.y);

        //auto B=glm_mat3(_DarbouxFrame(vec_3(normal_from_file)));
       auto B=DarbouxFrame(normal_from_file);
        auto normal_from_tex=glm_vec3(cur_model->normal(uv));
         glm::vec3 n=(glm::length(normal_from_tex)==0.0)?normal_from_file:glm::normalize(B*normal_from_tex);
       //glm::vec3 n=normal_from_file;
        double diff = std::max(0.0f, glm::dot(n,l));
         glm::vec3 r=glm::normalize(2.0f*dot(n,l)*n - l);
    
     auto spec_from_tex=cur_model->specular(uv);
       double spec = std::pow(std::max(r.z, 0.f), 5.0+spec_from_tex);
          
     TGAColor c = cur_model->diffuse(uv);
     float ambient=20.0;
    
     for (int i=0; i<3; i++) color[i] = std::min<int>(ambient + c[i]*shadow*(1.2*diff + 0.6*spec), 255) ; 
            //color[i] =shadow*255.0;

        return false;                           
    }
};

 
class  DepthShader : public IShader {
    public:
    const gl_enviroment& _envir;
    glm::mat3 varying_tri;

    DepthShader(const gl_enviroment& envir) : varying_tri() ,_envir(envir){}

    virtual glm::vec4 vertex(int iface, int nthvert,const gl_enviroment& envir) {
        glm::vec4 gl_Vertex = embed4(cur_model->vert(iface, nthvert));  
         auto mt=envir.Viewport*envir.Projection*envir.View;
         gl_Vertex =mt*gl_Vertex;     
        glm::vec3 v3(gl_Vertex.x/gl_Vertex[3],gl_Vertex.y/gl_Vertex[3],gl_Vertex.z/gl_Vertex[3]);
         mat3_set_col(varying_tri,nthvert,v3);
         return gl_Vertex;
    }

    virtual bool fragment(Vec3f bar, TGAColor &color) {
        glm::vec3 _bar(bar.x,bar.y,bar.z);
        glm::vec3 p= glm::transpose(varying_tri)*_bar;
        color = TGAColor(255, 255, 255)*(p.z/_envir.depth);
        return false;
    }
};
//just for render zbuffer
class ZShader : public IShader {
    public:
    const gl_enviroment& _envir;
    glm::mat3 varying_uv; 

    glm::mat3 ndc_tri; 
    glm::mat3 clip_space_coord;                 
    glm::mat4 Projection;
    glm::mat4 View; 
    glm::mat4 uniform_M;
    ZShader(const gl_enviroment& envir):
    _envir(envir),
    Projection(envir.Projection),
    View(envir.View)
 
    {
        uniform_M=envir.Projection*envir.View;
       
    }
 
    virtual glm::vec4 vertex(int iface, int nthvert,const gl_enviroment& envir) {
        //set up UI
        auto now_uv=cur_model->uv(iface, nthvert);
        mat3_set_col(varying_uv,nthvert,glm::vec3(now_uv.x,now_uv.y,1.0));
  
       //set up  vertex
        auto  gl_Vertex = embed4(cur_model->vert(iface, nthvert));        
        auto mt = (envir.Projection*envir.View) ;
        
        gl_Vertex =mt*gl_Vertex;
        mat3_set_col(varying_tri,nthvert, projectV4toV3(gl_Vertex));
        gl_Vertex= envir.Viewport*gl_Vertex;
        mat3_set_col(ndc_tri,nthvert, projectV4toV3(gl_Vertex));
        
       return gl_Vertex;
    }
    
    virtual bool fragment(Vec3f bar, TGAColor &color) {
        
        glm::vec3 _bar(bar.x,bar.y,bar.z);
        

        glm::vec3 vUV= glm::transpose(varying_uv)*_bar;
        glm::vec3 vPos=glm::transpose(ndc_tri)*_bar;
 
 
        Vec2f uv(vUV.x,vUV.y);
  
        color = TGAColor(255, 255, 255)*((gl_FragCoord.z/_envir.depth+1.f)/2.f);
        return false;
                 
    }
};
  int sample_count=0;
class AOShader : public IShader {
    public:
    const gl_enviroment& _envir;
    glm::mat3 varying_uv; 
    glm::mat3 ndc_tri;         
    glm::mat4 Projection;
    glm::mat4 uniform_M;
    glm::mat4 View;
  
    const std::vector<double>* _zbuffer{nullptr};
    TGAImage* occl{nullptr};
    //true compute ao,false render with ao
    bool produceOrRender{false};
    AOShader(const gl_enviroment& envir,const std::vector<double>* zbuffer,TGAImage* _occl=nullptr,bool mode_flag=false):
    _envir(envir),
    Projection(envir.Projection),
    View(envir.View),
    _zbuffer(zbuffer),
    occl{_occl},
    produceOrRender{mode_flag}
    {
        uniform_M=envir.Projection*envir.View;
       
    }
 
    virtual glm::vec4 vertex(int iface, int nthvert,const gl_enviroment& envir) {
        //set up UV
        auto now_uv=cur_model->uv(iface, nthvert);
        mat3_set_col(varying_uv,nthvert,glm::vec3(now_uv.x,now_uv.y,1.0));
  
       //set up  vertex
        auto  gl_Vertex = embed4(cur_model->vert(iface, nthvert));        
        auto mt = (envir.Viewport*envir.Projection*envir.View) ;
        gl_Vertex =mt*gl_Vertex;
        mat3_set_col(ndc_tri,nthvert, projectV4toV3(gl_Vertex));
        
       return gl_Vertex;
    }
    
    virtual bool fragment(Vec3f bar, TGAColor &color) {
        
        glm::vec3 _bar(bar.x,bar.y,bar.z);

        const auto& zbuffer=*_zbuffer;

        glm::vec3 vUV= glm::transpose(varying_uv)*_bar;
        glm::vec3 vPos=glm::transpose(ndc_tri)*_bar;
 
 
        Vec2f uv(vUV.x,vUV.y);
      
        if(produceOrRender){
            auto flag =zbuffer[int(gl_FragCoord.x+gl_FragCoord.y*_envir.zbuffer_resolution.x)]-gl_FragCoord.z<1e-2;
            if (flag) {
                occl->set(uv.x*occl->width, uv.y*occl->height, TGAColor(255));
                color = TGAColor(255,0,0);
        }
        }else{
             int t = occl->get(uv.x*occl->width, uv.y*occl->height)[0];
             color = TGAColor(t, t, t);
        }
        return false;
                    
    }
};
 #endif