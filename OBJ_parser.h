#include <string.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct{
    float x,y;
}Vec2;

typedef struct{
    float x,y,z;
}Vec3;

typedef struct{
    Vec3    *v; //vertice position
    Vec2    *vt; //texture coordinates
    Vec3    *vn; //vertice normal
    unsigned int    nb_v; //number of vertices
    unsigned int    nb_vt;
    unsigned int    nb_vn;
}Vertices;

typedef struct{
    char    **mtl_name; //material name
    Vec3    *Ka; //
    Vec3    *Kd; //
    Vec3    *Ks; //
    unsigned int     *illum; //
    float   *Ns;
    float   *Ni;
    unsigned int nb_mtl; //number of materials
}Materials;

typedef struct{
    unsigned int    *v_idx; //vertice index stored in vertices list that compose the face
    unsigned int    *vt_idx;
    unsigned int    *vn_idx;
    unsigned int    mtl_idx; //material index that composes the face
    unsigned int    nb_v; //number of vertices in the face
    unsigned int    nb_vt;
    unsigned int    nb_vn;
}Faces;

typedef struct{
    char    *mesh_name;
    Vertices    v;
    Materials   mtl;
    Faces   f;
}Mesh;

void AddVertice(Vertices *vlist, Vec3 v){
    vlist->v = realloc(vlist->v, (vlist->nb_v + 1) * sizeof(Vec3));
    vlist->v[vlist->nb_v] = v;
    vlist->nb_v++;
}

void AddTexture(Vertices *vlist, Vec2 vt) {
    vlist->vt = realloc(vlist->vt, (vlist->nb_vt + 1) * sizeof(Vec2));
    vlist->vt[vlist->nb_vt] = vt;
    vlist->nb_vt++;
}

void AddNormal(Vertices *vlist, Vec3 vn) {
    vlist->vn = realloc(vlist->vn, (vlist->nb_vn + 1) * sizeof(Vec3));
    vlist->vn[vlist->nb_vn] = vn;
    vlist->nb_vn++;
}

void AddFace(Faces *flist, unsigned int *v_idx, unsigned int *vt_idx, unsigned int *vn_idx, unsigned int nb_v, unsigned int mtl_idx) {
    flist->v_idx = realloc(flist->v_idx, (flist->nb_v + nb_v) * sizeof(unsigned int));
    flist->vt_idx = realloc(flist->vt_idx, (flist->nb_v + nb_v) * sizeof(unsigned int));
    flist->vn_idx = realloc(flist->vn_idx, (flist->nb_v + nb_v) * sizeof(unsigned int));

    for (unsigned int i = 0; i < nb_v; i++) {
        flist->v_idx[flist->nb_v + i]  = v_idx[i];
        flist->vt_idx[flist->nb_v + i] = vt_idx[i];
        flist->vn_idx[flist->nb_v + i] = vn_idx[i];
    }

    flist->mtl_idx = mtl_idx; // Une face = un matériau
    flist->nb_v += nb_v;
}

void AddKa(Materials *mlist, Vec3 Ka) {
    mlist->Ka = realloc(mlist->Ka, (mlist->nb_mtl + 1) * sizeof(Vec3));
    mlist->Ka[mlist->nb_mtl] = Ka;
}

void AddKd(Materials *mlist, Vec3 Kd) {
    mlist->Kd = realloc(mlist->Kd, (mlist->nb_mtl + 1) * sizeof(Vec3));
    mlist->Kd[mlist->nb_mtl] = Kd;
}

void AddKs(Materials *mlist, Vec3 Ks) {
    mlist->Ks = realloc(mlist->Ks, (mlist->nb_mtl + 1) * sizeof(Vec3));
    mlist->Ks[mlist->nb_mtl] = Ks;
}

void AddNs(Materials *mlist, float Ns) {
    mlist->Ns = realloc(mlist->Ns, (mlist->nb_mtl + 1) * sizeof(float));
    mlist->Ns[mlist->nb_mtl] = Ns;
}

void AddNi(Materials *mlist, float Ni) {
    mlist->Ni = realloc(mlist->Ni, (mlist->nb_mtl + 1) * sizeof(float));
    mlist->Ni[mlist->nb_mtl] = Ni;
}

void AddIllum(Materials *mlist, unsigned int illum) {
    mlist->illum = realloc(mlist->illum, (mlist->nb_mtl + 1) * sizeof(unsigned int));
    mlist->illum[mlist->nb_mtl] = illum;
}



