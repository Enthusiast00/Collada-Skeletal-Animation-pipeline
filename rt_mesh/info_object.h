#ifndef INFO_OBJECT_H
#define INFO_OBJECT_H

#include "rt_mesh.h"

struct rt_vertex_line;
class info_object : public rt_mesh ,public  rt_meshdata {
    friend class rabbit;
    friend class rt_d3dmanager;
    friend class rt_window;
    IDirect3DVertexBuffer9* m_line_vb;

    uint16_t m_gridcount;

    void GenGrid(rt_vertex_line* v,uint16_t count);
protected:
    info_object();
    virtual bool Init();
    virtual void Update();
    virtual void Delete();
    virtual void Draw();

};



#endif // INFO_OBJECT_H
