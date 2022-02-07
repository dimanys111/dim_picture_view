#include "workitem.h"
#include "work.h"
#include "QImageReader"
#include "QFile"
#include "QApplication"
#include "QProcess"

extern "C"
{
#include <libavdevice/avdevice.h>
#include <libswscale/swscale.h>
}

#if LIBAVCODEC_VERSION_INT >= AV_VERSION_INT(55,28,1)
 #define avcodec_alloc_frame() av_frame_alloc()
#else
 #define av_frame_alloc() avcodec_alloc_frame()
#endif

AVFrame* frame = avcodec_alloc_frame();
uint8_t* frame_buffer = (uint8_t *)av_malloc(1000000);
AVPixelFormat format=AV_PIX_FMT_YUVJ420P;
AVCodec *jpegCodec;
AVCodecContext *jpegContext;
FILE *JPEGFile;
AVPacket packet;
int gotFrame;

WorkItem::WorkItem(QObject *parent) : QObject(parent),
    stop(false),
    D(qApp->applicationDirPath()+"/cache/")
{
    av_register_all();
    jpegCodec = avcodec_find_encoder(AV_CODEC_ID_MJPEG);
    frame->format=format;
    frame->quality = 1;
    av_init_packet(&packet);
    moveToThread(&Mythread);
    Mythread.start();
}

WorkItem::~WorkItem()
{

}

void WorkItem::setSpis()
{
    Work::saveMap();
    stop=false;
    emit setDirFiles();
}

int WorkItem::max(QSize size)
{
    if(size.height()>size.width())
    {
        return size.height();
    }
    return size.width();
}

#ifdef __linux__
int
save_frame_as_jpeg(AVFrame *pFrame, char *fileout) {

    jpegContext = avcodec_alloc_context3(jpegCodec);
    jpegContext->pix_fmt = (AVPixelFormat)frame->format;
    jpegContext->codec_id = jpegCodec->id;
    jpegContext->time_base.num = 1;
    jpegContext->time_base.den = 1;

    double ratio = (double)pFrame->width / (double)pFrame->height;
    int icoHeight    = 200;
    int image_height = icoHeight;
    int image_width = image_height * ratio;

    SwsContext *resize = sws_getContext(pFrame->width, pFrame->height, (AVPixelFormat)pFrame->format, image_width, image_height, format, SWS_BICUBIC, NULL, NULL, NULL);

    frame->height = image_height;
    frame->width = image_width;

    avpicture_fill((AVPicture*)frame, frame_buffer, format, image_width, image_height);
    sws_scale(resize, pFrame->data, pFrame->linesize, 0, pFrame->height, frame->data, frame->linesize);
    sws_freeContext(resize);

    jpegContext->height = frame->height;
    jpegContext->width = frame->width;

    if (avcodec_open2(jpegContext, jpegCodec, NULL) < 0) {
        return -1;
    }

    if (avcodec_encode_video2(jpegContext, &packet, frame, &gotFrame) < 0) {
        av_free_packet(&packet);
        avcodec_free_context(&jpegContext);
        return -1;
    }

    JPEGFile = fopen(fileout, "wb");
    fwrite(packet.data, 1, packet.size, JPEGFile);
    fclose(JPEGFile);

    av_free_packet(&packet);
    avcodec_free_context(&jpegContext);
    return 0;
}

int
readsave_frames(int videoStreamIdx
                , AVFormatContext *pFormatCtx
                , AVCodecContext  *pCodecCtx
                , char *fileout)
{
    int             i;
    AVPacket        packet;
    int             frameFinished;
    AVFrame        *pFrame;
    // Allocate video frame
    pFrame = av_frame_alloc();
    for(i=0; av_read_frame(pFormatCtx, &packet) >= 0;)
    {
        // Is this a packet from the video stream?
        if(packet.stream_index==videoStreamIdx) {
            i++;

            // Decode video frame
            int z=avcodec_decode_video2(pCodecCtx, pFrame, &frameFinished, &packet);

            // Did we get a video frame?
            if(frameFinished && z>0)
            {
                // Write pixel data
                if(save_frame_as_jpeg(pFrame, fileout)==0){
                    av_free_packet(&packet);
                    // Free the packet that was allocated by av_read_frame
                    av_frame_free(&pFrame);
                    return 0;
                }
                else{
                    av_frame_free(&pFrame);
                    return -1;
                }
            }
        }
    }
    av_frame_free(&pFrame);
    return 0;
}

