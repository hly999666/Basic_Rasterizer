#ifndef GEOMETRY_H
#include "geometry.h"
#endif

template <> template <> 
Vec3<int>::Vec3<>(const Vec3<float> &v):x(int(v.x+.5)), y(int(v.y+.5)), z(int(v.z+.5)) {}
template <> template <> 
Vec3<float>::Vec3<>(const Vec3<int> &v):x(v.x), y(v.y), z(v.z) {}

Vec3f m2v(Matrix m) {
    return Vec3f(m[0][0]/m[3][0], m[1][0]/m[3][0], m[2][0]/m[3][0]);
}

Matrix v2m(Vec3f v) {
    Matrix m(4, 1);
    m[0][0] = v.x;
    m[1][0] = v.y;
    m[2][0] = v.z;
    m[3][0] = 1.f;
    return m;
}
Matrix viewport(int x, int y, int w, int h,int depth) {
    Matrix m = Matrix::identity(4);
    m[0][3] = x+w/2.f;
    m[1][3] = y+h/2.f;
    m[2][3] = depth/2.f;

    m[0][0] = w/2.f;
    m[1][1] = h/2.f;
    m[2][2] = depth/2.f;
    return m;
}

Matrix lookat(Vec3f eye, Vec3f center, Vec3f up) {
    Vec3f z = (eye-center).normalize();
    Vec3f x = (up^z).normalize();
    Vec3f y = (z^x).normalize();
    Matrix res = Matrix::identity(4);
    for (int i=0; i<3; i++) {
        res[0][i] = x[i];
        res[1][i] = y[i];
        res[2][i] = z[i];
        res[i][3] = -center[i];
    }
    return res;
}
