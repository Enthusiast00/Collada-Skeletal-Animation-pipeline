#ifndef RT_MESH_H
#define RT_MESH_H

#include "rt_defs.h"

class rt_meshdata {
protected:
    IDirect3DTexture9* m_tex;
    IDirect3DVertexBuffer9* m_vb;
    rt_array<rt_vertex> m_vertices;
    rt_array<sbmesh,uint16_t> m_sbmeshes;
    rt_matrix4 m_world;
public:
    rt_meshdata():m_tex(NULL),m_vb(NULL){}

};
class rt_mesh : public rt_object{
    friend class rabbit;
    friend class rt_d3dmanager;
protected:
    virtual void Draw()=0;

};

#endif // RT_MESH_H
