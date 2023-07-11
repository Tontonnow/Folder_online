#include <iostream>
#include <iomanip>
#include "MySampleReader.h"
#include "MySampleDecrypter.h"
#include "widevinecdm.h"
AP4_Result MySampleReader::ReadSampleData(AP4_Sample& sample, AP4_DataBuffer& sample_data)
{
	auto printfHex = [](char *byteArray, int length) {
		
		// 打印十六进制字符串
		for (int i = 0; i < length; ++i) {
			std::cout << std::hex << std::setw(2) << std::setfill('0')
				<< static_cast<int>(byteArray[i]);
		}

		std::cout << std::endl;
	};

    int dataSize = sample.GetSize();
    char* data = new char[dataSize + 1];
    memset(data, 0, dataSize + 1);
    sample.GetDataStream()->Seek(sample.GetOffset());
    sample.GetDataStream()->Read(data, dataSize);



    AVFrame* frame;
    frame = av_frame_alloc();
    AVPacket* packet = av_packet_alloc();

	if (this->m_decrypter != nullptr) {

		AP4_Cardinal subsample_count =0;
		const AP4_UI16* bytes_of_cleartext_data;
		const AP4_UI32* bytes_of_encrypted_data;

		const AP4_UI08* iv = m_decrypter->m_table->GetIv(m_decrypter->index);
		uint32_t ivSize = m_decrypter->m_table->GetIvSize();
		int64_t timestamp = ((double)sample.GetDts() * ((double)1000000 / (double)m_decrypter->m_timeScale) + 0.5);


		m_decrypter->m_table->GetSampleInfo(m_decrypter->index, subsample_count, bytes_of_cleartext_data, bytes_of_encrypted_data);


		printf("read sample offset: 0x%llX size: 0x%X isEncrypted: true\n", sample.GetOffset(), sample.GetSize());
		printf("keyid: ");
		printfHex((char *)m_decrypter->m_key_id, m_decrypter->m_key_id_size);
		printf("iv: ");
		printfHex((char*)iv, ivSize);

		for (int i = 0; i < subsample_count; i++) {
			printf("subsample[%d] bytes_of_cleartext_data %X bytes_of_encrypted_data %X\n", i, bytes_of_cleartext_data[i], bytes_of_encrypted_data[i]);
		}
		m_decrypter->index++;


       
        char* key_id = (char *)m_decrypter->m_key_id;
      

        SubsampleEntry* subsamples = new SubsampleEntry[subsample_count];
        for (int i = 0; i < subsample_count; i++) {
            subsamples[i].clear_bytes = bytes_of_cleartext_data[i];
            subsamples[i].cipher_bytes = bytes_of_encrypted_data[i];
        }

      

        InputBuffer_2 input;
        input.data = (UINT8 *)data;
        input.data_size = dataSize;
        input.encryption_scheme = EncryptionScheme::kCenc;
        input.key_id = (uint8_t*)key_id;
        input.key_id_size = 0x10;
        input.iv = iv;
        input.iv_size = ivSize;
        input.subsamples = subsamples;
        input.num_subsamples = subsample_count;
        input.pattern.crypt_byte_block = 0;
        input.pattern.skip_byte_block = 0;
        input.timestamp = timestamp;
        MyVideoFrame videoFrame;
        MyVideoFrame* video_frame = &videoFrame;
      
        int result = proxy->DecryptAndDecodeFrame(&input, &videoFrame);
        printf("DecryptAndDecodeFrame result %d", result);
        /*cout << "width * height:" << videoFrame.SSize().width << "*" << videoFrame.SSize().height << endl;
        cout << "videoFrame.m_format : " << videoFrame.Format() << endl;
        cout << "Timestamp : " << videoFrame.Timestamp() << endl;
        for (int i = 0; i < VideoFrame::VideoPlane::kMaxPlanes; i++) {
            cout << "videoFrame.PlaneOffset((VideoFrame::VideoPlane)" << i << ")" << videoFrame.PlaneOffset((VideoFrame::VideoPlane)i) << endl;
            cout << "videoFrame.Stride((VideoFrame::VideoPlane)" << i << ")" << videoFrame.Stride((VideoFrame::VideoPlane)i) << endl;
        }*/
        FILE* pVideo;
        pVideo = fopen("frame.yuv", "ab");
       
        unsigned char* buffer = NULL;
        transtoYUV(video_frame, buffer);
      
        fwrite(buffer, 1, video_frame->SSize().width * video_frame->SSize().height * 1.5, pVideo);
     

        fclose(pVideo);
    
        delete buffer;
      
      
       
	}
	else {
        packet->data = (uint8_t*)data;
        packet->size = dataSize;
        packet->duration = 0xa2c3;
        int error_code = av_packet_make_refcounted(packet);


        int sendCode = avcodec_send_packet(decodecContext, packet);
        if (sendCode >= 0) {
            while (1) {
                int arfcode = avcodec_receive_frame(decodecContext, frame);
                if (arfcode == 0 || arfcode == 0xDFB9B0BB || arfcode == AVERROR(EAGAIN)) {
                    break;
                }
                if (arfcode < 0) {
                    printf("receive frame failed \n");
                    return 0;
                }

            }
        }

        // 输入AVFrame的宽、高和像素格式
        int src_w = frame->width;
        int src_h = frame->height;
        AVPixelFormat src_pix_fmt = (AVPixelFormat)frame->format;
        // 目标输出YUV420P格式
        int dst_w = src_w;
        int dst_h = src_h;
        AVPixelFormat dst_pix_fmt = AV_PIX_FMT_YUV420P;
        // 创建SwsContext对象
        struct SwsContext* sws_ctx = sws_getContext(src_w, src_h, src_pix_fmt,
            dst_w, dst_h, dst_pix_fmt,
            SWS_BILINEAR, NULL, NULL, NULL);
        // 分配输出YUV数据缓冲区
        uint8_t* buffer[AV_NUM_DATA_POINTERS] = { 0 };
        buffer[0] = new uint8_t[dst_w * dst_h];
        buffer[1] = new uint8_t[dst_w / 2 * dst_h / 2];
        buffer[2] = new uint8_t[dst_w / 2 * dst_h / 2];
        int linesize[AV_NUM_DATA_POINTERS] = { 0 };
        linesize[0] = dst_w;
        linesize[1] = dst_w / 2;
        linesize[2] = dst_w / 2;
        // 调用sws_scale()函数进行像素格式转换和缩放操作
        sws_scale(sws_ctx, frame->data, frame->linesize,
            0, src_h, buffer, linesize);
        // 释放SwsContext对象和输出YUV数据缓冲区
        sws_freeContext(sws_ctx);
        const char* output_filename = "frame.yuv";
        FILE* fp_out = fopen(output_filename, "ab");
        if (!fp_out) {
            printf("Could not open %s\n", output_filename);
            return -1;
        }
        // 写入YUV420P数据
        fwrite(buffer[0], 1, dst_w * dst_h, fp_out);
        fwrite(buffer[1], 1, dst_w / 2 * dst_h / 2, fp_out);
        fwrite(buffer[2], 1, dst_w / 2 * dst_h / 2, fp_out);
        // 关闭输出文件
        fclose(fp_out);

        delete[] buffer[0];
        delete[] buffer[1];
        delete[] buffer[2];

		printf("read sample offset: 0x%llX size: 0x%X isEncrypted: false\n", sample.GetOffset(), sample.GetSize());
	}
    av_packet_free(&packet);
    av_frame_free(&frame);
    delete data;
	return AP4_Result();
}
