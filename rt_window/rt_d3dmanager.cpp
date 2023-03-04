#include "rabbit.h"
#include "rt_window.h"
#include "rt_mesh.h"

#include "rt_d3dmanager.h"

rt_d3dmanager * rt_d3dmanager::s_d3dmanager = NULL;

rt_d3dmanager::rt_d3dmanager(){

    m_d3dobject = NULL;
    RT_ZERO(m_d3dpp);
    m_d3ddevice = NULL;
    m_decl = m_decl_ = m_decl_s = NULL;

    m_sdata.m_htech = NULL;
    m_sdata.m_hatech = NULL;
    m_sdata.m_hatech_s = NULL;
    m_sdata.m_hlinetech = NULL;
    m_sdata.m_hmvp = NULL;
    m_sdata.m_hbones = NULL;
    m_sdata.m_heyepos = NULL;
    m_sdata.m_hmodel = NULL;
    m_sdata.m_htex = NULL;
    m_sdata.m_hmtrl = NULL;
    m_sdata.m_hlight = NULL;

    m_fx = NULL;

    m_cam_yaw = 0.0f;
    m_cam_pitch = -D3DXToRadian(20);
    m_cam_radius = 25.0f;
    m_cam_fov = 0.20f;

}
bool rt_d3dmanager::Init(){

    m_d3dobject = Direct3DCreate9(D3D_SDK_VERSION);
    if( !m_d3dobject ) { RT_THROW("Direct3DCreate9"); }

    D3DDISPLAYMODE mode;
    m_d3dobject->GetAdapterDisplayMode(D3DADAPTER_DEFAULT, &mode);
    RT_HR_T(m_d3dobject->CheckDeviceType(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, mode.Format, mode.Format, true));
    RT_HR_T(m_d3dobject->CheckDeviceType(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, D3DFMT_X8R8G8B8, D3DFMT_X8R8G8B8, false));

    D3DCAPS9 caps;
    RT_HR_T(m_d3dobject->GetDeviceCaps(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, &caps));
    DWORD devBehaviorFlags = 0;
    if( caps.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT ) { devBehaviorFlags |= D3DCREATE_HARDWARE_VERTEXPROCESSING; }
    else { devBehaviorFlags |= D3DCREATE_SOFTWARE_VERTEXPROCESSING; }
    if( caps.DevCaps & D3DDEVCAPS_PUREDEVICE &&
            devBehaviorFlags & D3DCREATE_HARDWARE_VERTEXPROCESSING) { devBehaviorFlags |= D3DCREATE_PUREDEVICE; }

    m_d3dpp.BackBufferFormat           = D3DFMT_UNKNOWN;
    m_d3dpp.BackBufferCount            = 1;
    m_d3dpp.MultiSampleType            = D3DMULTISAMPLE_NONE;
    m_d3dpp.SwapEffect                 = D3DSWAPEFFECT_DISCARD;
    m_d3dpp.hDeviceWindow              = RT_WIN->m_hwnd;
    m_d3dpp.Windowed                   = true;
    m_d3dpp.EnableAutoDepthStencil     = true;
    m_d3dpp.AutoDepthStencilFormat     = D3DFMT_D24S8;
    m_d3dpp.FullScreen_RefreshRateInHz = D3DPRESENT_RATE_DEFAULT;
    m_d3dpp.PresentationInterval       = D3DPRESENT_INTERVAL_IMMEDIATE;

    RT_HR_T(m_d3dobject->CreateDevice(D3DADAPTER_DEFAULT,D3DDEVTYPE_HAL,
                                      RT_WIN->m_hwnd,devBehaviorFlags,&m_d3dpp,&m_d3ddevice));

    RT_ZERO(caps);
    RT_HR_T(m_d3ddevice->GetDeviceCaps(&caps));
    if( caps.VertexShaderVersion < D3DVS_VERSION(2, 0) ) { RT_THROW("v_shaderversion"); }
    if( caps.PixelShaderVersion < D3DPS_VERSION(2, 0) ) { RT_THROW("p_shaderversion"); }

    D3DVERTEXELEMENT9 elements[] = {
        {0, 0,  D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0},
        {0, 12, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL, 0},
        {0, 24, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0},
        {0, 32, D3DDECLTYPE_FLOAT4, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_BLENDWEIGHT, 0},
        {0, 48, D3DDECLTYPE_UBYTE4, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_BLENDINDICES, 0},
        D3DDECL_END()
    };RT_HR_T(m_d3ddevice->CreateVertexDeclaration(elements, &m_decl));
    D3DVERTEXELEMENT9 elements_[] = {
        {0, 0,  D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0},
        {0, 12, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL, 0},
        {0, 24, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0},
        D3DDECL_END()
    };RT_HR_T(m_d3ddevice->CreateVertexDeclaration(elements_, &m_decl_));
    D3DVERTEXELEMENT9 elements_s[] = {
        {0, 0,  D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0},
        {0, 12, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL, 0},
        {0, 24, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0},
        {0, 32, D3DDECLTYPE_FLOAT1, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_BLENDWEIGHT, 0},
        {0, 36, D3DDECLTYPE_UBYTE4, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_BLENDINDICES, 0},
        D3DDECL_END()
    };RT_HR_T(m_d3ddevice->CreateVertexDeclaration(elements_s, &m_decl_s));
    D3DVERTEXELEMENT9 elements_line[] = {
        {0, 0,  D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0},
        {0, 12, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_COLOR, 0},
        D3DDECL_END()
    };RT_HR_T(m_d3ddevice->CreateVertexDeclaration(elements_line, &m_decl_line));

    ID3DXBuffer* errors = 0;
    RT_HR_E(D3DXCreateEffectFromFile(m_d3ddevice, "rabbit.hlsl",
                                     0, 0, D3DXSHADER_DEBUG, 0, &m_fx, &errors));
    if( errors ) { RT_THROW((char*)errors->GetBufferPointer()); }
    m_sdata.m_htech = m_fx->GetTechniqueByName("Tech");
    m_sdata.m_hatech = m_fx->GetTechniqueByName("AnimTech");
    m_sdata.m_hatech_s = m_fx->GetTechniqueByName("Anim_sTech");
    m_sdata.m_hlinetech = m_fx->GetTechniqueByName("LineTech");
    m_sdata.m_hmvp = m_fx->GetParameterByName(0, "MVP");
    m_sdata.m_hbones = m_fx->GetParameterByName(0, "Bones");
    m_sdata.m_hmtrl = m_fx->GetParameterByName(0, "Material");
    m_sdata.m_hlight = m_fx->GetParameterByName(0, "Light");
    m_sdata.m_heyepos = m_fx->GetParameterByName(0, "EyePos");
    m_sdata.m_hmodel = m_fx->GetParameterByName(0, "Model");
    m_sdata.m_htex = m_fx->GetParameterByName(0, "Tex");


    D3DXVECTOR3 dir = D3DXVECTOR3(0.0f, 0.0f, 1.0f);
    D3DXVec3Normalize(&dir, &dir);
    m_light.dir = rt_vector3(dir.x,dir.y,dir.z);

    m_light.ambient = rt_vector3(0.5f, 0.5f, 0.5f);
    m_light.diffuse = rt_vector3(0.8f, 0.8f, 0.8f);
    m_light.specular = rt_vector3(0.8f, 0.8f, 0.8f);

    GenProjection();
    return true;
}

