#include "tnt_wonder_hall_vita.h"

#include <math.h>
#include <stdint.h>
#include <string.h>

#define HALL_NATIVE_W 400
#define HALL_NATIVE_H 300
#define DEG2RAD (3.14159265358979323846f / 180.0f)

typedef struct { f32 m[4][4]; } HallMatrix;
typedef struct { f32 x,y,z,u,v,shade; } HallPV;

static f32 s_q[HALL_NATIVE_W * HALL_NATIVE_H];
static u16 s_stamp[HALL_NATIVE_W * HALL_NATIVE_H];
static u16 s_frame_stamp;
static uint32_t s_color[HALL_NATIVE_W * HALL_NATIVE_H];
static f32 s_tex_rgb[1024][3];
static u8 s_tex_ready;
static f32 s_finale_tex_rgb[1024][3];
static u8 s_finale_tex_ready;
static s16 s_map_x[960], s_map_y[544];
static s32 s_map_bx=0x7fffffff,s_map_by=0x7fffffff,s_map_cell=-1,s_map_w=-1,s_map_h=-1;

static void ident(HallMatrix *m){memset(m,0,sizeof(*m));m->m[0][0]=m->m[1][1]=m->m[2][2]=m->m[3][3]=1.0f;}
static void mult(HallMatrix*out,const HallMatrix*a,const HallMatrix*b){HallMatrix r;s32 i,j,k;for(i=0;i<4;i++)for(j=0;j<4;j++){f32 v=0;for(k=0;k<4;k++)v+=a->m[i][k]*b->m[k][j];r.m[i][j]=v;}*out=r;}
static f32 vlen(f32 x,f32 y,f32 z){return sqrtf(x*x+y*y+z*z);}
static void norm3(f32*x,f32*y,f32*z){f32 n=vlen(*x,*y,*z);if(n>0.000001f){*x/=n;*y/=n;*z/=n;}}

static void type2(const f32*v,HallMatrix*m){
    f32 sx=sinf(-v[0]*DEG2RAD),cx=cosf(-v[0]*DEG2RAD);
    f32 sy=sinf(-v[1]*DEG2RAD),cy=cosf(-v[1]*DEG2RAD);
    f32 sz=sinf(-v[2]*DEG2RAD),cz=cosf(-v[2]*DEG2RAD);
    ident(m);
    m->m[0][0]=(cz*cx)*v[6];m->m[0][1]=((sz*cy)+(cz*sx*sy))*v[7];m->m[0][2]=((sz*sy)-(cz*sx*cy))*v[8];
    m->m[1][0]=(-sz*cx)*v[6];m->m[1][1]=((cz*cy)-(sz*sx*sy))*v[7];m->m[1][2]=((cz*sy)+(sz*sx*cy))*v[8];
    m->m[2][0]=sx*v[6];m->m[2][1]=(-cx*sy)*v[7];m->m[2][2]=(cx*cy)*v[8];
    m->m[0][3]=v[3];m->m[1][3]=v[4];m->m[2][3]=v[5];
}

/* func_800A53E0 + func_800AE880 for scene1071 camera. Also returns the
 * camera right/up basis used by the source's guLookAtReflect texture-gen path. */
