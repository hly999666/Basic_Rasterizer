#ifndef MY_MATH_HELPER_H
#define MY_MATH_HELPER_H
#include <glm/glm.hpp>
#ifndef GEOMETRY_H
#include "geometry.h"
#endif
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

#endif