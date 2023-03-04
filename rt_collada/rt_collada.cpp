#include "rt_collada.h"
#include "tinyxml2.h"

void ReadInput(Input* out,tinyxml2::XMLElement * elem){
    out->semantic = elem->Attribute("semantic");
    const char* SRC = elem->Attribute("source");
    if(++SRC){ out->source = SRC; }
    const char* OFFSET = elem->Attribute("offset");
    if(OFFSET) { out->offset = atoi( OFFSET ); }
}
bool ReadSource(Source * src, tinyxml2::XMLElement * elem){

    src->id = elem->Attribute("id");
    elem = elem->FirstChildElement();
    while(elem){
        bool farr = RT_SCMP(elem->Name(),"float_array");
        bool narr = RT_SCMP(elem->Name(),"Name_array");
        if(farr || narr){
            StrSplit(elem->GetText(),&src->narray);
            if(farr){ S2Farray(src->narray,&src->farray); }
        }
        else if(RT_SCMP(elem->Name(),"technique_common")){
            tinyxml2::XMLElement * tcc = elem->FirstChildElement();
            src->acccount = atoi( tcc->Attribute("count") );
            src->accstride = atoi( tcc->Attribute("stride") );
            tcc = tcc->FirstChildElement();
            while(tcc){
                src->params.PushBack( Param(tcc->Attribute("name"),tcc->Attribute("type")) );
                tcc = tcc->NextSiblingElement();
            }
        }
        elem = elem->NextSiblingElement();
    }
    return true;
}

template<typename T>
int16_t findSource(const T& sources,const char *id){
    for(uint16_t i=0;i<sources.Size();i++){ if( RT_SCMP(id,sources[i].id.Data()) ){ return i; } }
    return -1;
}
template<typename T,typename T2 >
void loadsrc( T * dest, T2*src,uint8_t cnt=3){
    for(uint32_t inx=0,j=0;j<src->acccount;j++){
        for(uint32_t jj=0;jj<cnt;jj++){ dest[j][jj]= src->farray[inx++]; }
    }
}
bool rt_collada::Load(const char * file){

    tinyxml2::XMLDocument doc;
    if(doc.LoadFile(file) != tinyxml2::XML_SUCCESS){ RT_THROW("xml doc"); }

    tinyxml2::XMLElement * element = doc.FirstChildElement();
    if(!RT_SCMP(element->Name(),"COLLADA")){ RT_THROW("INV"); }
    element=element->FirstChildElement();

    while(element){
        if(RT_SCMP(element->Name(),"library_geometries")){
            if(!ReadGeometry(element)) { element = NULL; break;}
        }
        if(RT_SCMP(element->Name(),"library_controllers")){ ReadController(element); }
        if(RT_SCMP(element->Name(),"library_visual_scenes")){ ReadVisualScene(element); }
        if(RT_SCMP(element->Name(),"library_animations")){ ReadAnimations(element); }
        element = element->NextSiblingElement();
    }
    if(m_plists.Size()==0 ){ RT_THROW("geo"); }

    return LoadData();
}
bool rt_collada::ReadGeometry(tinyxml2::XMLElement * element){

    tinyxml2::XMLElement * geo = element->FirstChildElement();
    if(!RT_SCMP(geo->Name(),"geometry") || geo->NextSiblingElement() ){ RT_THROW("xml element"); }
    geo= geo->FirstChildElement()->FirstChildElement();

    while(geo){
        if(RT_SCMP("source",geo->Name())){
            Source src;
            if(ReadSource(&src,geo)){ m_sources.PushBack(src); }
        }
        if(RT_SCMP("vertices",geo->Name())){
            const char * SRC = geo->FirstChildElement()->Attribute("source");
            if(! (++SRC) ){ RT_THROW("Attribute"); }
            m_vertsrc = SRC;
        }
        if(RT_SCMP("polylist",geo->Name())){
            tinyxml2::XMLElement * pliste = geo->FirstChildElement();
            Polylist plist;
            while(pliste){
                if(RT_SCMP("input",pliste->Name())){
                    Input inpt;
                    ReadInput(&inpt,pliste);
                    plist.inputs.PushBack(inpt);
                }
                if( RT_SCMP("vcount",pliste->Name()) ) { Text2Iarray(pliste->GetText(),& plist.vcount); }
                if( RT_SCMP("p",pliste->Name()) ) { Text2Iarray(pliste->GetText(),& plist.p); }
                pliste = pliste->NextSiblingElement();
            }
            m_plists.PushBack(plist);
        }
        geo = geo->NextSiblingElement();
    }
    return true;
}

