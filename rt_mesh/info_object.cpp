#include "info_object.h"


#include "rabbit.h"
#include "rt_window.h"
#include "rt_d3dmanager.h"

#include "rt_collada.h"
info_object::info_object(){
    m_line_vb = NULL;
    m_gridcount = 20;

    AddFlags(RT_MESH_HGRID);
}

void  info_object::GenGrid(rt_vertex_line* v,uint16_t count){

    int pos(0);

    v[pos].pos = rt_vector3(0,0,(float)-count);
    v[pos++].col = rt_vector3(0,0,1);
    v[pos].pos = rt_vector3(0,0,(float)count);
    v[pos++].col = rt_vector3(0,0,1);
    rt_vector3 color(0.6f,0.6f,0.6f);
    for(int i=1;i<count+1;i++){
        v[pos].pos = rt_vector3((float)-count,0,i*1.f);
        v[pos++].col = color;
        v[pos].pos = rt_vector3((float)count,0,i*1.0f);
        v[pos++].col = color;

        v[pos].pos = rt_vector3((float)-count,0,-i*1.0f);
        v[pos++].col = color;
        v[pos].pos = rt_vector3((float)count,0,-i*1.0f);
        v[pos++].col = color;

        v[pos].pos = rt_vector3(i*1.f,0,(float)-count);
        v[pos++].col = color;
        v[pos].pos = rt_vector3(i*1.0f,0,(float)count);
        v[pos++].col = color;
        v[pos].pos = rt_vector3(-i*1.0f,0,(float)-count);
        v[pos++].col = color;
        v[pos].pos = rt_vector3(-i*1.0f,0,(float)count);
        v[pos++].col = color;
    }
    v[pos].pos = rt_vector3((float)-count,0,0);
    v[pos++].col = rt_vector3(1,0,0);
    v[pos].pos = rt_vector3((float)count,0,0);
    v[pos++].col = rt_vector3(1,0,0);


    v[pos].pos = rt_vector3(0,(float)-count,0);
    v[pos++].col = rt_vector3(0,1,0);
    v[pos].pos = rt_vector3(0,(float)count,0);
    v[pos++].col = rt_vector3(0,1,0);

    for(int i=1;i<count+1;i++){
        v[pos].pos = rt_vector3((float)-count,i*1.f,0.0f);
        v[pos++].col = color;
        v[pos].pos = rt_vector3((float)count,i*1.0f,0.0f);
        v[pos++].col = color;
        v[pos].pos = rt_vector3((float)-count,-i*1.0f,0.0f);
        v[pos++].col = color;
        v[pos].pos = rt_vector3((float)count,-i*1.0f,0.0f);
        v[pos++].col = color;

        v[pos].pos = rt_vector3(i*1.f,(float)-count,0.0f);
        v[pos++].col = color;
        v[pos].pos = rt_vector3(i*1.0f,(float)count,0.0f);
        v[pos++].col = color;
        v[pos].pos = rt_vector3(-i*1.0f,(float)-count,0.0f);
        v[pos++].col = color;
        v[pos].pos = rt_vector3(-i*1.0f,(float)count,0.0f);
        v[pos++].col = color;
    }

    printf("pos :%i \n",pos);
}

bool info_object::Init(){

    rt_collada loader;
    if(! loader.Load("stand.dae")){ return false; }
    m_vertices = loader.m_mesh.vertices;
    loader.m_mesh.ReverseIndices();
    for(uint32_t i=0;i<loader.m_mesh.indices.Size();i++){
        sbmesh sbm;
        sbm.indices = loader.m_mesh.indices[i];
        m_sbmeshes.PushBack(sbm);
    }
    m_world = rt_matrix4(1);
    m_world= rtm_rotate(D3DX_PI,0.0f,0.0f,1.0f)*m_world;
    m_world= m_world* rtm_rotate(0.5f*D3DX_PI,1.0f,0.0f,0.0f);


    printf("final grid cnt%i \n",(m_gridcount *8*+6));
    RT_HR_T(RT_D3D->GetD3dDevice()->CreateVertexBuffer( (m_gridcount *8*+6)* sizeof(rt_vertex_line),
                                                        D3DUSAGE_WRITEONLY,0,
                                                        D3DPOOL_MANAGED, &m_line_vb,0));
    rt_vertex_line* lv = 0;
    RT_HR_T(m_line_vb->Lock(0, 0, (void**)&lv, 0));
    GenGrid(lv,m_gridcount);
    RT_HR_T(m_line_vb->Unlock());


    RT_HR_T(RT_D3D->GetD3dDevice()->CreateVertexBuffer(m_vertices.Size() * sizeof(rt_vertex_),
                                                       D3DUSAGE_WRITEONLY,0,
                                                       D3DPOOL_MANAGED, &m_vb,0));
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
        m_sbmeshes[i].mat.diffuse = rt_vector3(0.2f, 0.2f, 0.4f);
        m_sbmeshes[i].mat.specular = rt_vector3(0.2f, 0.2f, 0.8f);
//        m_sbmeshes[i].mat.specPower = 100.0f;

    }
    RT_HR_T(D3DXCreateTextureFromFile(RT_D3D->GetD3dDevice(),"wht_tx.bmp", &m_tex));


    AddFlags(RT_MESH_INIT);

    return true;
}

