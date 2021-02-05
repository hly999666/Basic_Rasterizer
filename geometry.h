#ifndef GEOMETRY_H
#define GEOMETRY_H
#include <glm/glm.hpp>
#include <cmath>
#include <iostream>
#include <cassert>
#include <cmath>
#include <vector>
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

template <class t> struct Vec2 {
	union {
		struct {t u, v;};
		struct {t x, y;};
		t raw[2];
	};
	Vec2() : u(0), v(0) {}
	Vec2(t _u, t _v) : u(_u),v(_v) {}
	Vec2(const Vec2<t> &v) : x(t()), y(t()) { *this = v; }
	Vec2<t>& operator=(const Vec2<t> &v) {
        if (this != &v) {
            x = v.x;
            y = v.y;
        }
        return *this;
    }
	inline Vec2<t> operator +(const Vec2<t> &V) const { return Vec2<t>(u+V.u, v+V.v); }
	inline Vec2<t> operator -(const Vec2<t> &V) const { return Vec2<t>(u-V.u, v-V.v); }
	inline Vec2<t> operator *(float f)          const { return Vec2<t>(u*f, v*f); }
	inline t operator[](int i)const {return raw[i];}
	inline t& operator[](int i) {return raw[i];}
	template <class > friend std::ostream& operator<<(std::ostream& s, Vec2<t>& v);
};

template <class t> struct Vec3 {
	union {
		struct {t x, y, z;};
		struct { t ivert, iuv, inorm; };
		t raw[3];
	};
	Vec3() : x(0), y(0), z(0) {}
	Vec3(t _x, t _y, t _z) : x(_x),y(_y),z(_z) {}
	template <class u> Vec3<t>(const Vec3<u> &v);
    Vec3<t>(const Vec3<t> &v) : x(t()), y(t()), z(t()) { *this = v; }
	//cross
	inline Vec3<t> operator ^(const Vec3<t> &v) const { return Vec3<t>(y*v.z-z*v.y, z*v.x-x*v.z, x*v.y-y*v.x); }
	inline Vec3<t> operator +(const Vec3<t> &v) const { return Vec3<t>(x+v.x, y+v.y, z+v.z); }
	inline Vec3<t> operator -(const Vec3<t> &v) const { return Vec3<t>(x-v.x, y-v.y, z-v.z); }
	inline Vec3<t> operator *(float f)          const { return Vec3<t>(x*f, y*f, z*f); }
	inline t       operator *(const Vec3<t> &v) const { return x*v.x + y*v.y + z*v.z; }
	float norm () const { return std::sqrt(x*x+y*y+z*z); }
	Vec3<t> & normalize(t l=1) { *this = (*this)*(l/norm()); return *this; }
	inline t operator[](int i)const {return raw[i];}
	inline t& operator[](int i) {return raw[i];}
	template <class > friend std::ostream& operator<<(std::ostream& s, Vec3<t>& v);
};
typedef Vec3<float> vec3;
typedef Vec2<float> vec2;
typedef Vec2<float> Vec2f;
typedef Vec2<int>   Vec2i;
typedef Vec3<float> Vec3f;
typedef Vec3<int>   Vec3i;

template <class t> std::ostream& operator<<(std::ostream& s, Vec2<t>& v) {
	s << "(" << v.x << ", " << v.y << ")\n";
	return s;
}

template <class t> std::ostream& operator<<(std::ostream& s, Vec3<t>& v) {
	s << "(" << v.x << ", " << v.y << ", " << v.z << ")\n";
	return s;
}
//template can't implement in cpp file
 template <class t> Vec3<t> _cross(const Vec3<t> &v1, const Vec3<t> &v2) {
	  return Vec3<t>{v1.y*v2.z - v1.z*v2.y, v1.z*v2.x - v1.x*v2.z, v1.x*v2.y - v1.y*v2.x};
 } 

 //de-template by  template <> template <>,type is fixed
