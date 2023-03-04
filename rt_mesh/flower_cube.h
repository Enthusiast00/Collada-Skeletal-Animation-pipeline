#ifndef FLOWER_CUBE_H
#define FLOWER_CUBE_H
#include "rt_mesh.h"


struct rtc_mesh;
class flower_cube : public rt_mesh,public rt_meshdata{
    friend class rabbit;
    friend class rt_d3dmanager;

    float m_model_yaw;
    rt_matrix4 m_base;

protected:
    flower_cube();
    virtual bool Init();
    virtual void Update();
    virtual void Delete();
    virtual void Draw();
};

#endif // FLOWER_CUBE_H
