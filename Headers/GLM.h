#ifndef _GLM_H_
#define _GLM_H_
#include "GLM/glm.hpp"
#include "GLM/gtc/matrix_transform.hpp"
#include "GLM/gtc/type_ptr.hpp"
#include <string.h>
namespace glm {
    typedef float real;
    #include "glm_typedef.h"
    constexpr real PI = 3.14159265358979323846f;
    constexpr real sqrt2 = 1.41421356237309504880f;
    constexpr real sqrt3 = 1.73205080756887729352f;
    constexpr real sqrt5 = 2.23606797749978969640f;

    template <int n, typename T, qualifier Q=qualifier::defaultp>
    GLM_FUNC_QUALIFIER std::string toString(vec<n, T, Q> const& v) {
        std::string s = "(";
        char tmp[10];
        for(int i=0; i<n; i++) {
            sprintf(tmp, "%02.3f", v[i]);
            if(tmp[0] != '-') s += ' ';
            s += tmp;
            if(i<n-1) s += ", ";
        }
        s += ")";
        return s;
    }
    template <int n, int _m, typename T, qualifier Q=qualifier::defaultp>
    GLM_FUNC_QUALIFIER std::string toString(mat<n, _m, T, Q> const& m) {
        std::string s = "{";
        for(int i=0; i<n; i++) {
            s += toString(m[i]);
            if(i<n-1) s += ",\n ";
        }
        s += "}";
        return s;
    }
    template <int n, typename T, qualifier Q=qualifier::defaultp>
    GLM_FUNC_QUALIFIER T lengthSQ(vec<n, T, Q> const& v) {
        return dot(v, v);
    }

