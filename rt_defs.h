#ifndef RT_DEFS_H
#define RT_DEFS_H

#include "stdio.h"
#include "stdint.h"
#include "stdlib.h"
#include "string.h"

#include "Windows.h"

#include "d3d9.h"
#include "d3dx9.h"
#include "rt_math.h"

static HINSTANCE g_instance = NULL;

#define RT_ZERO(x) { memset((void*)&x,0,sizeof(x));}

#define RTW_TITLE "-rabbit-"
#define RTW_CLASS_NAME "-rabbit-class-"

#define RT_DEFAULT_X 100
#define RT_DEFAULT_Y 100
#define RT_DEFAULT_WIDTH 600
#define RT_DEFAULT_HEIGHT 400

#define RT_INST rabbit::s_instance
#define RT_WIN  rt_window::s_window
#define RT_D3D  rt_d3dmanager::s_d3dmanager
#define RT_EVENT rabbit::s_msg
#define RT_TIMER rt_timer::s_timer
#define RT_INFO  rabbit::s_info

#define RT_ERROR(X) fprintf(stderr,"Error: %s \nline:%i file:%s \n",X,__LINE__ , __FILE__)
#define RT_THROW(X) { RT_ERROR(X); return false; }

#define RT_SCMP(X,Y) (strcmp(X,Y)==0)

#define RT_HR_T(x) { if(FAILED(x)) {RT_THROW("hr"); } }
#define RT_HR_E(x) { if(FAILED(x)) {RT_ERROR("hr"); } }
#define RELEASECOM(x) { if(x){ x->Release();x = 0; } }

#define RT_NEW(X,Y) ( (X*)calloc(Y, sizeof(X)) )
#define RT_ALLOC(X,Y,Z) { if(X){ delete X; X=NULL; } X=RT_NEW(Z,Y); if(!X){ RT_THROW("Bad Alloc"); } }
#define RT_SWAP(X,Y) { T c=X; X=Y;Y=c; }


typedef rt_vec2<float> rt_vector2;
typedef rt_vec3<float> rt_vector3;
typedef rt_vec4<float> rt_vector4;
typedef rt_vec4<uint8_t> rt_bvector4;
typedef rt_mat4<float> rt_matrix4;

template<typename T ,typename T1 = uint32_t >
class rt_array {
    T1  m_pos,m_size;
    T * m_data;
public:
    rt_array():m_pos(0),m_size(0),m_data(NULL){}
    ~rt_array(){ Clear(); }
    rt_array(const rt_array& x): m_pos(0),m_size(0),m_data(NULL) { Copy(x); }
    rt_array(const T& val): m_pos(0),m_size(0),m_data(NULL) { PushBack(val); }
    rt_array(const T* val): m_pos(0),m_size(0),m_data(NULL) { operator =(val); }
    void operator = (const rt_array& x) { Copy(x); }
    void operator = (const T& val) { Clear(); PushBack(val); }
    void operator =(const T* val){
        if(val){
            Clear();
            T1 len = strlen(val);
            Allocate(len+1);
            m_pos = len;
            for(T1 i(0); i<len; m_data[i] = val[i], i++);
        }
    }
    void operator += (const T* val){
        if(val){
            T1 len = strlen(val);
            if( !Alloc(m_pos+len+1,false,false) ){ RT_ERROR("Alloc"); }
            for(T1 i(0); i<len;m_data[m_pos+i] = val[i],i++);
            m_pos+=len;
        }
    }
    void operator += (const T& val) {  PushBack(val); }
    void operator += (const rt_array&  val) { operator+=(val.Data()); }
    rt_array operator + (const T& val)const { rt_array out(m_data); out += val; return out; }
    rt_array operator + (const T* val)const { rt_array out(m_data); out += val; return out; }
    rt_array operator + (const rt_array& val)const  { return operator+(val.m_data); }
    T& operator [](const T1& index){ return m_data[index]; }
    const T& operator [](const T1& index) const { return m_data[index]; }
    void Copy (const rt_array& x){
        Clear();
        Allocate(x.m_size);
        m_size = x.m_size; m_pos  = x.m_pos;
        for(T1 i=0; i<m_size; m_data[i] = x.m_data[i], i++);
    }
    void Allocate(const T1& size ){ if( !Alloc(size,true,false) ){ RT_ERROR("Alloc"); } }
    void PushBack(const T& val, bool buffered = false){
        if( Alloc(m_pos+2,false,buffered) ){
            m_data[m_pos++] = val;
        }else{ RT_ERROR("Alloc"); }
    }
    void Clear(){
        if(m_data){ delete m_data; }
        m_data = NULL;
        m_size = m_pos = 0;
    }
public:
    const T* Data() const { return m_data; }
    const T1& Size() const { return m_pos; }
protected:
    bool Alloc(const T1& size ,bool pos ,bool buffered){
        if(size<=m_size){ return true; }
        T1 asize( size + T1(pos) );
        if(!m_data || !m_pos){ RT_ALLOC(m_data,asize,T); }
        else{
            asize = buffered? m_size*2 : asize;
            T* buf = NULL;
            RT_ALLOC(buf,asize,T);
            for(T1 i =0; i<m_pos; buf[i] = m_data[i], i++);
            delete m_data;
            m_data = buf;
        }
        if(pos){ m_pos = size; }
        m_size = asize;
        return true;
    }
};

