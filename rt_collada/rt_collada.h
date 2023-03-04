#ifndef RT_COLLADA_H
#define RT_COLLADA_H
#include "rt_defs.h"

struct Param {
    Param(){}
    Param(const char* n, const char* t):name(n),type(t){}
    Param(const Param& x){name=x.name; type=x.type;}
    void operator = (const Param& x){name=x.name;type=x.type;}
    rt_string16 name,type;
};
struct Source {
    Source():acccount(0), accstride(0){}
    Source(const Source&x) { Copy(x); }
    void operator = (const Source&x) { Copy(x); }
    void Copy( const Source&x){ id=x.id;farray=x.farray;narray=x.narray; acccount=x.acccount;accstride=x.accstride; params=x.params; }
    rt_string16 id;
    rt_farray farray;
    rt_sarray narray;
    uint32_t acccount, accstride;
    rt_array<Param,uint16_t> params;
};
struct Input {
    Input():offset(0){}
    Input(const Input& x) {semantic=x.semantic;source=x.source;offset=x.offset;}
    void operator = (const Input& x) {semantic=x.semantic;source=x.source;offset=x.offset;}
    rt_string16 semantic,source;
    uint16_t offset;
};
struct Polylist{
    Polylist(){}
    Polylist(const Polylist&x ){inputs=x.inputs;vcount=x.vcount;p=x.p;}
    void operator = (const Polylist &x ){inputs=x.inputs;vcount=x.vcount;p=x.p;}
    rt_array<Input,uint16_t> inputs;
    rt_iarray vcount;
    rt_iarray p;
};
struct Controller {
    Controller():vwcount(0){}
    Controller(const Controller & x){jsrc=x.jsrc;wsrc=x.wsrc;ibsrc=x.ibsrc;vcount=x.vcount;v=x.v; vwcount=x.vwcount;}
    void operator = (const Controller & x){jsrc=x.jsrc;wsrc=x.wsrc;ibsrc=x.ibsrc;vcount=x.vcount;v=x.v; vwcount=x.vwcount;}
    rt_string16 jsrc,wsrc,ibsrc;
    rt_iarray vcount;
    rt_iarray v;
    uint32_t vwcount;
};
struct rt_node{
    rt_node():ccnt(0),c(NULL){ }
    uint32_t ccnt;
    rt_node **c;
    rt_string16 name;
    rt_farray transform;
    rt_node* AddChild(rt_node * n){
        if(!n){ return NULL; }
        rt_node** cld = new rt_node*[ccnt+1];
        if(c){ for(uint32_t i=0;i<ccnt;cld[i]=c[i],i++); delete c;}
        c = cld;
        c[ccnt++]=n;
        return n;
    }
    static void DeleteNode(rt_node* n){
        if(!n){ return; }
        for(uint32_t i =0;i<n->ccnt;i++){ DeleteNode(n->c[i]); }
        delete n;

    }
};
struct anim_sampler{
    anim_sampler(){}
    anim_sampler(const anim_sampler&x){isrc=x.isrc;osrc=x.osrc;}
    void operator=(const anim_sampler&x){isrc=x.isrc;osrc=x.osrc;}
    rt_string16 isrc,osrc;
};
struct rtc_mesh {
    rtc_mesh():node_root(NULL){}
    rt_array< rt_vertex > vertices;
    rt_array< rt_array<uint16_t> , uint16_t > indices;
    rt_array< rt_matrix4, uint16_t> bm;
    rt_array< rt_matrix4, uint16_t> ibm;
    rt_matrix4 bsm;
    rt_sarray jntn;
    rt_node * node_root;
    rt_aarray anims;

    rt_tarray finaljointtransforms;
    rt_aarray finalanimstransforms;

    rt_farray a_inputs;
    rt_string16 texture;

    void ReverseIndices();
    void GenFinalTransforms();
    void Index();
private:
    void GenFinalTransforms(bool isanim,rt_node * node,rt_tarray* destbones,rt_matrix4 parent=rt_matrix4(1));

};
inline static rt_matrix4 ToMat(float * matf){
    rt_matrix4 mat;
    float * Mat = (float*) &mat;
    for(uint16_t i =0;i<16;i++){ Mat[i]=matf[i]; }
    mat=rtm_transpose(mat);
    return mat;
}
namespace tinyxml2 { class XMLElement; }
class rt_collada{
    rt_string16 m_vertsrc;
    rt_array<Source,uint16_t> m_sources;
    rt_array<Polylist,uint16_t> m_plists;
    Controller m_controller;
    rt_array<anim_sampler, uint16_t> m_anims;
public:
    rt_collada(){ }
    ~rt_collada(){ if(m_mesh.node_root){ rt_node::DeleteNode(m_mesh.node_root); } }
    rtc_mesh m_mesh;
    bool Load(const char * file);
private:
    bool LoadData();
    bool ReadGeometry(tinyxml2::XMLElement * element);
    void ReadController(tinyxml2::XMLElement * element);
    void ReadVisualScene(tinyxml2::XMLElement * element);
    void ReadNode(tinyxml2::XMLElement * element,rt_node*parent);
    void ReadAnimations(tinyxml2::XMLElement * element);
};

#endif // RT_COLLADA_H
