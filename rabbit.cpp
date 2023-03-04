#include "rabbit.h"

#include "rt_window.h"
#include "rt_d3dmanager.h"

#include "flower_cube.h"
#include "test_udo.h"
#include "suz.h"

#include "info_object.h"


rt_timer * rt_timer::s_timer = NULL;

GMSG rabbit::s_msg;
info_object * rabbit::s_info = NULL;
rabbit * rabbit::s_instance = NULL;

#define DELETE_ALL {delete RT_INST;RT_INST=NULL;delete RT_WIN;RT_WIN=NULL; \
    delete RT_D3D;RT_D3D=NULL; delete RT_TIMER; RT_TIMER=NULL;}

rabbit::rabbit(){
    m_fcube = NULL;
    m_testudo = NULL;
    m_suz = NULL;
    m_font = NULL;
    m_current_model = 0;
    AddFlags(RABBIT_HUD);
}

void rabbit::Run(){

    float sec(0.0f);
    uint16_t fcnt(0);
    RT_TIMER->Start();
    AddFlags(RABBIT_RUNNING);

    while(FlagTest(RABBIT_RUNNING)){

        RT_WIN->Update();
        RT_D3D->Update();
        Update();

        RT_D3D->PreRender();
        if(FlagTest(RABBIT_HUD)){
            RECT R = {5, 5, 0, 0};
            RT_HR_E(m_font->DrawText(0, m_hud_f.Data(), -1, &R, DT_NOCLIP, D3DCOLOR_XRGB(180,180,180)));
        }
        RT_INFO->Draw();
        for(uint32_t i=0;i<RT_INST->m_obj.Size();i++){
            if( RT_INST->m_obj[i]->FlagTest(RT_MESH_INIT) && !m_obj[i]->FlagTest(RT_MESH_DISABLE)){
                ( (rt_mesh*)RT_INST->m_obj[i] )->Draw();
            }
        }
        RT_D3D->PostRender();

        RT_TIMER->Proc();
        sec += RT_TIMER->LastFrameSeconds();
        if(sec>=1.0f){
            rt_string16 T(RTW_TITLE);
            T +=" fps: ";
            T +=L2Str(fcnt);
            T +=" lfms:";
            T +=F2Str(RT_TIMER->LastFrameMseconds());
            RT_WIN->SetTitle(T.Data());
            fcnt=0; sec=0;

        }else{ fcnt++; }

    }
}
void rabbit::Update(){

    if(RT_EVENT.isevent){
        switch( RT_EVENT.msg ) {
        case WM_KEYDOWN : {
            if(RT_EVENT.wparam == VK_ESCAPE){ PostMessage(RT_WIN->m_hwnd,WM_CLOSE,0,0); }

            if(!RT_WIN->FlagTest(RT_WIN_LSHIFT) && !RT_WIN->FlagTest(RT_WIN_LCTRL)){
                if(RT_EVENT.wparam == 0x31 ) {
                    m_testudo->Enable();m_fcube->Disable();m_suz->Disable();
                    RT_INST->m_hud_f.Clear();
                    RT_INST->m_hud_f += RT_INST->m_hud;
                    RT_INST->m_hud_f += "model : Test-udo\n";
                    RT_INST->m_hud_f += "play animation- leftshift + 1->2\n";
                    RT_INST->m_hud_f += "pause- leftctrl + p\n";
                    RT_INST->m_hud_f += "reset- r\n";
                    RT_INST->m_hud_f += "animspeed- leftshift+ -+\n";
                    RT_D3D->m_cam_target = rt_vector3(0,6.0f,0);
                    RT_D3D->m_cam_yaw = 0.0f;
                    RT_D3D->m_cam_pitch = -D3DXToRadian(20);
                    RT_D3D->m_cam_fov = 0.20f;
                    m_current_model = 0;
                    RT_D3D->GenProjection();
                }
                if(RT_EVENT.wparam == 0x32 ) {
                    m_testudo->Disable();m_fcube->Enable();m_suz->Disable();
                    RT_INST->m_hud_f.Clear();
                    RT_INST->m_hud_f+= RT_INST->m_hud;
                    RT_INST->m_hud_f+= "model : Flower Cube\n";
                    RT_D3D->m_cam_target = rt_vector3(0,0.75f,0);
                    RT_D3D->m_cam_yaw = 0.0f;
                    RT_D3D->m_cam_pitch = -D3DXToRadian(20);
                    RT_D3D->m_cam_fov = 0.10f;
                    m_current_model = 1;
                    RT_D3D->GenProjection();
                }
                if(RT_EVENT.wparam == 0x33 ) {
                    m_testudo->Disable();m_fcube->Disable();m_suz->Enable();
                    RT_INST->m_hud_f.Clear();
                    RT_INST->m_hud_f+= RT_INST->m_hud;
                    RT_INST->m_hud_f+= "model : Suzanne\n";
                    RT_INST->m_hud_f+= "play animation- leftshift + 1->3\n";
                    RT_INST->m_hud_f+= "pause- leftctrl + p\n";
                    RT_INST->m_hud_f+= "reset- r\n";
                    RT_INST->m_hud_f+= "animspeed- leftshift+ -+\n";
                    RT_D3D->m_cam_target = rt_vector3(0,1.0f,0);
                    RT_D3D->m_cam_yaw = 0.0f;
                    RT_D3D->m_cam_pitch = -D3DXToRadian(8);
                    RT_D3D->m_cam_fov = 0.08f;
                    m_current_model = 2;
                    RT_D3D->GenProjection();
                }
            }
            if(RT_EVENT.wparam == 0x54){
                if(FlagTest(RABBIT_HUD)){ RemoveFlags(RABBIT_HUD); }
                else {    AddFlags(RABBIT_HUD); }
            }
            if(RT_EVENT.wparam == 0x52){
                m_testudo->Enable();
                m_testudo->SetToBind();
                m_fcube->Disable();
                m_suz->Disable();
                m_suz->SetToBind();

                RT_INST->m_hud_f.Clear();
                RT_INST->m_hud_f+= RT_INST->m_hud;
                RT_INST->m_hud_f+= "model : Test-udo\n";
                RT_INST->m_hud_f+= "play animation- leftshift + 1->2\n";
                RT_INST->m_hud_f+= "pause- leftctrl + p\n";
                RT_INST->m_hud_f+= "reset- r\n";
                RT_INST->m_hud_f+= "animspeed- leftshift+ -+\n";
                RT_D3D->m_cam_target = rt_vector3(0,6.0f,0);
                m_current_model = 0;
                RT_D3D->m_cam_yaw = 0.0f;
                RT_D3D->m_cam_pitch = -D3DXToRadian(20);
                RT_D3D->m_cam_fov = 0.20f;
                RT_D3D->GenProjection();
            }
        }
        }
    }else{
        RT_INFO->Update();
        for(uint32_t i=0;i<RT_INST->m_obj.Size();i++){
            if(!m_obj[i]->FlagTest(RT_MESH_DISABLE)){ m_obj[i]->Update(); }
        }
    }
}
bool rabbit::Init(){

    RT_ZERO(RT_EVENT);
    RT_INST = new rabbit();
    RT_WIN = new rt_window();
    RT_D3D = new rt_d3dmanager();
    RT_TIMER = new rt_timer();

    if(!RT_WIN->Init()){ DELETE_ALL; RT_THROW("window init"); }
    if(!RT_D3D->Init()){  DELETE_ALL; RT_THROW("d3dmanager init"); }

    RT_INFO  = new info_object();

    RT_INST->m_fcube = new flower_cube();
    RT_INST->m_fcube->Disable();
    RT_INST->m_testudo = new test_udo();
    RT_INST->m_suz = new monkey();
    RT_INST->m_suz->Disable();

    RT_INST->m_obj.PushBack( RT_INST->m_fcube );
    RT_INST->m_obj.PushBack( RT_INST->m_testudo );
    RT_INST->m_obj.PushBack( RT_INST->m_suz );

    if(!RT_INFO->Init()){  DELETE_ALL; RT_THROW("info obj init"); }
    for(uint32_t i=0;i<RT_INST->m_obj.Size();i++){
        if(!RT_INST->m_obj[i]->Init()) {  DELETE_ALL;  RT_THROW("obj init"); }
    }

    D3DXFONT_DESC fontDesc;
    fontDesc.Height          = 16;
    fontDesc.Width           = 0;
    fontDesc.Weight          = 0;
    fontDesc.MipLevels       = 1;
    fontDesc.Italic          = false;
    fontDesc.CharSet         = DEFAULT_CHARSET;
    fontDesc.OutputPrecision = OUT_DEFAULT_PRECIS;
    fontDesc.Quality         = DEFAULT_QUALITY;
    fontDesc.PitchAndFamily  = DEFAULT_PITCH | FF_DONTCARE;

    const char * fn = "Times New Roman";
    for(int i=0;i<15;fontDesc.FaceName[i]=fn[i], i++);
    fontDesc.FaceName[15]=0;

    RT_HR_T(D3DXCreateFontIndirect(RT_D3D->GetD3dDevice(), &fontDesc, &RT_INST->m_font));

    RT_INST->m_hud.Clear();
    RT_INST->m_hud+="switch model- 1-> 3\n";
    RT_INST->m_hud+="toggle text- t\n";
    RT_INST->m_hud+="toggle vsync- leftctrl+v\n";
    RT_INST->m_hud+="toggle fullscreen- f\n";
    RT_INST->m_hud+="rotate model- <- -> \n";
    RT_INST->m_hud+= "magnification- -+\n";
    RT_INST->m_hud+= "toggle horizontal grid- h\n";
    RT_INST->m_hud+= "toggle vertical grid- v\n\n";
    RT_INST->m_hud_f+= RT_INST->m_hud;
    RT_INST->m_hud_f+= "model : Test-udo\n";
    RT_INST->m_hud_f+= "play animation- leftshift + 1->2\n";
    RT_INST->m_hud_f+= "pause- leftctrl + p\n";
    RT_INST->m_hud_f+= "reset- r\n";
    RT_INST->m_hud_f+= "animspeed- leftshift+ -+\n";
    RT_D3D->m_cam_target = rt_vector3(0,6.0f,0);
    RT_INST->AddFlags(RABBIT_INIT);
    return true;
}

void rabbit::Delete(){

    RELEASECOM(RT_INST->m_font);
    for(uint32_t i=0;i<RT_INST->m_obj.Size();i++){ RT_INST->m_obj[i]->Delete(); }
    RT_D3D->Delete();
    if(RT_INST->m_fcube) { delete RT_INST->m_fcube; RT_INST->m_fcube = NULL; }
    if(RT_INST->m_testudo) { delete RT_INST->m_testudo; RT_INST->m_testudo = NULL; }
    if(RT_INST->m_suz) { delete RT_INST->m_suz; RT_INST->m_suz = NULL;}
    DELETE_ALL;

    printf("delete rabbit \n");
}
