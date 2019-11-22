#include "Vector3.cpp"

template<typename T> 
class Matrix44 
{ 
public: 
    // initialize the coefficients of the matrix with the coefficients of the identity matrix
    T m[4][4] = {{1,0,0,0},{0,1,0,0},{0,0,1,0},{0,0,0,1}}; 

    Matrix44() {} 

    Matrix44 (T a, T b, T c, T d, 
              T e, T f, T g, T h, 
              T i, T j, T k, T l, 
              T m, T n, T o, T p) 
    { 
        m[0][0] = a; m[0][1] = b; m[0][2] = c; m[0][3] = d; 
        m[1][0] = e; m[1][1] = f; m[1][2] = g; m[1][3] = h; 
        m[2][0] = i; m[2][1] = j; m[2][2] = k; m[2][3] = l; 
        m[3][0] = m; m[3][1] = n; m[3][2] = o; m[3][3] = p; 
    }

    const T* operator [] (uint8_t i) const { return m[i]; } 
    T* operator [] (uint8_t i) { return m[i]; } 

    Matrix44 operator * (const Matrix44 &rhs) const 
    { 
        Matrix44 mult; 
        for (uint8_t i = 0; i < 4; ++i) 
        { 
            for (uint8_t j = 0; j < 4; ++j) 
            { 
                mult[i][j] = m[i][0] * rhs[0][j] + 
                             m[i][1] * rhs[1][j] + 
                             m[i][2] * rhs[2][j] + 
                             m[i][3] * rhs[3][j]; 
            } 
        } 
        return mult; 
    }

    void mult_point_matrix(const Vector3<T> &src, Vector3<T> &dst) const 
    { 
        T a, b, c, w; 

        //printf("m[0][1]: %f\n", m[0][2]);
        a = (src[0] * m[0][0]) + (src[1] * m[1][0]) + (src[2] * m[2][0]) + m[3][0]; 
        b = (src[0] * m[0][1]) + (src[1] * m[1][1]) + (src[2] * m[2][1]) + m[3][1]; 
        c = (src[0] * m[0][2]) + (src[1] * m[1][2]) + (src[2] * m[2][2]) + m[3][2]; 
        w = (src[0] * m[0][3]) + (src[1] * m[1][3]) + (src[2] * m[2][3]) + m[3][3]; 
 
        dst.x = a / w;
        dst.y = b / w; 
        dst.z = c / w; 
        //printf("(%f, %f, %f)\n", dst.x, dst.y, dst.z);
    } 

    void mult_vec_matrix(const Vector3<T> &src, Vector3<T> &dst) const 
    { 
        T a, b, c; 
 
        a = src[0] * m[0][0] + src[1] * m[1][0] + src[2] * m[2][0]; 
        b = src[0] * m[0][1] + src[1] * m[1][1] + src[2] * m[2][1]; 
        c = src[0] * m[0][2] + src[1] * m[1][2] + src[2] * m[2][2]; 
 
        dst.x = a; 
        dst.y = b; 
        dst.z = c; 
    }

    Matrix44 inverse() 
    { 
        int i, j, k; 
        Matrix44<T> s; 
        Matrix44<T> t (*this); 
 
        // Forward elimination
        for (i = 0; i < 3 ; i++) { 
            int pivot = i; 
 
            T pivotsize = t[i][i]; 
 
            if (pivotsize < 0) 
                pivotsize = -pivotsize; 
 
                for (j = i + 1; j < 4; j++) { 
                    T tmp = t[j][i]; 
 
                    if (tmp < 0) 
                        tmp = -tmp; 
 
                        if (tmp > pivotsize) { 
                            pivot = j; 
                            pivotsize = tmp; 
                        } 
                } 
 
            if (pivotsize == 0) { 
                // Cannot invert singular matrix
                return Matrix44(); 
            } 
 
            if (pivot != i) { 
                for (j = 0; j < 4; j++) { 
                    T tmp; 
 
                    tmp = t[i][j]; 
                    t[i][j] = t[pivot][j]; 
                    t[pivot][j] = tmp; 
 
                    tmp = s[i][j]; 
                    s[i][j] = s[pivot][j]; 
                    s[pivot][j] = tmp; 
                } 
            } 
 
            for (j = i + 1; j < 4; j++) { 
                T f = t[j][i] / t[i][i]; 
 
                for (k = 0; k < 4; k++) { 
                    t[j][k] -= f * t[i][k]; 
                    s[j][k] -= f * s[i][k]; 
                } 
            } 
        } 
 
        // Backward substitution
        for (i = 3; i >= 0; --i) { 
            T f; 
 
            if ((f = t[i][i]) == 0) { 
                // Cannot invert singular matrix
                return Matrix44<T>(); 
            } 
 
            for (j = 0; j < 4; j++) { 
                t[i][j] /= f; 
                s[i][j] /= f; 
            } 
 
            for (j = 0; j < i; j++) { 
                f = t[j][i]; 
 
                for (k = 0; k < 4; k++) { 
                    t[j][k] -= f * t[i][k]; 
                    s[j][k] -= f * s[i][k]; 
                } 
            } 
        } 
 
        return s; 
    } 
 
    // \brief set current matrix to its inverse
    const Matrix44<T>& invert() 
    { 
        *this = inverse(); 
        return *this; 
    } 
}; 

typedef Matrix44<float> Matrix44f;
typedef Matrix44<double> Matrix44d;