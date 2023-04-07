#include "mk_media.h"
#include "mk_pusher.h"
#include <iostream>
#include <string.h> 

typedef struct {
    mk_media media;
    mk_pusher pusher;
    char push_url[128];
} MkContext;

void release_media(mk_media *ptr) {
    if (ptr && *ptr) {
        mk_media_release(*ptr);
        *ptr = NULL;
    }
}

void release_pusher(mk_pusher *ptr) {
    if (ptr && *ptr) {
        mk_pusher_release(*ptr);
        *ptr = NULL;
    }
}

void release_context(MkContext **ptr){
    if (ptr && *ptr) {
        release_pusher(&(*ptr)->pusher);
        release_media(&(*ptr)->media);
        delete *ptr;
        *ptr = NULL;
    }
}

void API_CALL on_mk_push_event_func(void *user_data,int err_code,const char *err_msg){
    MkContext *ctx = (MkContext *) user_data;
    if (err_code == 0) {
        //push success
        printf( "push %s success!", ctx->push_url);
    } else {
        printf( "push %s failed:%d %s", ctx->push_url, err_code, err_msg);
        release_pusher(&(ctx->pusher));
    }
}

void API_CALL on_mk_media_source_regist_func(void *user_data, mk_media_source sender, int regist){
    MkContext *ctx = (MkContext *) user_data;
    const char *schema = mk_media_source_get_schema(sender);
    if (strncmp(schema, ctx->push_url, strlen(schema)) == 0) {
        //判断是否为推流协议相关的流注册或注销事件
        release_pusher(&(ctx->pusher));
        if (regist) {
            ctx->pusher = mk_pusher_create_src(sender);
            mk_pusher_set_on_result(ctx->pusher, on_mk_push_event_func, ctx);
            mk_pusher_set_on_shutdown(ctx->pusher, on_mk_push_event_func, ctx);
            mk_pusher_publish(ctx->pusher, ctx->push_url);
        } else {
            printf( "push stop");
        }
    }
}

int init_publish_h264(MkContext *mkCtx, int width, int height, int frameRate)
{
    int ret;
    std::string push_url = "rtmp://127.0.0.1/live/video0"; //推流地址
    strcpy(mkCtx->push_url, push_url.c_str());
    mkCtx->media = mk_media_create("__defaultVhost__", "live", "video0", 0, 0, 0);
    //h264的codec
    ret = mk_media_init_video(mkCtx->media, 0, width, height, frameRate,0);
    if(ret != 1)
    {
        printf( "mk_media_init_video error");
        return -1;
    }
    mk_media_init_complete(mkCtx->media);
    mk_media_set_on_regist(mkCtx->media, on_mk_media_source_regist_func, mkCtx);
    return 0;
}

int main()
{
    MkContext* mkCtx = new MkContext();
    int width = 1920; //视频宽
    int height = 1080; //视频高
    int frameRate = 25; //视频帧率
    int ret = init_publish_h264(mkCtx,width,height,frameRate);
    if(ret != 0)
    {
        std::cout << "init error" << std::endl;
        delete mkCtx;
        mkCtx = NULL;
    }
    void* h264_buffer = nullptr; //h264帧
    int h264_size = 66355200;//1080p,一般就是8M左右
    while(mkCtx != NULL)
        mk_media_input_h264(mkCtx->media, (void*)h264_buffer, h264_size, 0, 0); //持续推流
    release_context(&mkCtx);
}