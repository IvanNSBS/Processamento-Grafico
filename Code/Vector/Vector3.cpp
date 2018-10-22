#pragma once

#include <math.h>
#include <cstdlib> 
#include <cstdio> 
#include <iostream> 
#include <iomanip>
using namespace std;

template<typename K> 
class Vector3 
{ 
public: 
    // 3 most basic ways of initializing a vector
    K x, y, z; 
    Vector3() : x(K(0)), y(K(0)), z(K(0)) {} 
    Vector3(const K &xx) : x(xx), y(xx), z(xx) {} 
    Vector3(K xx, K yy, K zz) : x(xx), y(yy), z(zz) {} 

    K Length() { return sqrt(x * x + y * y + z * z); } 

    Vector3<K>& Normalize() 
    { 
        K len = Length(); 
        if (len > 0) { 
            K invLen = 1 / len; 
            x *= invLen, y *= invLen, z *= invLen; 
        } 
        return *this; 
    } 

    Vector3<K>& GetNormal()
    {
        Vector3<K>* ret;
        K len = Length();
        if(len < 0){
            K invLen = 1 / len;
            ret = new Vector3(x * invLen, y * invLen, z * invLen );
            return *ret;
        }
        return *ret;
    }

    K DotProduct(const Vector3<K> &v)
    {
        return (x* v.x + y*v.y + z*v.z);
    }

    Vector3<K> CrossProduct(const Vector3<K> &v) const 
    { 
        return Vector3<K>( 
            y * v.z - z * v.y, 
            z * v.x - x * v.z, 
            x * v.y - y * v.x); 
    } 

    Vector3<K> operator + (const Vector3<K> &v) const 
    { return Vector3<K>(x + v.x, y + v.y, z + v.z); } 

    Vector3<K> operator - (const Vector3<K> &v) const 
    { return Vector3<K>(x - v.x, y - v.y, z - v.z); } 
    
    Vector3<K> operator * (const K &r) const 
    { return Vector3<K>(x * r, y * r, z * r); } 

    Vector3<K> operator * (const Vector3<K> &v) const 
    { return Vector3<K>(x * v.x, y * v.y, z * v.z); } 

    Vector3<K>& operator += (const Vector3<K> &v) 
    { x += v.x, y += v.y, z += v.z; return *this; }

    const K& operator [] (uint8_t i) const { return (&x)[i]; } 
    K& operator [] (uint8_t i) { return (&x)[i]; }  
    friend std::ostream& operator << (ostream& os, const Vector3<K>& vt)  
    {  
        os << "( " << vt.x << " , " << vt.y << " , " << vt.z << " )" << endl;  
        return os;  
    }
}; 

typedef Vector3<float> Vector3f;
typedef Vector3<double> Vector3d;