void rt_collada::ReadController(tinyxml2::XMLElement * element){

    tinyxml2::XMLElement * ctrl = element->FirstChildElement();
    if(!ctrl){ return; }
    if(!RT_SCMP(ctrl->Name(),"controller") || ctrl->NextSiblingElement() ){ RT_ERROR("xml element"); }

    ctrl= ctrl->FirstChildElement()->FirstChildElement();
    while(ctrl){
        if(RT_SCMP("source",ctrl->Name())){
            Source src;
            if(ReadSource(&src,ctrl)){ m_sources.PushBack(src); }
        }
        if(RT_SCMP("bind_shape_matrix",ctrl->Name())){
            rt_farray bsm;
            Text2Farray(ctrl->GetText(),&bsm);
            m_mesh.bsm = ToMat((float*)bsm.Data());
        }
        if(RT_SCMP("joints",ctrl->Name())){
            tinyxml2::XMLElement * jnts = ctrl->FirstChildElement();
            while(jnts){
                const char* SEM = jnts->Attribute("semantic");
                if(RT_SCMP("INV_BIND_MATRIX",SEM)){
                    const char* SRC = jnts->Attribute("source");
                    m_controller.ibsrc=++SRC;
                }
                jnts=jnts->NextSiblingElement();
            }
        }
        if(RT_SCMP("vertex_weights",ctrl->Name())){
            tinyxml2::XMLElement * vwts = ctrl->FirstChildElement();
            while(vwts){
                if(RT_SCMP("input",vwts->Name())){
                    const char* SEM = vwts->Attribute("semantic");
                    rt_string16 * srcstr = RT_SCMP("WEIGHT",SEM)?&m_controller.wsrc:&m_controller.jsrc;
                    const char* SRC = vwts->Attribute("source");
                    (*srcstr) = ++SRC;
                }
                if(RT_SCMP("vcount",vwts->Name())) { Text2Iarray(vwts->GetText(),&m_controller.vcount); }
                if(RT_SCMP("v",vwts->Name())) { Text2Iarray(vwts->GetText(),&m_controller.v); }
                vwts=vwts->NextSiblingElement();
            }
        }
        ctrl = ctrl->NextSiblingElement();
    }
}