typedef rt_array<char,uint32_t> rt_string;
typedef rt_array<char,uint16_t> rt_string16;
typedef rt_array<rt_string> rt_sarray;
typedef rt_array<int32_t> rt_iarray;
typedef rt_array<float> rt_farray;

typedef rt_array<rt_matrix4,uint16_t> rt_tarray;
typedef rt_array< rt_tarray ,uint16_t> rt_aarray;


inline void zerostrip(char * str){
    int8_t des(0), i(0), len = int8_t(strlen(str));

    while((!des)&&(i<len)){  if(str[i++]=='.') { des=1; }  }
    if(des){
        int8_t stp(0), start = len;
        while ( (!stp) && (i<= start) ) {
            if(str[start]==0){ start--; }
            else{
                if(str[start--]=='0'){ str[start+1]=0; }
                else  stp=1;
            }
        }
        if( (stp==0) && (i>=start) ){ str[i-1]=0; }
    }
}
#define NUMTOSTR_BUFF 25

inline rt_string SubStr(const rt_string& str ,uint32_t st,uint32_t en) {
    if( (st>=en) || (st>str.Size()) || (en>str.Size()) ) { return rt_string(""); }
    rt_string rt;
    rt.Allocate(en-st);
    for(uint32_t ii=0, i=st; i<en; ii++,i++) {  rt[ii]=str[i]; }
    return rt;
}
inline rt_string16 L2Str(const  long& val) {
    if(val==0) { return "0"; }
    char * str = new char[NUMTOSTR_BUFF];
    for(uint8_t i=0; i<NUMTOSTR_BUFF; str[i++]=0);
    sprintf_s(str,NUMTOSTR_BUFF,"%ld",val);
    rt_string16 rt(str);
    delete [] str;
    return rt;
}
inline rt_string16 F2Str(const float& val) {
    if(val==0) { return "0"; }
    char * str = new char[NUMTOSTR_BUFF];
    for(uint8_t i=0; i<NUMTOSTR_BUFF; str[i++]=0);
    sprintf_s(str,NUMTOSTR_BUFF, "%f",val);
    zerostrip(str);
    rt_string16 rt(str);
    delete [] str;
    return rt;
}
static void StrSplit(const rt_string& str, rt_sarray * out ,char sp=' '){
    rt_string buf;
    for(uint32_t i=0; i< str.Size(); i++){
        if(str[i]==sp){
            if(buf.Size()){ out->PushBack(buf,true); buf.Clear(); }
        }else{ buf.PushBack(str[i],true); }
    }
    if(buf.Size()){ out->PushBack(buf); }
}
static void S2Iarray(rt_sarray & sarray,rt_iarray * out){
    for(uint32_t i=0; i<sarray.Size();i++){ out->PushBack( atoi( sarray[i].Data() ) ,true); }
}
static void S2Farray(rt_sarray & sarray,rt_farray * out){
    for(uint32_t i=0; i<sarray.Size();i++){ out->PushBack( float(atof( sarray[i].Data() )) ,true); }
}

static void Text2Iarray(const char* str,rt_iarray * out){
    rt_sarray strs; StrSplit(str,&strs); S2Iarray(strs,out);
}
static void Text2Farray(const char* str,rt_farray * out){
    rt_sarray strs; StrSplit(str,&strs); S2Farray(strs,out);
}

static inline float lerp( const float& from,const float& too,float lerp_tm) {
    return ((too*lerp_tm) + ((1.0f-lerp_tm)*from));
}
class rabbit;
class rt_window;
class rt_d3dmanager;
class rt_mesh;