void info_object::Update(){

    if(RT_EVENT.isevent){
        switch( RT_EVENT.msg ) {
        case WM_KEYUP : {
            if(RT_EVENT.wparam == 0x48){
                if(FlagTest(RT_MESH_HGRID)){ RemoveFlags(RT_MESH_HGRID); }
                else { AddFlags(RT_MESH_HGRID); }
            }
            if((RT_EVENT.wparam == 0x56) && !RT_WIN->FlagTest(RT_WIN_LCTRL)){
                if(FlagTest(RT_MESH_VGRID)){ RemoveFlags(RT_MESH_VGRID); }
                else { AddFlags(RT_MESH_VGRID); }
            }

        }break;

        }
    }
}
void info_object::Delete(){

    RELEASECOM(m_vb);
    RELEASECOM(m_tex);
    RELEASECOM(m_line_vb);
    for(uint16_t i=0;i<m_sbmeshes.Size();i++) { RELEASECOM(m_sbmeshes[i].ib); }
    printf("delete info object \n");

}
void info_object::Draw(){
    static rt_matrix4 MVP;


    if(RT_D3D->FlagTest(RT_D3D_DEVICE_LOST)){ return; }


    MVP = RT_D3D->Projection() * RT_D3D->View();

    if(FlagTest(RT_MESH_HGRID) || FlagTest(RT_MESH_VGRID)){
        RT_HR_E(RT_D3D->GetEffect()->SetMatrix(RT_D3D->Sdata().m_hmvp,(D3DXMATRIX*) &MVP));

        RT_HR_E(RT_D3D->GetEffect()->SetTechnique(RT_D3D->Sdata().m_hlinetech));
        RT_HR_E(RT_D3D->GetD3dDevice()->SetVertexDeclaration(RT_D3D->GetLineDec()));
        RT_HR_E(RT_D3D->GetD3dDevice()->SetStreamSource(0,m_line_vb, 0, sizeof(rt_vertex_line)));

        RT_HR_E(RT_D3D->GetEffect()->Begin(NULL, 0));
        RT_HR_E(RT_D3D->GetEffect()->BeginPass(0));
        if(FlagTest(RT_MESH_HGRID)){
            RT_D3D->GetD3dDevice()->DrawPrimitive(D3DPT_LINELIST,0, (m_gridcount *4*2+4)/2 );
        }
        if(FlagTest(RT_MESH_VGRID)){
            RT_D3D->GetD3dDevice()->DrawPrimitive(D3DPT_LINELIST,(m_gridcount *4*2+2), (m_gridcount *4*2+4)/2 );
        }
        RT_HR_E(RT_D3D->GetEffect()->EndPass());
        RT_HR_E(RT_D3D->GetEffect()->End());

    }
    MVP = MVP * m_world;
    RT_HR_E(RT_D3D->GetEffect()->SetMatrix(RT_D3D->Sdata().m_hmvp,(D3DXMATRIX*) &MVP));
    RT_HR_E(RT_D3D->GetEffect()->SetTechnique(RT_D3D->Sdata().m_htech));
    RT_HR_E(RT_D3D->GetD3dDevice()->SetVertexDeclaration(RT_D3D->GetDec()));
    RT_HR_E(RT_D3D->GetD3dDevice()->SetStreamSource(0,m_vb, 0, sizeof(rt_vertex_)));
    RT_HR_E(RT_D3D->GetEffect()->SetMatrix(RT_D3D->Sdata().m_hmodel, (D3DXMATRIX*)&m_world));



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

