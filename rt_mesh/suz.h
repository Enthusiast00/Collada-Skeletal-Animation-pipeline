#ifndef MONKEY_H
#define MONKEY_H

#include "rt_mesh.h"

struct rt_node;
struct rtc_mesh;

class monkey : public rt_mesh ,public  rt_meshdata {
    friend class rabbit;
    friend class rt_d3dmanager;
    float m_end;
    float m_start;
    float m_interval;
    float m_intervalinc;
    float m_boneinterval;
    float m_animspeed;
    int16_t m_boneinx,m_fboneinx,m_lboneinx;

    float m_model_yaw;
    rt_matrix4 m_base;

protected:
    monkey();
    virtual bool Init();
    virtual void Update();
    virtual void Delete();
    virtual void Draw();
private:
    void SetToBind();
    void Play(float st, float en);
    void Pause();
    void Reset();
    void AnimProc();

    D3DXMATRIX* GetBones();

    rt_tarray m_bones;
    rt_aarray m_anims;
    rt_tarray m_currbones;

    rt_farray m_animinputs;
};


#endif // MONKEY_H