struct rt_material {
    rt_material():specPower(0.0f){}
    rt_material(const rt_material& x) { ambient=x.ambient;diffuse=x.diffuse;specular=x.specular;specPower=x.specPower; }
    void operator = (const rt_material& x) { ambient=x.ambient;diffuse=x.diffuse;specular=x.specular;specPower=x.specPower; }
    rt_vector3 ambient;
    rt_vector3 diffuse;
    rt_vector3 specular;
    float specPower;
};
struct rt_dirlight {
    rt_dirlight(){}
    rt_dirlight(const rt_dirlight& x){ ambient=x.ambient;diffuse=x.diffuse;specular=x.specular;dir=x.dir; }
    void operator = (const rt_dirlight& x){ ambient=x.ambient;diffuse=x.diffuse;specular=x.specular;dir=x.dir; }
    rt_vector3 ambient;
    rt_vector3 diffuse;
    rt_vector3 specular;
    rt_vector3 dir;
};

struct rt_vertex {
    rt_vertex(){}
    rt_vertex(const rt_vertex&x) { pos=x.pos;norm=x.norm;uv=x.uv;bw=x.bw;bi=x.bi; }
    void operator = (const rt_vertex&x) { pos=x.pos;norm=x.norm;uv=x.uv;bw=x.bw;bi=x.bi; }
    rt_vector3 pos;
    rt_vector3 norm;
    rt_vector2 uv;
    rt_vector4 bw;
    rt_bvector4 bi;
};
struct rt_vertex_s {
    rt_vertex_s(){}
    rt_vertex_s(const rt_vertex_s&x) { pos=x.pos;norm=x.norm;uv=x.uv;bw=x.bw;bi=x.bi; }
    void operator = (const rt_vertex_s&x) { pos=x.pos;norm=x.norm;uv=x.uv;bw=x.bw;bi=x.bi; }
    rt_vector3 pos;
    rt_vector3 norm;
    rt_vector2 uv;
    float bw;
    uint8_t bi;
};
struct rt_vertex_ {
    rt_vertex_(){}
    rt_vertex_(const rt_vertex_&x) { pos=x.pos;norm=x.norm;uv=x.uv; }
    void operator = (const rt_vertex_&x) { pos=x.pos;norm=x.norm;uv=x.uv; }
    rt_vector3 pos;
    rt_vector3 norm;
    rt_vector2 uv;
};

struct rt_vertex_line {
    rt_vertex_line(){}
    rt_vertex_line(const rt_vertex_line&x) { pos=x.pos;col=x.col; }
    void operator = (const rt_vertex_line&x) { pos=x.pos;col=x.col; }
    rt_vector3 pos;
    rt_vector3 col;
};


#define RABBIT_INIT 0x1
#define RABBIT_RUNNING 0x2
#define RABBIT_PAUSED 0x4
#define RABBIT_HUD 0x8

#define RT_WIN_MINORMAXED 0x1
#define RT_WIN_LSHIFT 0x2
#define RT_WIN_RSHIFT 0x4
#define RT_WIN_LCTRL 0x8
#define RT_WIN_RCTRL 0x10
#define RT_WIN_LBUTTOND 0x20
#define RT_WIN_RBUTTOND 0x40
#define RT_WIN_LEAVE 0x80

#define RT_D3D_DEVICE_LOST 0x1

#define RT_MESH_INIT 0x1
#define RT_MESH_DISABLE 0x2
#define RT_MESH_PAUSE 0x4
#define RT_MESH_HGRID 0x8
#define RT_MESH_VGRID 0x10

class flag_c{
    uint32_t m_flags;
public:
    flag_c() :m_flags(0) {}
    void Clear() { m_flags = 0; }
    bool FlagTest(const uint32_t f) { return (f&m_flags)!=0; }
    void AddFlags(const uint32_t f) { m_flags|=f; }
    void RemoveFlags(const uint32_t f) { m_flags&= ~f; }
};
class rt_object : public flag_c{
public:
    virtual bool Init() = 0;
    virtual void Update() = 0;
    virtual void Delete() = 0;
    virtual void Enable() { RemoveFlags(RT_MESH_DISABLE); }
    virtual void Disable() { AddFlags(RT_MESH_DISABLE); }
};
struct sbmesh {sbmesh():ib(NULL){} IDirect3DIndexBuffer9* ib; rt_material mat; rt_array<uint16_t> indices; };
struct GMSG { UINT msg; WPARAM wparam; LPARAM lparam; bool isevent; };

static uint64_t StartClock() {
    uint64_t currtick = 0;
    QueryPerformanceCounter((LARGE_INTEGER*)&currtick);
    return currtick;
}
static uint64_t EndClock(const uint64_t& st) {
    uint64_t currtick = 0;
    QueryPerformanceCounter((LARGE_INTEGER*)&currtick);
    return currtick-st;
}
#endif // RT_DEFS_H