void rt_d3dmanager::Update(){

    if(RT_EVENT.isevent){
        switch( RT_EVENT.msg ) {
        case WM_KEYDOWN : {
            if((RT_EVENT.wparam == 0x56) && RT_WIN->FlagTest(RT_WIN_LCTRL)){
                if(m_d3dpp.PresentationInterval == D3DPRESENT_INTERVAL_ONE){ Vsync(false); }
                else { Vsync(true); }
            }
            if( !RT_WIN->FlagTest(RT_WIN_LSHIFT) ){
                if(RT_EVENT.wparam == VK_OEM_PLUS ){
                    m_cam_fov = m_cam_fov<0.07f?0.06f:m_cam_fov-0.01f;
                    GenProjection();
                }
                if(RT_EVENT.wparam == VK_OEM_MINUS ){
                    m_cam_fov = m_cam_fov>0.79f?0.8f:m_cam_fov+0.01f;
                    GenProjection();
                }
            }
        }break;
        case WM_LBUTTONDOWN: {
            m_currpointerpos = RT_WIN->m_pointerpos;
        }break;
        case WM_MOUSEMOVE: {
            if(RT_WIN->FlagTest(RT_WIN_LBUTTOND)){
                int x = m_currpointerpos.x-RT_WIN->m_pointerpos.x;
                int y = m_currpointerpos.y-RT_WIN->m_pointerpos.y;
                m_currpointerpos = RT_WIN->m_pointerpos;

                m_cam_yaw -= D3DXToRadian(x);
                m_cam_pitch += D3DXToRadian(y);
                m_cam_yaw = m_cam_yaw>D3DX_PI?D3DX_PI:m_cam_yaw;
                m_cam_yaw = m_cam_yaw<-D3DX_PI?-D3DX_PI:m_cam_yaw;
                m_cam_pitch = m_cam_pitch>D3DX_PI?D3DX_PI:m_cam_pitch;
                m_cam_pitch = m_cam_pitch<-D3DX_PI?-D3DX_PI:m_cam_pitch;
            }
        }break;

        }

    }else{
        HRESULT hr = m_d3ddevice->TestCooperativeLevel();
        RemoveFlags(RT_D3D_DEVICE_LOST);
        if( hr == D3DERR_DEVICELOST ) {
            Sleep(20);
            AddFlags(RT_D3D_DEVICE_LOST);
        }
        else if( hr == D3DERR_DRIVERINTERNALERROR ) {
            RT_ERROR("device error");
            RT_INST->RemoveFlags(RABBIT_RUNNING);
        }
        else if( hr == D3DERR_DEVICENOTRESET ) { Reset(); }
        GenView();
    }
}

