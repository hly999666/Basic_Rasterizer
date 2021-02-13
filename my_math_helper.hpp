#ifndef MY_MATH_HELPER_H
#define MY_MATH_HELPER_H
#include <glm/glm.hpp>
#ifndef GEOMETRY_H
#include "geometry.h"
#endif
#ifndef __GEOMETRY_A_H__
#include "geometry_a.h"
#endif
#ifndef M_PI
#define M_PI 3.1415926535898
#endif
 
#include <limits>
inline glm::vec3 col(const glm::mat3& mat,int i){
    return glm::vec3(mat[0][i],mat[1][i],mat[2][i]);
}
inline void mat3_set_col(glm::mat3& mat,int i,const glm::vec3&v){
   mat[0][i]=v.x; mat[1][i]=v.y;mat[2][i]=v.z;
}
inline glm::mat3 buildMat3FromColums(
     const glm::vec3&v0,
     const glm::vec3&v1,
     const glm::vec3&v2
){
    glm::mat3 ans;
    mat3_set_col(ans,0,v0);
    mat3_set_col(ans,1,v1);
    mat3_set_col(ans,2,v2);
    return ans;
}
inline glm::vec3 glm_vec3(const Vec3f& v ){
    return glm::vec3(v.x,v.y,v.z);
}

inline vec<3> vec_3 (const glm::vec3& v ){
    return vec<3> (v.x,v.y,v.z);
}
inline Vec3f Vec_3f (const glm::vec3& v ){
    return Vec3f (v.x,v.y,v.z);
}
inline glm::mat3 glm_mat3 (const mat<3,3>& m ){
    glm::mat3 ans;
    for(int i=0;i<3;i++){
          for(int j=0;j<3;j++){
        ans[i][j]=m[i][j];
    }
    }
    return glm::transpose(ans);
}
inline glm::vec3 projectV4toV3(const glm::vec4& v ){
    return glm::vec3(v.x/v[3],v.y/v[3],v.z/v[3]);
}
inline void clear_zbuffer(std::vector<double>&zbuffer){
       for(auto& val:zbuffer)val= -std::numeric_limits<float>::max();
}
inline glm::vec3 samplingOverSphere(){
     float u = (float)rand()/(float)RAND_MAX;
    float v = (float)rand()/(float)RAND_MAX;
    float theta = 2.f*M_PI*u;
    float phi   = acos(2.f*v - 1.f);
    return glm::vec3(sin(phi)*cos(theta), sin(phi)*sin(theta), cos(phi));
}
#endif