void rt_collada::ReadVisualScene(tinyxml2::XMLElement * element){

    tinyxml2::XMLElement * nodes = element->FirstChildElement()->FirstChildElement();
    if(!nodes ){ RT_ERROR("xml element"); }

    bool pass(false);
    rt_string16 skel;
    while(nodes){
        if(RT_SCMP(nodes->Name(),"node")){
            tinyxml2::XMLElement * icntrl = nodes->FirstChildElement();
            while(icntrl){
                if(skel.Size()){
                    if(RT_SCMP(icntrl->Name(),"node")){
                        if(RT_SCMP(skel.Data(),icntrl->Attribute("id"))){
                            m_mesh.node_root = new rt_node();
                            m_mesh.node_root->name = "_ROOT_";
                            ReadNode(icntrl,m_mesh.node_root);
                        }
                    }
                }else{
                    if(RT_SCMP(icntrl->Name(),"instance_controller")){
                        const char* SKEL =icntrl->FirstChildElement()->GetText();
                        if(!++SKEL) { RT_ERROR("skeleton"); return;}
                        skel=SKEL;
                        nodes = element->FirstChildElement()->FirstChildElement();
                        pass = true;
                    }
                }
                icntrl=icntrl->NextSiblingElement();
            }
        }
        if(!pass) { nodes=nodes->NextSiblingElement(); }
        else{ pass = false; }

    }
}
void  rt_collada::ReadNode(tinyxml2::XMLElement * element,rt_node*parent){

    rt_node * node_ = new rt_node();
    node_->name = element->Attribute("id");
    element=element->FirstChildElement();
    while(element){
        if(RT_SCMP(element->Name(),"matrix")){
            Text2Farray(element->GetText(),&node_->transform);
            parent->AddChild(node_);
        }
        if(RT_SCMP(element->Name(),"node")){ ReadNode(element,node_); }
        element = element->NextSiblingElement();
    }
}
void rt_collada::ReadAnimations(tinyxml2::XMLElement * element){

    element = element->FirstChildElement();
    while(element){
        tinyxml2::XMLElement * anim = element->FirstChildElement();
        while(anim){
            if(RT_SCMP("source",anim->Name())){
                Source src;
                if(ReadSource(&src,anim)){ m_sources.PushBack(src); }
            }
            if(RT_SCMP("sampler",anim->Name())){
                tinyxml2::XMLElement * sampler_ = anim->FirstChildElement();
                anim_sampler sampler;
                while(sampler_){
                    const char* SRC = sampler_->Attribute("source");
                    if(RT_SCMP("INPUT",sampler_->Attribute("semantic"))) { sampler.isrc = ++SRC; }
                    if(RT_SCMP("OUTPUT",sampler_->Attribute("semantic"))) { sampler.osrc = ++SRC; }
                    sampler_ = sampler_->NextSiblingElement();
                }
                m_anims.PushBack(sampler);
            }
            anim=anim->NextSiblingElement();
        }
        element=element->NextSiblingElement();
    }
}

