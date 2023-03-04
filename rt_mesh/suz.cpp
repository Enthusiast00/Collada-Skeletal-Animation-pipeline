#include "suz.h"

#include "rabbit.h"
#include "rt_window.h"
#include "rt_d3dmanager.h"

#include "rt_collada.h"


monkey::monkey(){
    m_end = 0.0f;
    m_start = 0.0f;
    m_interval = 0.0f;
    m_intervalinc = 0.0f;
    m_boneinterval = 0.0f;
    m_animspeed = 1.0f;

    m_boneinx = -1;
    m_lboneinx = 0;
    m_fboneinx = 0;
    m_model_yaw = 0.0f;
}

bool monkey::Init(){
    rt_collada loader;
    uint64_t st = StartClock();
    if(! loader.Load("suz.dae")){ return false; }
    printf("loading .. time %u \n",EndClock(st));

    m_vertices = loader.m_mesh.vertices;
    loader.m_mesh.ReverseIndices();
    for(uint32_t i=0;i<loader.m_mesh.indices.Size();i++){
        sbmesh sbm;
        sbm.indices = loader.m_mesh.indices[i];
        m_sbmeshes.PushBack(sbm);
    }
    m_bones=loader.m_mesh.finaljointtransforms;
    m_currbones.Allocate(m_bones.Size());
    m_anims = loader.m_mesh.finalanimstransforms;
    m_animinputs = loader.m_mesh.a_inputs;

    m_base = rt_matrix4(1);
    m_base= rtm_rotate(D3DX_PI,0.0f,0.0f,1.0f)*m_base;
    m_base= m_base* rtm_rotate(0.5f*D3DX_PI,1.0f,0.0f,0.0f);
    rtm_translate(&m_base,0.0f,2.0f,0.0f);
    m_world=m_base;



    RT_HR_T(RT_D3D->GetD3dDevice()->CreateVertexBuffer(m_vertices.Size() * sizeof(rt_vertex),
                                                       D3DUSAGE_WRITEONLY,0,D3DPOOL_MANAGED, &m_vb,0));
    rt_vertex* v = 0;
    RT_HR_T(m_vb->Lock(0, 0, (void**)&v, 0));
    for(uint32_t i=0;i<m_vertices.Size();v[i] = m_vertices[i], i++);
    RT_HR_T(m_vb->Unlock());

    for(uint32_t i=0;i<m_sbmeshes.Size();i++){
        RT_HR_T(RT_D3D->GetD3dDevice()->CreateIndexBuffer(m_sbmeshes[i].indices.Size() * sizeof(WORD),
                                                          D3DUSAGE_WRITEONLY,D3DFMT_INDEX16, D3DPOOL_MANAGED,
                                                          &m_sbmeshes[i].ib, 0));
        WORD* k = 0;
        RT_HR_T(m_sbmeshes[i].ib->Lock(0, 0, (void**)&k, 0));
        for(uint16_t ii=0;ii<m_sbmeshes[i].indices.Size(); ii++){
            k[ii] = m_sbmeshes[i].indices[ii];
        }
        RT_HR_T(m_sbmeshes[i].ib->Unlock());

        m_sbmeshes[i].mat.ambient = rt_vector3(0.5f, 0.5f, 0.5f);
        m_sbmeshes[i].mat.diffuse = rt_vector3(0.8f, 0.8f, 0.8f);
        m_sbmeshes[i].mat.specular = rt_vector3(0.8f, 0.8f, 0.8f);
//        m_sbmeshes[i].mat.specPower = 100.0f;

    }
    RT_HR_T(D3DXCreateTextureFromFile(RT_D3D->GetD3dDevice(),"suz_uv.bmp", &m_tex));
    AddFlags(RT_MESH_INIT);
    return true;
}

void monkey::Update(){
    if(RT_EVENT.isevent){
        switch( RT_EVENT.msg ) {
        case WM_KEYUP : {
            if(!RT_WIN->FlagTest(RT_WIN_LSHIFT) && !RT_WIN->FlagTest(RT_WIN_LCTRL)){
                if(RT_EVENT.wparam == 0x50){ Pause(); }
            }
            if(RT_WIN->FlagTest(RT_WIN_LSHIFT) && !RT_WIN->FlagTest(RT_WIN_LCTRL)){
                if(RT_EVENT.wparam == 0x31){ Play(m_animinputs[0],m_animinputs[ 13 ]); }
                if(RT_EVENT.wparam == 0x32){ Play(m_animinputs[14],m_animinputs[20]); }
                if(RT_EVENT.wparam == 0x33){ Play(m_animinputs[21],m_animinputs[25]); }
            }
        }break;
        case WM_KEYDOWN : {
            if((RT_EVENT.wparam == VK_LEFT) && (RT_INST->GetCurrentModel()==2)){
                m_model_yaw = m_model_yaw< -D3DX_PI? m_model_yaw:m_model_yaw-0.1f;
                m_world= m_base* rtm_rotate(m_model_yaw,0.0f,0.0f,1.0f);
            }
            if((RT_EVENT.wparam == VK_RIGHT) && (RT_INST->GetCurrentModel()==2)){
                m_model_yaw = m_model_yaw> D3DX_PI? m_model_yaw:m_model_yaw+0.1f;
                m_world= m_base* rtm_rotate(m_model_yaw,0.0f,0.0f,1.0f);
            }

            if(RT_WIN->FlagTest(RT_WIN_LSHIFT)){
                if(RT_EVENT.wparam == VK_OEM_MINUS ){
                    m_animspeed = m_animspeed<=0.2f?0.2f:m_animspeed-0.01f;
                }
                if(RT_EVENT.wparam == VK_OEM_PLUS ){
                    m_animspeed = m_animspeed>=2.0f?2.0f:m_animspeed+0.01f;
                }
            }
        }break;
        }
    }else { AnimProc(); }
}
void monkey::Delete(){
    RELEASECOM(m_vb);
    RELEASECOM(m_tex);
    for(uint16_t i=0;i<m_sbmeshes.Size();i++){
        RELEASECOM(m_sbmeshes[i].ib);
    }
    printf("delete monkey \n");
}