static void camera_view(HallMatrix *view,f32 right[3],f32 up[3]){
    const f32*v=g_tnt_wonder_locked_nodes[0];
    f32 ex=v[0],ey=v[1],ez=v[2],fx=ex-v[3],fy=ey-v[4],fz=ez-v[5];
    f32 rx,ry,rz,ux,uy,uz,rc,rs,rrx,rry,rrz,rux,ruy,ruz;
    if(fx==0&&fy==0&&fz==0)fz=-1;
    norm3(&fx,&fy,&fz);rx=fz;ry=0;rz=-fx;norm3(&rx,&ry,&rz);
    ux=fy*rz-ry*fz;uy=fz*rx-rz*fx;uz=fx*ry-rx*fy;rc=cosf(v[6]*DEG2RAD);rs=sinf(v[6]*DEG2RAD);
    rrx=rx*rc+ux*rs;rry=ry*rc+uy*rs;rrz=rz*rc+uz*rs;rux=-rx*rs+ux*rc;ruy=-ry*rs+uy*rc;ruz=-rz*rs+uz*rc;
    right[0]=rrx;right[1]=rry;right[2]=rrz;up[0]=rux;up[1]=ruy;up[2]=ruz;
    ident(view);view->m[0][0]=rrx;view->m[0][1]=rry;view->m[0][2]=rrz;view->m[1][0]=rux;view->m[1][1]=ruy;view->m[1][2]=ruz;view->m[2][0]=fx;view->m[2][1]=fy;view->m[2][2]=fz;
    ex*=16.0f;ey*=16.0f;ez*=16.0f;view->m[0][3]=-(rrx*ex+rry*ey+rrz*ez);view->m[1][3]=-(rux*ex+ruy*ey+ruz*ez);view->m[2][3]=-(fx*ex+fy*ey+fz*ez);
}
static void point(const HallMatrix*m,f32 x,f32 y,f32 z,f32*ox,f32*oy,f32*oz){*ox=m->m[0][0]*x+m->m[0][1]*y+m->m[0][2]*z+m->m[0][3];*oy=m->m[1][0]*x+m->m[1][1]*y+m->m[1][2]*z+m->m[1][3];*oz=m->m[2][0]*x+m->m[2][1]*y+m->m[2][2]*z+m->m[2][3];}
static void vector(const HallMatrix*m,f32 x,f32 y,f32 z,f32*ox,f32*oy,f32*oz){*ox=m->m[0][0]*x+m->m[0][1]*y+m->m[0][2]*z;*oy=m->m[1][0]*x+m->m[1][1]*y+m->m[1][2]*z;*oz=m->m[2][0]*x+m->m[2][1]*y+m->m[2][2]*z;}
static void prep_tex(void){s32 i;if(s_tex_ready)return;for(i=0;i<1024;i++){u16 p=g_tnt_wonder_locked_texture[i];s_tex_rgb[i][0]=(f32)((p>>11)&31)*(255.0f/31.0f);s_tex_rgb[i][1]=(f32)((p>>6)&31)*(255.0f/31.0f);s_tex_rgb[i][2]=(f32)((p>>1)&31)*(255.0f/31.0f);}s_tex_ready=1;}
static void sample(f32 u,f32 v,f32*r,f32*g,f32*b){s32 x0,y0,x1,y1;f32 fx,fy;const f32*c00,*c10,*c01,*c11;if(u<0)u=0;if(v<0)v=0;if(u>31)u=31;if(v>31)v=31;x0=(s32)u;y0=(s32)v;x1=x0<31?x0+1:x0;y1=y0<31?y0+1:y0;fx=u-x0;fy=v-y0;c00=s_tex_rgb[y0*32+x0];c10=s_tex_rgb[y0*32+x1];c01=s_tex_rgb[y1*32+x0];c11=s_tex_rgb[y1*32+x1];*r=(c00[0]+(c10[0]-c00[0])*fx)*(1-fy)+(c01[0]+(c11[0]-c01[0])*fx)*fy;*g=(c00[1]+(c10[1]-c00[1])*fx)*(1-fy)+(c01[1]+(c11[1]-c01[1])*fx)*fy;*b=(c00[2]+(c10[2]-c00[2])*fx)*(1-fy)+(c01[2]+(c11[2]-c01[2])*fx)*fy;}
static s32 fdiv(s64 n,s64 d){if(n>=0)return(s32)(n/d);return-(s32)(((-n)+d-1)/d);}
static void prep_map(s32 bx,s32 by,s32 cell,s32 w,s32 h){s32 i;if(s_map_bx==bx&&s_map_by==by&&s_map_cell==cell&&s_map_w==w&&s_map_h==h)return;for(i=0;i<w&&i<960;i++)s_map_x[i]=(s16)(83+fdiv((s64)(i-bx)*11,cell));for(i=0;i<h&&i<544;i++)s_map_y[i]=(s16)(36+fdiv((s64)(i-by)*11,cell));s_map_bx=bx;s_map_by=by;s_map_cell=cell;s_map_w=w;s_map_h=h;}