bool rt_collada::LoadData(){

    rt_vector4 *vertw(NULL);
    rt_bvector4 *verti(NULL);
    rt_vector2 *uvs(NULL);
    rt_vector3 *verts(NULL),*norms(NULL);

    int16_t vinx(-1),ninx(-1),tinx(-1);

    for(uint16_t ii=0; ii<m_plists[0].inputs.Size();ii++){
        if( RT_SCMP("VERTEX",m_plists[0].inputs[ii].semantic.Data()) ){
            vinx = findSource(m_sources,m_vertsrc.Data());
            verts = new rt_vector3[m_sources[vinx].acccount];
            loadsrc(verts,&m_sources[vinx]);
        }
        else if( RT_SCMP("NORMAL",m_plists[0].inputs[ii].semantic.Data()) ){
            ninx = findSource(m_sources,m_plists[0].inputs[ii].source.Data());
            norms = new rt_vector3[m_sources[ninx].acccount];
            loadsrc(norms,&m_sources[ninx]);
        }
        else if( RT_SCMP("TEXCOORD",m_plists[0].inputs[ii].semantic.Data()) ){
            tinx = findSource(m_sources,m_plists[0].inputs[ii].source.Data());
            uvs = new rt_vector2[m_sources[tinx].acccount];
            loadsrc(uvs,&m_sources[tinx],2);
            for(uint32_t j=0;j<m_sources[tinx].acccount; uvs[j].y=1.0f-uvs[j].y, j++);
        }
        else { RT_THROW( (rt_string(m_plists[0].inputs[ii].semantic.Data())+rt_string(" not supported")).Data() ); }
    }
    if(vinx<0){ RT_THROW("vertex source"); }

    if(m_controller.vcount.Size()){
        int sinx = findSource(m_sources,m_controller.jsrc.Data());
        m_mesh.jntn = m_sources[sinx].narray;

        sinx = findSource(m_sources,m_controller.ibsrc.Data());
        int ivbsz = m_sources[sinx].farray.Size()/16;
        m_mesh.ibm.Allocate(ivbsz);
        for(int i=0;i<ivbsz;i++){ m_mesh.ibm[i]= ToMat( &(m_sources[sinx].farray[i*16]) ); }

        sinx = findSource(m_sources,m_controller.wsrc.Data());
        verti = new rt_bvector4[ m_controller.vcount.Size() ];
        vertw = new rt_vector4[ m_controller.vcount.Size() ];
        for(uint32_t icnt=0,v=0;v<m_controller.vcount.Size();v++){
            for(int32_t vv=0;vv<m_controller.vcount[v];vv++){
                verti[v][vv] = m_controller.v[icnt++];
                vertw[v][vv] = m_sources[sinx].farray[ m_controller.v[icnt++] ];
            }
        }
    }
    if(m_anims.Size() > 0){
        int sinx = findSource(m_sources,m_anims[0].isrc.Data());
        m_mesh.a_inputs = m_sources[sinx].farray;

        rt_aarray kframes;
        kframes.Allocate( m_anims.Size() );
        for(uint16_t i=0; i<m_anims.Size();i++){
            sinx = findSource(m_sources,m_anims[i].osrc.Data());
            rt_tarray kframe;
            kframe.Allocate(m_sources[sinx].acccount);
            for(uint16_t ii=0;ii<m_sources[sinx].acccount;ii++){ kframe[ii]= ToMat( &(m_sources[sinx].farray[ii*16]) ); }
            kframes[i] = kframe;
        }
        m_mesh.anims.Allocate( kframes[0].Size() );
        for(uint16_t i=0; i<kframes[0].Size(); i++){
            rt_tarray frame;
            for(uint16_t ii=0; ii<kframes.Size(); ii++){ frame.PushBack(kframes[ii][i]); }
            m_mesh.anims[i] = frame;
        }
    }
    for(uint32_t i=0; i<m_plists.Size();i++){
        rt_array<uint16_t> indices;
        for(uint32_t icnt=0,ii=0; ii<m_plists[i].vcount.Size();ii++){
            for(int32_t vc_=0; vc_<m_plists[i].vcount[ii]; vc_++){
                rt_vertex vert;
                for(uint32_t vc=0; vc<m_plists[i].inputs.Size();vc++){
                    const char* SEM = m_plists[i].inputs[vc].semantic.Data();
                    if(RT_SCMP("VERTEX",SEM)) {
                        if(verti && vertw){
                            vert.bw = vertw[ m_plists[i].p[icnt] ];
                            vert.bi = verti[ m_plists[i].p[icnt] ];
                        }
                        vert.pos = verts[ m_plists[i].p[icnt++] ];
                    }
                    if(RT_SCMP("NORMAL",SEM)) { vert.norm = norms[ m_plists[i].p[icnt++] ]; }
                    if(RT_SCMP("TEXCOORD",SEM)) { vert.uv = uvs[ m_plists[i].p[icnt++] ]; }
                }
                indices.PushBack(m_mesh.vertices.Size(),true);
                m_mesh.vertices.PushBack(vert,true);
            }
        }
        m_mesh.indices.PushBack(indices);
    }
    if( (m_mesh.jntn.Size()>0) || (m_mesh.anims.Size()>0) ){ m_mesh.GenFinalTransforms(); }
    m_mesh.Index();
    printf("collada loader start_____  \n");
    printf("%u-sources  \n",m_sources.Size());
    printf("%u-vertices \n",m_mesh.vertices.Size());
    printf("%u-sub meshes \n",m_mesh.indices.Size());
    for(uint16_t i=0;i<m_mesh.indices.Size();i++){
        printf("%u-sub meshe %u-indecies \n",i+1,m_mesh.indices[i].Size());
    }
    if(m_mesh.jntn.Size()) { printf("%u-bones \n",m_mesh.jntn.Size()); }
    if(m_mesh.anims.Size()) { printf("%u-keyframes \n",m_mesh.anims.Size()); }
    printf("collada loader end_____  \n\n");
    return true;
}