	template <typename T, qualifier Q=qualifier::defaultp>
    GLM_FUNC_QUALIFIER mat<4, 4, T, Q> rotateXl(T angle) {
        T const&a = angle;
        T const c = cos(a);
        T const s = sin(a);

        mat<4, 4, T, Q> Result(
            T(1), T(0), T(0), T(0),
            T(0),   c ,  -s , T(0),
            T(0),   s ,   c , T(0),
            T(0), T(0), T(0), T(1)
        );
        return Result;
    }
    template <typename T, qualifier Q=qualifier::defaultp>
    GLM_FUNC_QUALIFIER mat<4, 4, T, Q> rotateXr(T angle) {
        return rotateXr(-angle);
    }
    template <typename T, qualifier Q=qualifier::defaultp>
    GLM_FUNC_QUALIFIER mat<4, 4, T, Q> rotateYl(T angle) {
        T const&a = angle;
        T const c = cos(a);
        T const s = sin(a);

        mat<4, 4, T, Q> Result(
              c , T(0),   s , T(0),
            T(0), T(1), T(0), T(0),
             -s , T(0),   c , T(0),
            T(0), T(0), T(0), T(1)
        );
        return Result;
    }
    template <typename T, qualifier Q=qualifier::defaultp>
    GLM_FUNC_QUALIFIER mat<4, 4, T, Q> rotateYr(T angle) {
        return rotateYr(-angle);
    }
    template <typename T, qualifier Q=qualifier::defaultp>
    GLM_FUNC_QUALIFIER mat<4, 4, T, Q> rotateZl(T angle) {
        T const&a = angle;
        T const c = cos(a);
        T const s = sin(a);

        mat<4, 4, T, Q> Result(
              c ,  -s , T(0), T(0),
              s ,   c , T(0), T(0),
            T(0), T(0), T(1), T(0),
            T(0), T(0), T(0), T(1)
        );
        return Result;
    }
    template <typename T, qualifier Q=qualifier::defaultp>
    GLM_FUNC_QUALIFIER mat<4, 4, T, Q> rotateZr(T angle) {
        return rotateZr(-angle);
    }/*
    template <typename T, qualifier Q=qualifier::defaultp>
    GLM_FUNC_QUALIFIER mat<4, 4, T, Q> rotateR(T angle, vec<3, T, Q> axis) {
        T const&a = angle;
        T const c = cos(a);
        T const s = sin(a);
        T const omc = T(1) - c;

        vec<3, T, Q> a2 = axis * axis;

        mat<4, 4, T, Q> Result(
            a2.x + c * (T(1) - a2.x), axis.x * axis.y * omc + axis.z * s, axis.x * axis.z * omc - axis.y * s, T(0),
            axis.x * axis.y * omc - axis.z * s, a2.y + c * (T(1) - a2.y), axis.y * axis.z * omc + axis.x * s, T(0),
            axis.x * axis.z * omc + axis.y * s, axis.y * axis.z * omc - axis.x * s, a2.z + c * (T(1) - a2.z), T(0),
            T(0), T(0), T(0), T(1)
        );
        return Result;
    }
    template <typename T, qualifier Q=qualifier::defaultp>
    GLM_FUNC_QUALIFIER mat<4, 4, T, Q> rotateL(T angle, vec<3, T, Q> axis) {
        return rotateL(-angle, axis);
    }*/
    template <typename T, qualifier Q=qualifier::defaultp>
    GLM_FUNC_QUALIFIER mat<4, 4, T, Q> rotateXYZr(vec<3, T, Q> const& angles) {
        T const&a = angles.x;
        T const b = angles.y;
        T const c = angles.z;
        T const cosA = cos(a);
        T const sinA = sin(a);
        T const cosB = cos(b);
        T const sinB = sin(b);
        T const cosC = cos(c);
        T const sinC = sin(c);

        mat<4, 4, T, Q> Result(
            cosB * cosC, cosB * sinC, -sinB, T(0),
            sinA * sinB * cosC - cosA * sinC, sinA * sinB * sinC + cosA * cosC, sinA * cosB, T(0),
            cosA * sinB * cosC + sinA * sinC, cosA * sinB * sinC - sinA * cosC, cosA * cosB, T(0),
            T(0), T(0), T(0), T(1)
        );
        return Result;
    }
    template <typename T, qualifier Q=qualifier::defaultp>
    GLM_FUNC_QUALIFIER mat<4, 4, T, Q> rotateXYZl(vec<3, T, Q> const& angles) {
        return rotateXYZr(-angles);
    }
    template <typename T, qualifier Q=qualifier::defaultp>
    GLM_FUNC_QUALIFIER mat<4, 4, T, Q> rotateYXZr(T x, T y, T z) {
        return rotateXYZr(vec<3, T, Q>(y, x, z));
    }
    template <typename T, qualifier Q=qualifier::defaultp>
    GLM_FUNC_QUALIFIER mat<4, 4, T, Q> rotateYXZl(T x, T y, T z) {
        return rotateXYZl(vec<3, T, Q>(y, x, z));
    }
    template <typename T, qualifier Q=qualifier::defaultp>
    GLM_FUNC_QUALIFIER mat<4, 4, T, Q> scale(vec<3, T, Q> const& v) {
        mat<4, 4, T, Q> Result(T(1));
        Result[0][0] = v.x;
        Result[1][1] = v.y;
        Result[2][2] = v.z;
        return Result;
    }
    template <typename T, qualifier Q=qualifier::defaultp>
    GLM_FUNC_QUALIFIER mat<4, 4, T, Q> scale(T const& s) {
        return scale(vec<3, T, Q>(s));
    }
    template <typename T, qualifier Q=qualifier::defaultp>
    GLM_FUNC_QUALIFIER mat<4, 4, T, Q> translate(vec<3, T, Q> const& v) {
        mat<4, 4, T, Q> Result(T(1));
        Result[3] = vec<4, T, Q>(v, T(1));
        return Result;
    }
    template <typename T, qualifier Q=qualifier::defaultp>
    GLM_FUNC_QUALIFIER mat<4, 4, T, Q> translate(T const& x, T const& y, T const& z) {
        return translate(vec<3, T, Q>(x, y, z));
    }/*
    template <typename T, qualifier Q=qualifier::defaultp>
    GLM_FUNC_QUALIFIER mat<4, 4, T, Q> lookAt(vec<3, T, Q> const& eye, vec<3, T, Q> const& center, vec<3, T, Q> const& up) {
        vec<3, T, Q> f = normalize(center - eye);
        vec<3, T, Q> u = normalize(up);
        vec<3, T, Q> s = normalize(cross(f, u));
        u = cross(s, f);

        mat<4, 4, T, Q> Result(1);
        Result[0][0] = s.x;
        Result[1][0] = s.y;
        Result[2][0] = s.z;
        Result[0][1] = u.x;
        Result[1][1] = u.y;
        Result[2][1] = u.z;
        Result[0][2] =-f.x;
        Result[1][2] =-f.y;
        Result[2][2] =-f.z;
        Result[3][0] =-dot(s, eye);
        Result[3][1] =-dot(u, eye);
        Result[3][2] = dot(f, eye);
        return Result;
    }//*/
}
#endif // _GLM_H_