template <> template <> 
Vec3<int>::Vec3<>(const Vec3<float> &v);
/*  : x(int(v.x+.5)), y(int(v.y+.5)), z(int(v.z+.5)) {} */
template <> template <> 
Vec3<float>::Vec3<>(const Vec3<int> &v);
/* : x(v.x), y(v.y), z(v.z) {} */
const int DEFAULT_ALLOC=4;
 

class Matrix {
    std::vector<std::vector<float> > m;
    int rows, cols;
public:
    Matrix(int r=DEFAULT_ALLOC, int c=DEFAULT_ALLOC):m(std::vector<std::vector<float> >(r, std::vector<float>(c, 0.f))), rows(r), cols(c) { };
    inline int nrows(){return rows;};
    inline int ncols() {return cols;};

    static Matrix identity(int dimensions){
	   Matrix E(dimensions, dimensions);
        for (int i=0; i<dimensions; i++) {
           for (int j=0; j<dimensions; j++) {
            E[i][j] = (i==j ? 1.f : 0.f);
           }
           }
        return E;
	};
    std::vector<float>& operator[](const int i){
		 assert(i>=0 && i<rows);
         return m[i];
	};
    Matrix operator*(const Matrix& a){
	assert(cols == a.rows);
     Matrix result(rows, a.cols);
    for (int i=0; i<rows; i++) {
        for (int j=0; j<a.cols; j++) {
            result.m[i][j] = 0.f;
            for (int k=0; k<cols; k++) {
                result.m[i][j] += m[i][k]*a.m[k][j];
            }
        }
    }
    return result;
	};
    Matrix transpose(){
		   Matrix result(cols, rows);
    for(int i=0; i<rows; i++)
        for(int j=0; j<cols; j++)
            result[j][i] = m[i][j];
    return result;
	};
    Matrix inverse(){
	assert(rows==cols);
    // augmenting the square matrix with the identity matrix of the same dimensions a => [ai]
    //Gauss-Jordan Elimination
	Matrix result(rows, cols*2);
    for(int i=0; i<rows; i++)
        for(int j=0; j<cols; j++)
            result[i][j] = m[i][j];
    for(int i=0; i<rows; i++)
        result[i][i+cols] = 1;
    // first pass
    for (int i=0; i<rows-1; i++) {
        // normalize the first row
        for(int j=result.cols-1; j>=0; j--)
            result[i][j] /= result[i][i];
        for (int k=i+1; k<rows; k++) {
            float coeff = result[k][i];
            for (int j=0; j<result.cols; j++) {
                result[k][j] -= result[i][j]*coeff;
            }
        }
    }
    // normalize the last row
    for(int j=result.cols-1; j>=rows-1; j--)
        result[rows-1][j] /= result[rows-1][rows-1];
    // second pass
    for (int i=rows-1; i>0; i--) {
        for (int k=i-1; k>=0; k--) {
            float coeff = result[k][i];
            for (int j=0; j<result.cols; j++) {
                result[k][j] -= result[i][j]*coeff;
            }
        }
    }
    // cut the identity matrix back
    Matrix truncate(rows, cols);
    for(int i=0; i<rows; i++)
        for(int j=0; j<cols; j++)
            truncate[i][j] = result[i][j+cols];
    return truncate;
	};

    friend std::ostream& operator<<(std::ostream& s, Matrix& m){
		 for (int i=0; i<m.nrows(); i++)  {
        for (int j=0; j<m.ncols(); j++) {
            s << m[i][j];
            if (j<m.ncols()-1) s << "\t";
        }
        s << "\n";
    }
    return s;
	};
};

Vec3f m2v(Matrix m);
Matrix v2m(Vec3f v);
inline glm::vec4 embed4(Vec3f v){
    return glm::vec4(v[0],v[1],v[2],1.0);
};
glm::mat4 viewport(int x, int y, int w, int h,int depth);
glm::mat4 lookat(Vec3f eye, Vec3f center, Vec3f up);
glm::mat4 projection(float coeff); 
#endif 
