使用export LD_LIBRARY_PATH="/root/test/lib":$LD_LIBRARY_PATH  包含libmk_api.so

或将libmk_api.so放入/usr/local/lib

无法直接运行，请结合自己代码修改，或者使用zlmediakit的分帧器读取h264文件，头文件包含在include中寻找

//创建h264分帧器

mk_h264_splitter splitter = mk_h264_splitter_create(on_h264_frame, media, 0);

signal(SIGINT, s_on_exit);// 设置退出信号


char buf[1024];

while (!exit_flag) {

    int size = fread(buf, 1, sizeof(buf) - 1, fp);
	
    if (size > 0) {
	
        mk_h264_splitter_input_data(splitter, buf, size);
		
    } else {
	
        //文件读完了，重新开始
		
        fseek(fp, 0, SEEK_SET);
		
    }
	
}


log_info("文件读取完毕");

mk_h264_splitter_release(splitter);