int openVideo(char *url,char *fileout)
{
    AVFormatContext *pFormatCtx;
    AVCodecContext  *pCodecCtx;
    AVCodec         *pCodec;
    int             videoStreamIdx;
    // Register all formats and codecs
    pFormatCtx = avformat_alloc_context();

    // Open video file
    if(avformat_open_input(&pFormatCtx, url, 0, NULL) != 0)
    {
        printf("avformat_open_input failed: Couldn't open file\n");
        avformat_free_context(pFormatCtx);
        return -1;
    }

    // Retrieve stream information
    if(avformat_find_stream_info(pFormatCtx, NULL) < 0)
    {
        printf("avformat_find_stream_info failed: Couldn't find stream information\n");
        avformat_close_input(&pFormatCtx);
        return -1;
    }

    // Dump information about file onto standard error
    av_dump_format(pFormatCtx, 0, url, 0);


    // Find the first video stream
    int i = 0;
    videoStreamIdx=-1;
    for(i=0; i<pFormatCtx->nb_streams; i++)
    {
        if(pFormatCtx->streams[i]->codec->codec_type == AVMEDIA_TYPE_VIDEO) { //CODEC_TYPE_VIDEO
            videoStreamIdx=i;
            break;
        }
    }

    // Get a pointer to the codec context for the video stream
    pCodecCtx = pFormatCtx->streams[videoStreamIdx]->codec;

    // Find the decoder for the video stream
    pCodec = avcodec_find_decoder( pCodecCtx->codec_id);
    if(pCodec==NULL) {
        fprintf(stderr, "Unsupported codec!\n");
        avformat_close_input(&pFormatCtx);
        return -1;
    }

    // Open codec
    if( avcodec_open2(pCodecCtx, pCodec, NULL) < 0 ){
        avformat_close_input(&pFormatCtx);
        return -1;
    }

    // Read frames and save them to disk
    if (readsave_frames(videoStreamIdx, pFormatCtx, pCodecCtx,fileout) < 0)
    {
        avformat_close_input(&pFormatCtx);
        return -1;
    }

    // Close the video file
    avformat_close_input(&pFormatCtx);

    return 0;
}
#endif

void WorkItem::obrItem(QString p,QStringList fileNames)
{
    QString s;s.fill(' ',500);
    QListWidgetItem* item=new QListWidgetItem(p+s);
    item->setStatusTip(p);
    item->setForeground(Qt::blue); // sets red text
    emit emitItem(item);
    foreach (QString fileName, fileNames)
    {
        if(stop)
            return;
        QFileInfo inf(fileName);
        QPixmap img;

        QString file_prev=Work::getMap(fileName);
        int n=fileName.lastIndexOf(".");
        QString suf;
        if(n>0)
        {
            suf=fileName.mid(n+1);
        }
        suf=suf.toLower();
        if(suf=="avi" || suf=="mp4" || suf=="3gp"
                || suf=="mov" || suf=="m4v" || suf=="mkv")
        {
            obrVideo(file_prev,fileName,img,inf);
        }
        else
        {
            if(file_prev=="")
            {
                obrImage(fileName,img,inf);
            }
            else
            {
                img.load(D+file_prev);
                if(img.isNull())
                {
                    QString ss=QString().number(inf.size())+"_"+inf.fileName();
                    img.load(D+ss);
                    if(img.isNull())
                    {
                        obrImage(fileName,img,inf);
                    }
                    else
                    {
                        Work::insertMap(fileName,ss);
                    }
                }
            }
        }
        QString s=inf.fileName();
        QListWidgetItem* item=new QListWidgetItem(s.mid(s.size()-10));
        item->setStatusTip(fileName);
        item->setIcon(QIcon(img));
        emit emitItem(item);
    }
}

void WorkItem::obrImage(const QString &fileName, QPixmap &img, const QFileInfo &inf)
{
    QImageReader imageReader(fileName);
    if (imageReader.supportsOption(QImageIOHandler::Size))
    {
        QSize size = imageReader.size();
        if(max(size)>400)
        {
            int image_width = size.width();
            int image_height = size.height();
            double ratio = (double)image_width / (double)image_height;
            int icoHeight    = 200;
            image_height = icoHeight;
            image_width = image_height * ratio;
            imageReader.setScaledSize(QSize(image_width, image_height));
            img = QPixmap::fromImageReader(&imageReader);
            QString ss=QString().number(inf.size())+"_"+inf.fileName();
            img.save(D+ss);
            Work::insertMap(fileName,ss);
        }
        else
        {
            img = QPixmap::fromImageReader(&imageReader);
        }
    }
}

void WorkItem::obrVideo(const QString &file_prev, const QString &fileName, QPixmap &img, const QFileInfo &inf)
{
    #ifdef __linux__

    if(file_prev=="")
    {
        QString s=QString().number(inf.size())+"_"+inf.fileName();
        img.load(D+s);
        if(img.isNull())
        {
            if(openVideo(fileName.toUtf8().data(),QString(D+s).toUtf8().data())==0)
            {
                img.load(D+s);
                Work::insertMap(fileName,s);
            }
        }
        else{
            Work::insertMap(fileName,s);
        }
    }
    else
    {
        img.load(D+file_prev);
        if(img.isNull())
        {
            QString s=QString().number(inf.size())+"_"+inf.fileName();
            if(openVideo(fileName.toUtf8().data(),QString(D+s).toUtf8().data())==0)
            {
                img.load(D+s);
            }
        }
    }

    #endif
}