void rtc_mesh::ReverseIndices(){
    rt_array< rt_array<uint16_t> , uint16_t > t_indices = indices;
    indices.Clear();
    for(uint32_t i=0;i<t_indices.Size();i++){
        rt_array<uint16_t> sbm;
        sbm.Allocate( t_indices[i].Size() );
        for(int32_t in=0,ii=t_indices[i].Size()-1;ii>=0; in++,ii--){
            sbm[in] = t_indices[i][ii];
        }
        indices.PushBack(sbm,true);
    }
}

void rtc_mesh::GenFinalTransforms(){
    if(jntn.Size()){
        finaljointtransforms.Allocate(jntn.Size());
        GenFinalTransforms(false,node_root,&finaljointtransforms);
    }
    if(anims.Size()){
        finalanimstransforms = anims;
        for(uint16_t i=0;i<anims.Size();i++){
            GenFinalTransforms(true,node_root,&finalanimstransforms[i]);
        }
    }
}
void rtc_mesh::GenFinalTransforms(bool isanim, rt_node * node, rt_tarray* destbones, rt_matrix4 parent){
    rt_matrix4 T;
    if(RT_SCMP("_ROOT_",node->name.Data())){ T = parent; }
    else{
        int inx;
        for(uint16_t i=0;i<jntn.Size();i++){
            if( RT_SCMP(jntn[i].Data(),node->name.Data()) ){ inx = i; break;}
        }
        if(isanim) { T = (*destbones)[inx]; }
        else { T = ToMat((float*)node->transform.Data()); }
        T = parent*T;
        (*destbones)[inx] = T*ibm[inx]*bsm;
    }
    for(uint16_t i=0; i<node->ccnt;i++){ GenFinalTransforms(isanim,node->c[i],destbones,T); }
}

#define INX_EPSILON 0.01f
bool vert_test( const rt_vertex& v1,const rt_vertex& v2){
    if(!(abs(v1.pos.x-v2.pos.x) <= INX_EPSILON) ){ return false; }
    if(!(abs(v1.pos.y-v2.pos.y) <= INX_EPSILON) ){ return false; }
    if(!(abs(v1.pos.z-v2.pos.z) <= INX_EPSILON) ){ return false; }

    if(!(abs(v1.norm.x-v2.norm.x) <= INX_EPSILON) ){ return false; }
    if(!(abs(v1.norm.y-v2.norm.y) <= INX_EPSILON) ){ return false; }
    if(!(abs(v1.norm.z-v2.norm.z) <= INX_EPSILON) ){ return false; }

    if(!(abs(v1.uv.x-v2.uv.x) <= INX_EPSILON) ){ return false; }
    if(!(abs(v1.uv.y-v2.uv.y) <= INX_EPSILON) ){ return false; }
    return true;
}

inline int32_t findvert(const rt_vertex& v,const rt_array< rt_vertex >& vertices){
    for(uint16_t i=0; i<vertices.Size(); i++){
        if(vert_test(v,vertices[i])) { return i; }
    }
    return -1;
}

void rtc_mesh::Index(){
    rt_array< rt_vertex > i_vertices = vertices;
    vertices.Clear();
    for(uint16_t i=0; i < indices.Size(); i++ ){
        for(uint16_t ii=0; ii < indices[i].Size(); ii++ ){

            int32_t inx = findvert(i_vertices[indices[i][ii]],vertices);
            if(inx<0){
                vertices.PushBack(i_vertices[indices[i][ii]],true);
                indices[i][ii] = vertices.Size()-1;
            }else{
                indices[i][ii] = inx;
            }
        }

    }




}