void monkey::Draw(){
    static rt_matrix4 MVP;

    if(RT_D3D->FlagTest(RT_D3D_DEVICE_LOST)){ return; }

    MVP = RT_D3D->Projection() * RT_D3D->View() * m_world;

    RT_HR_E(RT_D3D->GetEffect()->SetMatrix(RT_D3D->Sdata().m_hmvp,(D3DXMATRIX*) &MVP));
    RT_HR_E(RT_D3D->GetEffect()->SetMatrix(RT_D3D->Sdata().m_hmodel, (D3DXMATRIX*)&m_world));

    RT_HR_E(RT_D3D->GetD3dDevice()->SetStreamSource(0,m_vb, 0, sizeof(rt_vertex)));

    RT_HR_E(RT_D3D->GetEffect()->SetTechnique(RT_D3D->Sdata().m_hatech));
    RT_HR_E(RT_D3D->GetD3dDevice()->SetVertexDeclaration(RT_D3D->GetAnimDec()));
    RT_HR_E(RT_D3D->GetEffect()->SetMatrixArray(RT_D3D->Sdata().m_hbones,GetBones(),m_currbones.Size() ));

    RT_HR_E(RT_D3D->GetEffect()->SetTexture(RT_D3D->Sdata().m_htex, m_tex));

    RT_HR_E(RT_D3D->GetEffect()->Begin(NULL, 0));
    RT_HR_E(RT_D3D->GetEffect()->BeginPass(0));
    for(uint32_t ii=0;ii<m_sbmeshes.Size(); ii++){
        RT_HR_E(RT_D3D->GetEffect()->SetValue(RT_D3D->Sdata().m_hmtrl, &m_sbmeshes[ii].mat, sizeof(rt_material)));
        RT_HR_E( RT_D3D->GetD3dDevice()->SetIndices(m_sbmeshes[ii].ib) );
        RT_HR_E( RT_D3D->GetEffect()->CommitChanges() );
        RT_HR_E(RT_D3D->GetD3dDevice()->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0,
                                                             m_vertices.Size(), 0,
                                                             m_sbmeshes[ii].indices.Size()/3));
    }
    RT_HR_E(RT_D3D->GetEffect()->EndPass());
    RT_HR_E(RT_D3D->GetEffect()->End());
}

void  monkey::SetToBind(){
    RemoveFlags(RT_MESH_PAUSE);
    m_boneinx = -1;
    m_end = m_start = m_boneinterval = 0.0f;
    m_lboneinx = m_fboneinx = 0;
}
void  monkey::Play(float st, float en){

    m_end = en;
    m_start = m_boneinterval = st;

    for(uint16_t i=0;i<m_animinputs.Size();i++){
        if( m_animinputs[i] >= st){ m_fboneinx = i; break;}
    }
    m_lboneinx = m_animinputs.Size()-1;
    for(uint16_t i=0;i<m_animinputs.Size();i++){
        if( m_animinputs[i] >= en){ m_lboneinx = i; break;}
    }
    int16_t cnt = m_lboneinx - m_fboneinx;
    if(cnt>2){
        m_interval = m_animinputs[m_fboneinx+1]-m_start;
        m_boneinx = m_fboneinx;
    }
    else{
        m_end = m_start = m_boneinterval = 0.0f;
        m_boneinx = -1;
        m_lboneinx = 0;
        m_fboneinx = 0;
    }

    printf(" fi: %i li: %i \n",m_fboneinx,m_lboneinx);
}

void  monkey::Pause(){
    if(FlagTest(RT_MESH_PAUSE)){ RemoveFlags(RT_MESH_PAUSE); }
    else { AddFlags(RT_MESH_PAUSE); }
}

void  monkey::AnimProc(){

    if(m_boneinx>=0 && !FlagTest(RT_MESH_PAUSE)){

        m_boneinterval += RT_TIMER->LastFrameSeconds()*m_animspeed;
        m_intervalinc  += RT_TIMER->LastFrameSeconds()*m_animspeed;

        if(m_boneinterval >= m_animinputs[m_boneinx+1]){
            if( m_boneinx == m_lboneinx-1 ){
                m_boneinterval = m_start;
                m_boneinx = m_fboneinx;
                m_interval = m_animinputs[m_boneinx+1]-m_start;
            }else {
                m_boneinx++;
                float last = m_boneinx==m_lboneinx-1?m_end:m_animinputs[m_boneinx+1];
                m_interval = last-m_animinputs[m_boneinx];
            }
            m_intervalinc = 0.0f;
        }
        for(uint16_t i=0;i<m_anims[m_boneinx].Size();i++){
            float * St = (float*)&(m_anims[m_boneinx][i]);
            float * En = (float*)&(m_anims[m_boneinx+1][i]);
            float * C = (float*)&(m_currbones[i]);
            for(uint8_t ii=0;ii<16;ii++){
                C[ii] = lerp(St[ii],En[ii],(1.0f/m_interval)*m_intervalinc);
            }
        }


    }

}

D3DXMATRIX* monkey::GetBones(){
    if(m_boneinx>=0) { return (D3DXMATRIX*)m_currbones.Data(); }
    return (D3DXMATRIX*)m_bones.Data();
}