void TNT_Vita_WonderRenderLocked(void *buffer,s32 stride,s32 sw,s32 sh,s32 bx,s32 by,s32 cell,u16 anim_tick){
    HallMatrix view,model,mv,local;f32 right[3],up[3],vals[9];uint32_t*dst=(uint32_t*)buffer;u16 fs=(u16)(s_frame_stamp+1);s32 i,y;f32 focal=1.0f/tanf((41.5394402f*0.5f)*DEG2RAD);
    prep_tex();prep_map(bx,by,cell,sw,sh);if(fs==0){memset(s_stamp,0,sizeof(s_stamp));fs=1;}s_frame_stamp=fs;camera_view(&view,right,up);
    memcpy(vals,g_tnt_wonder_locked_nodes[1],sizeof(vals));vals[0]=-1.40625f-0.703125f*(f32)(anim_tick%511U);type2(vals,&local);model=local;
    for(i=0;i<3;i++){model.m[i][0]*=0.125f;model.m[i][1]*=0.125f;model.m[i][2]*=0.125f;model.m[i][3]*=16.0f;}mult(&mv,&view,&model);
    for(i=0;i<236;i++){
        const TNTWonderTriangle*tri=&g_tnt_wonder_locked_triangles[i];HallPV p[3];f32 iz[3],area;s32 k,minx,maxx,miny,maxy;
        for(k=0;k<3;k++){
            const TNTWonderVertex*v=&tri->v[k];f32 cx,cy,cz,d,nx,ny,nz,wnx,wny,wnz,dot;
            point(&mv,v->x,v->y,v->z,&cx,&cy,&cz);d=-cz;if(d<2.0f||d>32000.0f)goto skip;
            p[k].x=200.0f+(cx*focal/(4.0f/3.0f)/d)*200.0f;p[k].y=150.0f-(cy*focal/d)*150.0f;p[k].z=d;iz[k]=1.0f/d;
            /* G_TEXTURE_GEN (without LINEAR): source guLookAtReflect supplies
             * camera-relative S/T look-at vectors. Generated RSP texture
             * coordinates span the full 32x32 texture as dot goes -1..+1. */
            vector(&local,(f32)v->nx/127.0f,(f32)v->ny/127.0f,(f32)v->nz/127.0f,&wnx,&wny,&wnz);norm3(&wnx,&wny,&wnz);
            p[k].u=(wnx*right[0]+wny*right[1]+wnz*right[2]+1.0f)*16.0f;
            p[k].v=(wnx*up[0]+wny*up[1]+wnz*up[2]+1.0f)*16.0f;
            vector(&local,(f32)v->nx/127.0f,(f32)v->ny/127.0f,(f32)v->nz/127.0f,&nx,&ny,&nz);norm3(&nx,&ny,&nz);dot=ny*(120.0f/127.0f);if(dot<0)dot=0;p[k].shade=0.5f+dot;if(p[k].shade>1)p[k].shade=1;
        }
        area=(p[1].x-p[0].x)*(p[2].y-p[0].y)-(p[1].y-p[0].y)*(p[2].x-p[0].x);if(area>=0)continue;
        minx=(s32)floorf(fminf(p[0].x,fminf(p[1].x,p[2].x)));maxx=(s32)ceilf(fmaxf(p[0].x,fmaxf(p[1].x,p[2].x)));miny=(s32)floorf(fminf(p[0].y,fminf(p[1].y,p[2].y)));maxy=(s32)ceilf(fmaxf(p[0].y,fmaxf(p[1].y,p[2].y)));if(minx<0)minx=0;if(maxx>399)maxx=399;if(miny<0)miny=0;if(maxy>299)maxy=299;if(minx>maxx||miny>maxy)continue;
        {f32 ia=1.0f/area,dw0x=(p[1].y-p[2].y)*ia,dw0y=(p[2].x-p[1].x)*ia,dw1x=(p[2].y-p[0].y)*ia,dw1y=(p[0].x-p[2].x)*ia,dw2x=-dw0x-dw1x,dw2y=-dw0y-dw1y;f32 px=minx+.5f,py=miny+.5f,w0r=((p[1].x-px)*(p[2].y-py)-(p[1].y-py)*(p[2].x-px))*ia,w1r=((p[2].x-px)*(p[0].y-py)-(p[2].y-py)*(p[0].x-px))*ia,w2r=1-w0r-w1r;f32 uq[3]={p[0].u*iz[0],p[1].u*iz[1],p[2].u*iz[2]},vq[3]={p[0].v*iz[0],p[1].v*iz[1],p[2].v*iz[2]},sq[3]={p[0].shade*iz[0],p[1].shade*iz[1],p[2].shade*iz[2]};f32 qr=w0r*iz[0]+w1r*iz[1]+w2r*iz[2],ur=w0r*uq[0]+w1r*uq[1]+w2r*uq[2],vr=w0r*vq[0]+w1r*vq[1]+w2r*vq[2],sr=w0r*sq[0]+w1r*sq[1]+w2r*sq[2];f32 dqx=dw0x*iz[0]+dw1x*iz[1]+dw2x*iz[2],dqy=dw0y*iz[0]+dw1y*iz[1]+dw2y*iz[2],dux=dw0x*uq[0]+dw1x*uq[1]+dw2x*uq[2],duy=dw0y*uq[0]+dw1y*uq[1]+dw2y*uq[2],dvx=dw0x*vq[0]+dw1x*vq[1]+dw2x*vq[2],dvy=dw0y*vq[0]+dw1y*vq[1]+dw2y*vq[2],dsx=dw0x*sq[0]+dw1x*sq[1]+dw2x*sq[2],dsy=dw0y*sq[0]+dw1y*sq[1]+dw2y*sq[2];
            for(y=miny;y<=maxy;y++){s32 x;f32 w0=w0r,w1=w1r,w2=w2r,q=qr,uo=ur,vo=vr,so=sr;for(x=minx;x<=maxx;x++){s32 pos=y*400+x;if(w0>=0&&w1>=0&&w2>=0&&q>0&&(s_stamp[pos]!=fs||q>s_q[pos])){f32 iq=1.0f/q,u=uo*iq,v=vo*iq,shd=so*iq,r,g,b;sample(u,v,&r,&g,&b);r*=shd;g*=shd;b*=shd;if(r>255)r=255;if(g>255)g=255;if(b>255)b=255;s_color[pos]=0xff000000U|((uint32_t)b<<16)|((uint32_t)g<<8)|(uint32_t)r;s_q[pos]=q;s_stamp[pos]=fs;}w0+=dw0x;w1+=dw1x;w2+=dw2x;q+=dqx;uo+=dux;vo+=dvx;so+=dsx;}w0r+=dw0y;w1r+=dw1y;w2r+=dw2y;qr+=dqy;ur+=duy;vr+=dvy;sr+=dsy;}}
skip: ;
    }
    for(y=0;y<sh&&y<544;y++){s32 ny=s_map_y[y],x;if(ny<0||ny>=300)continue;for(x=0;x<sw&&x<960;x++){s32 nx=s_map_x[x],pos;if(nx<0||nx>=400)continue;pos=ny*400+nx;if(s_stamp[pos]==fs)dst[y*stride+x]=s_color[pos];}}
}