void rt_d3dmanager::Delete(){


    RELEASECOM(m_d3dobject);
    RELEASECOM(m_d3ddevice);
    RELEASECOM(m_decl);
    RELEASECOM(m_fx);
    printf("delete d3d \n");

}
void rt_d3dmanager::Vsync(bool v){
    if(v) { m_d3dpp.PresentationInterval= D3DPRESENT_INTERVAL_ONE; }
    else { m_d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE; }
    Reset();
}

bool  rt_d3dmanager::Reset(){
    RT_HR_T(m_fx->OnLostDevice());
    RT_HR_E(RT_INST->m_font->OnLostDevice());

    RT_HR_T(m_d3ddevice->Reset(&m_d3dpp));

    RT_HR_T(m_fx->OnResetDevice());
    RT_HR_E(RT_INST->m_font->OnResetDevice());
    return true;
}


void rt_d3dmanager::PreRender(){
    if(FlagTest(RT_D3D_DEVICE_LOST)){ return ; }
    RT_HR_E(m_d3ddevice->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0x4c4c4c, 1.0f, 0));
    RT_HR_E(m_d3ddevice->BeginScene());
    RT_HR_E(m_fx->SetValue(m_sdata.m_hlight, &m_light, sizeof(rt_dirlight)));
}

void rt_d3dmanager::PostRender(){
    if(FlagTest(RT_D3D_DEVICE_LOST)){ return; }
    RT_HR_E(m_d3ddevice->EndScene());
    RT_HR_E(m_d3ddevice->Present(0, 0, 0, 0));
}

void rt_d3dmanager::GenView(){
    rt_vector4 pos(0, 0, -m_cam_radius,0);
    rt_vector4 up(0, 1, 0,0);
    rt_matrix4 R = rtm_rotate(m_cam_yaw,0.0f,1.0f,0.0f);
    R = rtm_rotate(m_cam_pitch,1.0f,0.0f,0.0f)*R;
    pos = R*pos;
    up = R*up;
    m_cam_pos = rt_vector3(pos.x,pos.y,pos.z);
    m_view = rtm_lookat( m_cam_pos,m_cam_target, rt_vector3(up.x,up.y,up.z) );
}

void  rt_d3dmanager::GenProjection(){
    float w = (float)m_d3dpp.BackBufferWidth,h = (float)m_d3dpp.BackBufferHeight;
    m_proj = rtm_perspective(D3DX_PI * m_cam_fov, w/h, 1.0f, 5000.0f);
}
