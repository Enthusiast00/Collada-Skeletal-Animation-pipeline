#ifndef RT_MATH_H
#define RT_MATH_H
#include "math.h"

template < typename T >
struct rt_vec2{
    T x,y;
    rt_vec2():x(T(0)),y(T(0)){}
    rt_vec2(const rt_vec2&X){ x=X.x,y=X.y; }
    rt_vec2(const T& X,const T& Y):x(X),y(Y){}
    void operator = (const rt_vec2&X){ x=X.x,y=X.y; }
    T & operator [] (const int& i) { return (&x)[i]; }
};
template < typename T >
struct rt_vec3{
    T x,y,z;
    rt_vec3():x(0),y(0),z(0){}
    rt_vec3(const rt_vec3&X){ x=X.x,y=X.y,z=X.z; }
    rt_vec3(const T& X,const T& Y,const T& Z):x(X),y(Y),z(Z){}
    void Nagate() {x=-x;y=-y;z=-z;}
    T Magnitude()const { return T( sqrt(x*x+y*y+z*z) ); }
    void operator = (const rt_vec3&X){ x=X.x,y=X.y,z=X.z; }
    T & operator [] (const int& i){ return (&x)[i]; }
    rt_vec3 operator * (const T& s){ return rt_vec3(x*s,y*s,z*s); }
};
template < typename T >
struct rt_vec4{
    T x,y,z,w;
    rt_vec4():x(0),y(0),z(0),w(0){}
    rt_vec4(const rt_vec4&X){ x=X.x,y=X.y,z=X.z,w=X.w; }
    rt_vec4(const T& X,const T& Y,const T& Z,const T& W):x(X),y(Y),z(Z),w(W){}
    void operator = (const rt_vec4&X){ x=X.x,y=X.y,z=X.z,w=X.w; }
    T & operator [] (const int& i){ return (&x)[i]; }
};
template < typename T >
rt_vec3<T> operator + (const rt_vec3<T> & v1,const rt_vec3<T> & v2){ return rt_vec3<T>(v1.x+v2.x,v1.y+v2.y,v1.z+v2.z); }
template < typename T >
rt_vec3<T> operator - (const rt_vec3<T> & v1,const rt_vec3<T> & v2){ return rt_vec3<T>(v1.x-v2.x,v1.y-v2.y,v1.z-v2.z); }

