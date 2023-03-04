#include "flower_cube.h"


#include "rabbit.h"
#include "rt_d3dmanager.h"

#include "rt_collada.h"
flower_cube::flower_cube(){
    m_model_yaw = 0.0f;
}

bool flower_cube::Init(){

    rt_collada loader;
    if(! loader.Load("flower_cube.dae")){ return false; }
    m_vertices = loader.m_mesh.vertices;
    loader.m_mesh.ReverseIndices();
    for(uint32_t i=0;i<loader.m_mesh.indices.Size();i++){
        sbmesh sbm;
        sbm.indices = loader.m_mesh.indices[i];
        m_sbmeshes.PushBack(sbm);
    }
    m_base = rt_matrix4(1);
    m_base= rtm_rotate(D3DX_PI,0.0f,0.0f,1.0f)*m_base;
    m_base= m_base* rtm_rotate(0.5f*D3DX_PI,1.0f,0.0f,0.0f);
    rtm_translate(&m_base,0.0f,0.5f,0.0f);
    m_world=m_base;


    RT_HR_T(RT_D3D->GetD3dDevice()->CreateVertexBuffer(m_vertices.Size() * sizeof(rt_vertex_),
                                                       D3DUSAGE_WRITEONLY,0,D3DPOOL_MANAGED, &m_vb,0));
    rt_vertex_* v = 0;
    RT_HR_T(m_vb->Lock(0, 0, (void**)&v, 0));
    for(uint32_t i=0;i<m_vertices.Size();i++) {
        v[i].pos = m_vertices[i].pos;
        v[i].norm = m_vertices[i].norm;
        v[i].uv = m_vertices[i].uv;
    }
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
//        m_sbmeshes[i].mat.specPower = 200.0f;

    }
    RT_HR_T(D3DXCreateTextureFromFile(RT_D3D->GetD3dDevice(),"flower_cube.bmp", &m_tex));
    AddFlags(RT_MESH_INIT);
    return true;
}

void flower_cube::Update(){

    if(RT_EVENT.isevent){
        switch( RT_EVENT.msg ) {
        case WM_KEYDOWN : {
            if((RT_EVENT.wparam == VK_LEFT) && (RT_INST->GetCurrentModel()==1)){
                m_model_yaw = m_model_yaw< -D3DX_PI? m_model_yaw:m_model_yaw-0.1f;
                m_world= m_base* rtm_rotate(m_model_yaw,0.0f,0.0f,1.0f);
            }
            if((RT_EVENT.wparam == VK_RIGHT) && (RT_INST->GetCurrentModel()==1)){
                m_model_yaw = m_model_yaw> D3DX_PI? m_model_yaw:m_model_yaw+0.1f;
                m_world= m_base* rtm_rotate(m_model_yaw,0.0f,0.0f,1.0f);
            }
        }break;
        }
    }
}
void flower_cube::Delete(){

    RELEASECOM(m_vb);
    RELEASECOM(m_tex);
    for(uint16_t i=0;i<m_sbmeshes.Size();i++){
        RELEASECOM(m_sbmeshes[i].ib);
    }
    printf("delete flower cube mesh \n");

}
void  flower_cube::Draw(){
    static rt_matrix4 MVP;

    if(RT_D3D->FlagTest(RT_D3D_DEVICE_LOST)){ return; }

    MVP = RT_D3D->Projection() * RT_D3D->View() * m_world;

    RT_HR_E(RT_D3D->GetEffect()->SetMatrix(RT_D3D->Sdata().m_hmvp,(D3DXMATRIX*) &MVP));
    RT_HR_E(RT_D3D->GetEffect()->SetMatrix(RT_D3D->Sdata().m_hmodel, (D3DXMATRIX*)&m_world));

    RT_HR_E(RT_D3D->GetD3dDevice()->SetStreamSource(0,m_vb, 0, sizeof(rt_vertex_)));

    RT_HR_E(RT_D3D->GetEffect()->SetTechnique(RT_D3D->Sdata().m_htech));
    RT_HR_E(RT_D3D->GetD3dDevice()->SetVertexDeclaration(RT_D3D->GetDec()));

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
