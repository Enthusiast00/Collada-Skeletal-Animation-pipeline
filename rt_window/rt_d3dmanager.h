#ifndef RT_D3DMANAGER_H
#define RT_D3DMANAGER_H

#include "rt_defs.h"

struct shader_data{
    D3DXHANDLE   m_htech , m_hatech ,m_hatech_s,m_hlinetech;
    D3DXHANDLE   m_hmvp, m_hbones,m_heyepos,m_hmodel,m_htex,m_hmtrl,m_hlight;
    shader_data(){
        m_htech=m_hatech=m_hatech_s=m_hlinetech=NULL;
        m_hmvp=m_hbones=m_heyepos=m_hmodel=m_htex=m_hmtrl=m_hlight=NULL;
    }
};

class rt_d3dmanager : public flag_c {

    friend class rabbit;
    friend class rt_window;
    friend class rt_mesh;

    POINT m_currpointerpos;
protected:
    IDirect3D9* m_d3dobject;
    D3DPRESENT_PARAMETERS m_d3dpp;
    IDirect3DDevice9* m_d3ddevice;
    IDirect3DVertexDeclaration9* m_decl;
    IDirect3DVertexDeclaration9* m_decl_;
    IDirect3DVertexDeclaration9* m_decl_s;

    IDirect3DVertexDeclaration9* m_decl_line;


    shader_data m_sdata;
    rt_dirlight m_light;
    ID3DXEffect* m_fx;

    rt_string16 m_stats;

    float m_cam_yaw;
    float m_cam_pitch;
    float m_cam_radius;

    float m_cam_fov;

    rt_matrix4 m_view;
    rt_matrix4 m_proj;
    rt_vector3 m_cam_pos;
    rt_vector3 m_cam_target;

    rt_d3dmanager();
    bool Init();
    void Update();
    void Delete();

    bool Reset();
    void Vsync(bool v);
    void PreRender();
    void PostRender();
    void GenView();
    void GenProjection();

public:
    ID3DXEffect*  GetEffect() { return m_fx; }
    IDirect3DDevice9* GetD3dDevice() { return m_d3ddevice; }

    IDirect3DVertexDeclaration9* GetDec(){ return m_decl_;}
    IDirect3DVertexDeclaration9* GetAnimDec(){ return m_decl; }
    IDirect3DVertexDeclaration9* GetSAnimDec(){ return m_decl_s; }
    IDirect3DVertexDeclaration9* GetLineDec(){ return m_decl_line; }

    const shader_data & Sdata(){return m_sdata;}
    const rt_matrix4 & View(){return m_view;}
    const rt_matrix4 & Projection(){return m_proj;}

    static  rt_d3dmanager * s_d3dmanager;
};

#endif // RT_D3DMANAGER_H