template < typename T >
T  rtm_distance(const rt_vec3<T>& v1,const rt_vec3<T>& v2){ return ( v1 - v2 ).Magnitude(); }
template < typename T >
T  rtm_dot(const rt_vec3<T>& v1,const rt_vec3<T>& v2){ return v1.x*v2.x+v1.y*v2.y+v1.z*v2.z; }
template < typename T >
rt_vec3<T>  rtm_cross(const rt_vec3<T>& v1,const rt_vec3<T>& v2){
    return rt_vec3<T>( v1.y*v2.z - v1.z*v2.y, v1.z*v2.x - v1.x*v2.z, v1.x*v2.y - v1.y*v2.x );
}
template < typename T >
rt_vec3<T> rtm_normalize(const rt_vec3<T>& v){
    T mag = v.Magnitude();
    if(mag != T(0) ) { return rt_vec3<T>(v.x/mag,v.y/mag,v.z/mag); }
    return v;
}
template < typename T >
struct rt_mat4 {
    ~rt_mat4(){}
    rt_vec4<T> col[4];
    rt_mat4(){ col[0][0]=col[1][1]=col[2][2]=col[3][3]=T(1); }
    rt_mat4(const T& s){ col[0][0]=col[1][1]=col[2][2]=col[3][3]=s; }
    void operator = (const rt_mat4&x){ for(int i=0;i<16;((T*)&col)[i]=((T*)&x)[i],i++); }
    rt_vec4<T> & operator [] (const int& i){ return col[i]; }
};
template < typename T >
void rtm_translate( rt_mat4<T>* M,const T& x,const T& y,const T& z){ T* m = (T*)M; m[12]+=x; m[13]+=y; m[14]+=z; }
template < typename T >
inline rt_mat4<T>  rtm_transpose(const rt_mat4<T>& M){
    rt_mat4<T> out_;
    T *in = (T*)&M , *out = (T*)&out_;
    out[0]=in[0]; out[1]=in[4]; out[2]=in[8]; out[3]=in[12];
    out[4]=in[1]; out[5]=in[5]; out[6]=in[9]; out[7]=in[13];
    out[8]=in[2]; out[9]=in[6]; out[10]=in[10]; out[11]=in[14];
    out[12]=in[3]; out[13]=in[7]; out[14]=in[11]; out[15]=in[15];
    return out_;
}
template < typename T >
inline rt_vec4<T> operator * ( rt_mat4<T>& m_,rt_vec4<T> & v_ ){
    rt_vec4<T> out_;
    T * out =  (T*)&out_,* v =  (T*)&v_, *m =(T*)&m_;
    out[0] = m[0]*v[0] + m[1]*v[1] + m[2]*v[2] + m[3]*v[3];
    out[1] = m[4]*v[0] + m[5]*v[1] + m[6]*v[2] + m[7]*v[3];
    out[2] = m[8]*v[0] + m[9]*v[1] + m[10]*v[2] + m[11]*v[3];
    out[3] = m[12]*v[0] + m[13]*v[1] + m[14]*v[2] + m[15]*v[3];
    return out_;
}
template < typename T >
inline rt_mat4<T>  operator * (const rt_mat4<T>& m1_,const rt_mat4<T>& m2_){
    rt_mat4<T> out_;
    T * out =  (T*)&out_, *m1 =(T*)&m1_, *m2 =(T*)&m2_;
    out[0] = m2[0] * m1[0] + m2[1] * m1[4] + m2[2] * m1[8] + m2[3] * m1[12];
    out[4] = m2[4] * m1[0] + m2[5] * m1[4] + m2[6] * m1[8] + m2[7] * m1[12];
    out[8] = m2[8] * m1[0] + m2[9] * m1[4] + m2[10] * m1[8] + m2[11] * m1[12];
    out[12] = m2[12] * m1[0] + m2[13] * m1[4] + m2[14] * m1[8] + m2[15] * m1[12];

    out[1] = m2[0] * m1[1] + m2[1] * m1[5] + m2[2] * m1[9] + m2[3] * m1[13];
    out[5] = m2[4] * m1[1] + m2[5] * m1[5] + m2[6] * m1[9] + m2[7] * m1[13];
    out[9] = m2[8] * m1[1] + m2[9] * m1[5] + m2[10] * m1[9] + m2[11] * m1[13];
    out[13] = m2[12] * m1[1] + m2[13] * m1[5] + m2[14] * m1[9] + m2[15] * m1[13];

    out[2] = m2[0] * m1[2] + m2[1] * m1[6] + m2[2] * m1[10] + m2[3] * m1[14];
    out[6] = m2[4] * m1[2] + m2[5] * m1[6] + m2[6] * m1[10] + m2[7] * m1[14];
    out[10] = m2[8] * m1[2] + m2[9] * m1[6] + m2[10] * m1[10] + m2[11] * m1[14];
    out[14] = m2[12] * m1[2] + m2[13] * m1[6] + m2[14] * m1[10] + m2[15] * m1[14];

    out[3] = m2[0] * m1[3] + m2[1] * m1[7] + m2[2] * m1[11] + m2[3] * m1[15];
    out[7] = m2[4] * m1[3] + m2[5] * m1[7] + m2[6] * m1[11] + m2[7] * m1[15];
    out[11] = m2[8] * m1[3] + m2[9] * m1[7] + m2[10] * m1[11] + m2[11] * m1[15];
    out[15] = m2[12] * m1[3] + m2[13] * m1[7] + m2[14] * m1[11] + m2[15] * m1[15];
    return out_;
}
template <typename T>
rt_mat4<T> rtm_rotate ( const T & angle,const T & x,const T & y,const T & z ) {

    const T a = angle ,c = cos(a) ,s = sin(a);
    rt_vec3<T> axis(x,y,z);
    axis = rtm_normalize(axis);
    rt_vec3<T> temp( axis*(T(1) - c) );

    rt_mat4<T> Rotate;
    Rotate[0][0] = c + temp[0] * axis[0];
    Rotate[0][1] = 0 + temp[0] * axis[1] + s * axis[2];
    Rotate[0][2] = 0 + temp[0] * axis[2] - s * axis[1];

    Rotate[1][0] = 0 + temp[1] * axis[0] - s * axis[2];
    Rotate[1][1] = c + temp[1] * axis[1];
    Rotate[1][2] = 0 + temp[1] * axis[2] + s * axis[0];

    Rotate[2][0] = 0 + temp[2] * axis[0] + s * axis[1];
    Rotate[2][1] = 0 + temp[2] * axis[1] - s * axis[0];
    Rotate[2][2] = c + temp[2] * axis[2];

    return Rotate;
}
template <typename T>
rt_mat4<T> rtm_lookat ( const rt_vec3<T> & eye, const rt_vec3<T> & center, const rt_vec3<T> & up ) {

     const rt_vec3<T> f( rtm_normalize( center - eye)    );
     const rt_vec3<T> s( rtm_normalize( rtm_cross(f, up) ) );
     const rt_vec3<T> u( rtm_cross(s, f) );
    rt_mat4<T> Result(1);
    Result[0][0] = s.x;
    Result[1][0] = s.y;
    Result[2][0] = s.z;
    Result[0][1] = u.x;
    Result[1][1] = u.y;
    Result[2][1] = u.z;
    Result[0][2] =-f.x;
    Result[1][2] =-f.y;
    Result[2][2] =-f.z;
    Result[3][0] =-rtm_dot(s, eye);
    Result[3][1] =-rtm_dot(u, eye);
    Result[3][2] = rtm_dot(f, eye);
    return Result;
}
template <typename T>
rt_mat4<T> rtm_perspective ( const T& fovy, const T& aspect, const T& zNear, const T& zFar ){

    if( (aspect ==T(0)) || (zFar == zNear) ){ fprintf(stderr,"rtm perspective \nline:%i file:%s \n",__LINE__ , __FILE__);}

    T const rad = fovy;
    T const tanHalfFovy = tan(rad / static_cast<T>(2));

    rt_mat4<T> Result;
    Result[0][0] = static_cast<T>(1) / (aspect * tanHalfFovy);
    Result[1][1] = static_cast<T>(1) / (tanHalfFovy);
    Result[2][2] = - (zFar + zNear) / (zFar - zNear);
    Result[2][3] = - static_cast<T>(1);
    Result[3][2] = - (static_cast<T>(2) * zFar * zNear) / (zFar - zNear);
    return Result;
}

#endif // RT_MATH_H
