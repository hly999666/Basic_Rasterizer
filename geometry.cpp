#include "geometry.h"
 template <class t> Vec3<t> _cross(const Vec3<t> &v1, const Vec3<t> &v2){
	  return Vec3<t>{v1.y*v2.z - v1.z*v2.y, v1.z*v2.x - v1.x*v2.z, v1.x*v2.y - v1.y*v2.x};
 }