/* Retail WONDERS Hall screen 7 uses scene 1425 driven by animation 1426.
 * It is intentionally kept in this lightweight software Hall renderer: the
 * scene is only 340 triangles with one opaque 32x32 texture. */
static void finale_prep_tex(void){
    s32 i;if(s_finale_tex_ready)return;
    for(i=0;i<1024;i++){
        u16 p=g_tnt_wonder_finale_hall_texture[i];
        s_finale_tex_rgb[i][0]=(f32)((p>>11)&31)*(255.0f/31.0f);
        s_finale_tex_rgb[i][1]=(f32)((p>>6)&31)*(255.0f/31.0f);
        s_finale_tex_rgb[i][2]=(f32)((p>>1)&31)*(255.0f/31.0f);
    }
    s_finale_tex_ready=1;
}
static void finale_sample(f32 u,f32 v,f32*r,f32*g,f32*b){
    s32 x0,y0,x1,y1;f32 fx,fy;const f32*c00,*c10,*c01,*c11;
    if(u<0)u=0;if(v<0)v=0;if(u>31)u=31;if(v>31)v=31;
    x0=(s32)u;y0=(s32)v;x1=x0<31?x0+1:x0;y1=y0<31?y0+1:y0;fx=u-x0;fy=v-y0;
    c00=s_finale_tex_rgb[y0*32+x0];c10=s_finale_tex_rgb[y0*32+x1];
    c01=s_finale_tex_rgb[y1*32+x0];c11=s_finale_tex_rgb[y1*32+x1];
    *r=(c00[0]+(c10[0]-c00[0])*fx)*(1-fy)+(c01[0]+(c11[0]-c01[0])*fx)*fy;
    *g=(c00[1]+(c10[1]-c00[1])*fx)*(1-fy)+(c01[1]+(c11[1]-c01[1])*fx)*fy;
    *b=(c00[2]+(c10[2]-c00[2])*fx)*(1-fy)+(c01[2]+(c11[2]-c01[2])*fx)*fy;
}
static void finale_camera_view(HallMatrix *view){
    const f32*v=g_tnt_wonder_finale_hall_nodes[0];
    f32 ex=v[0],ey=v[1],ez=v[2],fx=ex-v[3],fy=ey-v[4],fz=ez-v[5];
    f32 rx,ry,rz,ux,uy,uz,rc,rs,rrx,rry,rrz,rux,ruy,ruz;
    if(fx==0&&fy==0&&fz==0)fz=-1;norm3(&fx,&fy,&fz);rx=fz;ry=0;rz=-fx;norm3(&rx,&ry,&rz);
    ux=fy*rz-ry*fz;uy=fz*rx-rz*fx;uz=fx*ry-rx*fy;rc=cosf(v[6]*DEG2RAD);rs=sinf(v[6]*DEG2RAD);
    rrx=rx*rc+ux*rs;rry=ry*rc+uy*rs;rrz=rz*rc+uz*rs;rux=-rx*rs+ux*rc;ruy=-ry*rs+uy*rc;ruz=-rz*rs+uz*rc;
    ident(view);view->m[0][0]=rrx;view->m[0][1]=rry;view->m[0][2]=rrz;view->m[1][0]=rux;view->m[1][1]=ruy;view->m[1][2]=ruz;view->m[2][0]=fx;view->m[2][1]=fy;view->m[2][2]=fz;
    ex*=16.0f;ey*=16.0f;ez*=16.0f;view->m[0][3]=-(rrx*ex+rry*ey+rrz*ez);view->m[1][3]=-(rux*ex+ruy*ey+ruz*ez);view->m[2][3]=-(fx*ex+fy*ey+fz*ez);
}
void TNT_Vita_WonderRenderFinaleHall(void *buffer,s32 stride,s32 sw,s32 sh,s32 bx,s32 by,s32 cell,u16 anim_tick){
    HallMatrix view,local[4],world[4],draw[4],mv[4];f32 vals[4][9];
    uint32_t*dst=(uint32_t*)buffer;u16 fs=(u16)(s_frame_stamp+1);s32 i,y;
    const f32 focal=1.0f/tanf((21.3204402924f*0.5f)*DEG2RAD);
    finale_prep_tex();prep_map(bx,by,cell,sw,sh);if(fs==0){memset(s_stamp,0,sizeof(s_stamp));fs=1;}s_frame_stamp=fs;finale_camera_view(&view);
    memcpy(vals,g_tnt_wonder_finale_hall_nodes,sizeof(vals));
    /* Resource 1426: node 3 X rotation, terminal 511, first difference
     * -0.70588237047 degrees/tick (exactly one revolution per loop). */
    vals[3][0]=-0.70588237047f*(f32)(anim_tick%511U);
    for(i=0;i<4;i++)type2(vals[i],&local[i]);
    world[3]=local[3];mult(&world[1],&world[3],&local[1]);mult(&world[2],&world[3],&local[2]);world[0]=local[0];
    for(i=1;i<=3;i++){
        s32 mi=i==1?0:(i==2?1:2);draw[i]=world[i];
        draw[i].m[0][0]*=g_tnt_wonder_finale_hall_mesh_scale[mi];draw[i].m[0][1]*=g_tnt_wonder_finale_hall_mesh_scale[mi];draw[i].m[0][2]*=g_tnt_wonder_finale_hall_mesh_scale[mi];
        draw[i].m[1][0]*=g_tnt_wonder_finale_hall_mesh_scale[mi];draw[i].m[1][1]*=g_tnt_wonder_finale_hall_mesh_scale[mi];draw[i].m[1][2]*=g_tnt_wonder_finale_hall_mesh_scale[mi];
        draw[i].m[2][0]*=g_tnt_wonder_finale_hall_mesh_scale[mi];draw[i].m[2][1]*=g_tnt_wonder_finale_hall_mesh_scale[mi];draw[i].m[2][2]*=g_tnt_wonder_finale_hall_mesh_scale[mi];
        draw[i].m[0][3]*=16.0f;draw[i].m[1][3]*=16.0f;draw[i].m[2][3]*=16.0f;mult(&mv[i],&view,&draw[i]);
    }
    for(i=0;i<340;i++){
        const TNTWonderTriangle*tri=&g_tnt_wonder_finale_hall_triangles[i];HallPV p[3];f32 iz[3],area;s32 k,minx,maxx,miny,maxy;u8 node=tri->node;
        if(node<1||node>3)continue;
        for(k=0;k<3;k++){
            const TNTWonderVertex*v=&tri->v[k];f32 cx,cy,cz,d,nx,ny,nz,dot;
            point(&mv[node],v->x,v->y,v->z,&cx,&cy,&cz);d=-cz;if(d<2.0f||d>4000.0f)goto fskip;
            p[k].x=200.0f+(cx*focal/(4.0f/3.0f)/d)*200.0f;p[k].y=150.0f-(cy*focal/d)*150.0f;p[k].z=d;iz[k]=1.0f/d;
            p[k].u=(f32)v->s/64.0f;p[k].v=(f32)v->t/64.0f;
            vector(&world[node],(f32)v->nx/127.0f,(f32)v->ny/127.0f,(f32)v->nz/127.0f,&nx,&ny,&nz);norm3(&nx,&ny,&nz);
            /* Scene light 1424 carries 0.3 gray ambient. Retain the same Hall
             * +Y diffuse convention used by the restored locked-Wonder model. */
            dot=ny*(120.0f/127.0f);if(dot<0)dot=0;p[k].shade=0.3f+dot;if(p[k].shade>1)p[k].shade=1;
        }
        area=(p[1].x-p[0].x)*(p[2].y-p[0].y)-(p[1].y-p[0].y)*(p[2].x-p[0].x);if(area>=0)continue;
        minx=(s32)floorf(fminf(p[0].x,fminf(p[1].x,p[2].x)));maxx=(s32)ceilf(fmaxf(p[0].x,fmaxf(p[1].x,p[2].x)));miny=(s32)floorf(fminf(p[0].y,fminf(p[1].y,p[2].y)));maxy=(s32)ceilf(fmaxf(p[0].y,fmaxf(p[1].y,p[2].y)));if(minx<0)minx=0;if(maxx>399)maxx=399;if(miny<0)miny=0;if(maxy>299)maxy=299;if(minx>maxx||miny>maxy)continue;
        {f32 ia=1.0f/area,dw0x=(p[1].y-p[2].y)*ia,dw0y=(p[2].x-p[1].x)*ia,dw1x=(p[2].y-p[0].y)*ia,dw1y=(p[0].x-p[2].x)*ia,dw2x=-dw0x-dw1x,dw2y=-dw0y-dw1y;f32 px=minx+.5f,py=miny+.5f,w0r=((p[1].x-px)*(p[2].y-py)-(p[1].y-py)*(p[2].x-px))*ia,w1r=((p[2].x-px)*(p[0].y-py)-(p[2].y-py)*(p[0].x-px))*ia,w2r=1-w0r-w1r;f32 uq[3]={p[0].u*iz[0],p[1].u*iz[1],p[2].u*iz[2]},vq[3]={p[0].v*iz[0],p[1].v*iz[1],p[2].v*iz[2]},sq[3]={p[0].shade*iz[0],p[1].shade*iz[1],p[2].shade*iz[2]};f32 qr=w0r*iz[0]+w1r*iz[1]+w2r*iz[2],ur=w0r*uq[0]+w1r*uq[1]+w2r*uq[2],vr=w0r*vq[0]+w1r*vq[1]+w2r*vq[2],sr=w0r*sq[0]+w1r*sq[1]+w2r*sq[2];f32 dqx=dw0x*iz[0]+dw1x*iz[1]+dw2x*iz[2],dqy=dw0y*iz[0]+dw1y*iz[1]+dw2y*iz[2],dux=dw0x*uq[0]+dw1x*uq[1]+dw2x*uq[2],duy=dw0y*uq[0]+dw1y*uq[1]+dw2y*uq[2],dvx=dw0x*vq[0]+dw1x*vq[1]+dw2x*vq[2],dvy=dw0y*vq[0]+dw1y*vq[1]+dw2y*vq[2],dsx=dw0x*sq[0]+dw1x*sq[1]+dw2x*sq[2],dsy=dw0y*sq[0]+dw1y*sq[1]+dw2y*sq[2];
            for(y=miny;y<=maxy;y++){s32 x;f32 w0=w0r,w1=w1r,w2=w2r,q=qr,uo=ur,vo=vr,so=sr;for(x=minx;x<=maxx;x++){s32 pos=y*400+x;if(w0>=0&&w1>=0&&w2>=0&&q>0&&(s_stamp[pos]!=fs||q>s_q[pos])){f32 iq=1.0f/q,u=uo*iq,v=vo*iq,shd=so*iq,r,g,b;finale_sample(u,v,&r,&g,&b);r*=shd;g*=shd;b*=shd;if(r>255)r=255;if(g>255)g=255;if(b>255)b=255;s_color[pos]=0xff000000U|((uint32_t)b<<16)|((uint32_t)g<<8)|(uint32_t)r;s_q[pos]=q;s_stamp[pos]=fs;}w0+=dw0x;w1+=dw1x;w2+=dw2x;q+=dqx;uo+=dux;vo+=dvx;so+=dsx;}w0r+=dw0y;w1r+=dw1y;w2r+=dw2y;qr+=dqy;ur+=duy;vr+=dvy;sr+=dsy;}}
fskip: ;
    }
    for(y=0;y<sh&&y<544;y++){s32 ny=s_map_y[y],x;if(ny<0||ny>=300)continue;for(x=0;x<sw&&x<960;x++){s32 nx=s_map_x[x],pos;if(nx<0||nx>=400)continue;pos=ny*400+nx;if(s_stamp[pos]==fs)dst[y*stride+x]=s_color[pos];}}
}