Materials loadMTL(const char* MTL_path){
    char line[256];
    char line_clone[256];
    Materials tmp;
    FILE *file=fopen(MTL_path,"r");
    if(file==NULL){printf("File '%s' not found\n",MTL_path);return tmp;}

    Vec3 v3_buffer;
    while (fgets(line,sizeof(line),file)!=NULL){
        strcpy(line_clone,line);
        char *syntax=strtok(line_clone," ");
        if (!strcmp(syntax,"#")){
            printf("Commentaire: %s\n",line+1);
            continue;
        }else if (!strcmp(syntax,"newmtl")){
            *(tmp.mtl_name)=strdup(line+7);
            printf("Nom: %s\n",*(tmp.mtl_name));
        }else if (!strcmp(syntax,"Ka")){
            sscanf(line,"Ka %f %f %f",&v3_buffer.x,&v3_buffer.y,&v3_buffer.z);
            AddKa(&tmp,v3_buffer);
            printf("Ka %f %f %f\n",v3_buffer.x,v3_buffer.y,v3_buffer.z);
        }else if (!strcmp(syntax,"Kd")){
            sscanf(line,"Kd %f %f %f",&v3_buffer.x,&v3_buffer.y,&v3_buffer.z);
            AddKd(&tmp,v3_buffer);
            printf("Kd %f %f %f\n",v3_buffer.x,v3_buffer.y,v3_buffer.z);
        }else if (!strcmp(syntax,"Ks")){
            sscanf(line,"Kd %f %f %f",&v3_buffer.x,&v3_buffer.y,&v3_buffer.z);
            AddKd(&tmp,v3_buffer);
            printf("Kd %f %f %f\n",v3_buffer.x,v3_buffer.y,v3_buffer.z);
        }
    }
    return tmp;
}

Mesh loadOBJ(const char* OBJ_path){
    char line[256];
    char line_clone[256];
    char *path;
    char mtl_path[256];
    Mesh tmp;
    tmp.mesh_name = NULL;
    tmp.v.v = NULL;
    tmp.v.vt = NULL;
    tmp.v.vn = NULL;
    tmp.v.nb_v = 0;
    tmp.v.nb_vt = 0;
    tmp.v.nb_vn = 0;

    tmp.mtl.nb_mtl = 0;
    tmp.mtl.Ka = NULL;
    tmp.mtl.Kd = NULL;
    tmp.mtl.Ks = NULL;
    tmp.mtl.illum = NULL;
    tmp.mtl.Ns = NULL;
    tmp.mtl.Ni = NULL;
    for (int i=0; i<256; i++)
        tmp.mtl.mtl_name[i] = NULL;

    tmp.f.v_idx = NULL;
    tmp.f.vt_idx = NULL;
    tmp.f.vn_idx = NULL;
    tmp.f.nb_v = 0;
    tmp.f.nb_vt = 0;
    tmp.f.nb_vn = 0;
    tmp.f.mtl_idx = 0;
    FILE *file=fopen(OBJ_path,"r");
    if (file==NULL){printf("File '%s' not found\n",OBJ_path);return tmp;}
    Vec3 v3_buffer;
    Vec2 v2_buffer;
    int v_idx[3];
    int vt_idx[3];
    int vn_idx[3];
    int current_mtl_idx=0;
    while (fgets(line,sizeof(line),file)!=NULL){
        strcpy(line_clone,line);
        char *syntax=strtok(line_clone," ");
        if (!strcmp(syntax,"#")){
            printf("Commentaire: %s\n",line+1);
            continue;
        }else if (!strcmp(syntax,"o")){
            tmp.mesh_name=strdup(line+2);
            printf("Nom: %s\n",tmp.mesh_name);
        }else if (!strcmp(syntax,"v")){
            sscanf(line,"v %f %f %f",&v3_buffer.x,&v3_buffer.y,&v3_buffer.z);
            AddVertice(&tmp.v,v3_buffer);
            printf("v %f %f %f\n",v3_buffer.x,v3_buffer.y,v3_buffer.z);
        }else if (!strcmp(syntax,"vt")){
            sscanf(line,"vt %f %f",&v2_buffer.x,&v2_buffer.y);
            AddTexture(&tmp.v,v2_buffer);
            printf("vt %f %f\n",v2_buffer.x,v2_buffer.y);
        }else if (!strcmp(syntax,"vn")){
            sscanf(line,"vt %f %f %f",&v3_buffer.x,&v3_buffer.y,&v3_buffer.z);
            AddNormal(&tmp.v,v3_buffer);
            printf("vt %f %f %f\n",v3_buffer.x,v3_buffer.y,v3_buffer.z);
        }else if (!strcmp(syntax,"usemtl")){
            sscanf(line,"usemtl %s",mtl_path);
            path=strrchr(OBJ_path,'/');
            if (path){
                strcpy(path+1,mtl_path);
                //LoadMTL(path);
                printf("usemtl %s\n",path);
            }else {
                //LoadMTL(mtl_path);
                printf("usemtl %s\n",mtl_path);
            }
        }else if (!strcmp(syntax,"f")){
            sscanf(line,"f %d/%d/%d %d/%d/%d %d/%d/%d",
                &v_idx[0],&vt_idx[0],&vn_idx[0],
                &v_idx[1],&vt_idx[1],&vn_idx[1],
                &v_idx[2],&vt_idx[2],&vn_idx[2]);
            for(int i=0;i<3;i++){
                v_idx[i]  -= 1;
                vt_idx[i] -= 1;
                vn_idx[i] -= 1;
            }
            AddFace(&tmp.f,v_idx,vt_idx,vn_idx,3,current_mtl_idx);
            printf("f %d/%d/%d %d/%d/%d %d/%d/%d\n",
                v_idx[0],vt_idx[0],vn_idx[0],
                v_idx[1],vt_idx[1],vn_idx[1],
                v_idx[2],vt_idx[2],vn_idx[2]);
        }
    }
    return tmp;
}