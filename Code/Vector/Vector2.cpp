#include <iostream>
#include <math.h>
using namespace std;

template<typename K> 
class Vector2 
{ 
public: 
    // 3 most basic ways of initializing a vector
    K x, y; 
    Vector2() : x(K(0)), y(K(0)) {} 
    Vector2(const K &xx) : x(xx), y(xx) {} 
    Vector2(K xx, K yy, K zz) : x(xx), y(yy) {} 

    K Length() { return sqrt((x * x) + (y * y)); } 

    Vector2<K>& Normalize() 
    { 
        K len = Length(); 
        if (len > 0) { 
            K invLen = 1 / len; 
            x *= invLen, y *= invLen; 
        } 
        return *this; 
    } 

    Vector2<K>& GetNormal()
    {
        Vector2<K>* ret;
        K len = Length();
        if(len < 0){
            K invLen = 1 / len;
            ret = new Vector2(x * invLen, y * invLen );
            return *ret;
        }
        return *ret;
    }

    K DotProduct(const Vector2<K> &v)
    {
        return (x* v.x + y*v.y);
    }

    Vector2<K> operator + (const Vector2<K> &v) const 
    { return Vector2<K>(x + v.x, y + v.y); } 

    Vector2<K> operator - (const Vector2<K> &v) const 
    { return Vector2<K>(x - v.x, y - v.y); } 
    
    Vector2<K> operator * (const K &r) const 
    { return Vector2<K>(x * r, y * r); } 

    friend std::ostream& operator<<(ostream& os, const Vector2<K>& vt)  
    {  
        os << "( " << vt.x << " , " << vt.y << " )" << endl;  
        return os;  
    } 
}; 

typedef Vector2<float> Vector2f;