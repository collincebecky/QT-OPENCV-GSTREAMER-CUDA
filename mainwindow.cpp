#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>



MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    QObject::connect(this, SIGNAL(returntomain(QImage)),
    this, SLOT(update_image(QImage)));
}




void MainWindow::sig_handler(int signo)
{
    if( signo == SIGINT )
    {
        printf("received SIGINT\n");
        signal_recieved = true;
    }
}

int MainWindow::usage()
{
    printf("usage: detectnet-camera [-h] [--network NETWORK] [--threshold THRESHOLD]\n");
    printf("                        [--camera CAMERA] [--width WIDTH] [--height HEIGHT]\n\n");
    printf("Locate objects in a live camera stream using an object detection DNN.\n\n");
    printf("optional arguments:\n");
    printf("  --help            show this help message and exit\n");
    printf("  --network NETWORK pre-trained model to load (see below for options)\n");
    printf("  --overlay OVERLAY detection overlay flags (e.g. --overlay=box,labels,conf)\n");
    printf("                    valid combinations are:  'box', 'labels', 'conf', 'none'\n");
     printf("  --alpha ALPHA     overlay alpha blending value, range 0-255 (default: 120)\n");
    printf("  --camera CAMERA   index of the MIPI CSI camera to use (e.g. CSI camera 0),\n");
    printf("                    or for VL42 cameras the /dev/video device to use.\n");
     printf("                    by default, MIPI CSI camera 0 will be used.\n");
    printf("  --width WIDTH     desired width of camera stream (default is 1280 pixels)\n");
    printf("  --height HEIGHT   desired height of camera stream (default is 720 pixels)\n");
    printf("  --threshold VALUE minimum threshold for detection (default is 0.5)\n\n");

    printf("%s\n", detectNet::Usage());


}
void MainWindow::show_video(){
    /*
         * parse command line
         */


        //if( cmdLine.GetFlag("help") )
          //  return usage();


        /*
         * attach signal handler
         */
        if( signal(SIGINT, sig_handler) == SIG_ERR )
            printf("\ncan't catch SIGINT\n");


        /*
         * create the camera device
         */


        /*

        gstCamera* camera = gstCamera::Create(cmdLine.GetInt("width", gstCamera::DefaultWidth),
                                       cmdLine.GetInt("height", gstCamera::DefaultHeight),
                                       cmdLine.GetString("camera"));
                                       */


        gstCamera* camera = gstCamera::Create(width,height,NULL);




        if( !camera )
        {
            printf("\ndetectnet-camera:  failed to initialize camera device\n");

        }

        printf("\ndetectnet-camera:  successfully initialized camera device\n");
        printf("    width:  %u\n", camera->GetWidth());
        printf("   height:  %u\n", camera->GetHeight());
        printf("    depth:  %u (bpp)\n\n", camera->GetPixelDepth());


        /*
         * create detection network
         */
        detectNet* net = detectNet::Create(prototxt_path,model_path);

        if( !net )
        {
            printf("detectnet-camera:   failed to load detectNet model\n");

        }

        // parse overlay flags
       const uint32_t overlayFlags = detectNet::OverlayFlagsFromStr("overlay");


        /*
         * create openGL window
         */
       // glDisplay* display = glDisplay::Create();




        /*
         * start streaming
         */
        if( !camera->Open() )
        {
            printf("detectnet-camera:  failed to open camera for streaming\n");

        }

        printf("detectnet-camera:  camera open for streaming\n");


        /*
         * processing loop
         */
        float confidence = 0.0f;

        //cv::Mat rgba[4];

        while( !signal_recieved )
        {
            // capture RGBA image
            float* imgRGBA = NULL;


               //->NOTE add 1 into the parameter so the data could be available in CPU/GPU shared memory

            if( !camera->CaptureRGBA(&imgRGBA, 1000,1) )
                printf("detectnet-camera:  failed to capture RGBA image from camera\n");

            // detect objects in the frame
            detectNet::Detection* detections = NULL;

            const int numDetections = net->Detect(imgRGBA, camera->GetWidth(), camera->GetHeight(), &detections, overlayFlags);

             qDebug()<<"out on you way .........................................."<<endl;

            if( numDetections > 0 )
            {

                printf("%i objects detected\n", numDetections);

                for( int n=0; n < numDetections; n++ )
                {
                    printf("detected obj %i  class #%u (%s)  confidence=%f\n", n, detections[n].ClassID, net->GetClassDesc(detections[n].ClassID), detections[n].Confidence);
                    printf("bounding box %i  (%f, %f)  (%f, %f)  w=%f  h=%f\n", n, detections[n].Left, detections[n].Top, detections[n].Right, detections[n].Bottom, detections[n].Width(), detections[n].Height());
                }
            }



           cv::Mat dst_img=cv::Mat(camera->GetHeight(),camera->GetWidth(),CV_32FC4,imgRGBA);


           dst_img/=255;
           cv::cvtColor(dst_img,dst_img,CV_BGRA2RGB);


           QImage qt_image=putImage(dst_img);

          // emit returntomain(qt_image);

           ui->label->setPixmap(QPixmap::fromImage(qt_image).scaled(ui->label->size(),
           Qt::KeepAspectRatio, Qt::FastTransformation));


          /// cv::imshow("Imshow",dst_img);
           //if(cv::waitKey(10)==27) signal_recieved=true;
            break;








       }


        /*
         * destroy resources
         */
        //printf("detectnet-camera:  shutting down...\n");

        SAFE_DELETE(camera);

        SAFE_DELETE(net);

        printf("detectnet-camera:  shutdown complete.\n");




}
const QImage MainWindow::mat8ToImage(const cv::Mat &mat)
{
    switch (mat.type()) {
    case CV_8UC1:
    {

        QVector<QRgb> ct;
        for (int i = 0; i < 256; ++i)
            ct.append(qRgb(i, i, i));
        QImage result(mat.data, mat.cols, mat.rows, (int) mat.step, QImage::Format_Indexed8);
        result.setColorTable(ct);
        return result.copy();
    }
    case CV_8UC3:
    {

        cv::Mat tmp;
        cvtColor(mat, tmp, cv::COLOR_BGR2BGRA);
        return mat8ToImage(tmp);
    }
    case CV_8UC4:
    {
        QImage result(mat.data, mat.cols, mat.rows, (int) mat.step, QImage::Format_RGB32);
        return result.rgbSwapped();
    }
    default:
        qWarning("Unhandled Mat format %d", mat.type());
        return QImage();
    }
}

void MainWindow::update_image(const QImage &image){

    qDebug()<<"comming here .................."<<image<<endl;
    ui->label->setPixmap(QPixmap::fromImage(image).scaled(ui->label->pixmap()->size(),
    Qt::KeepAspectRatio,Qt::FastTransformation));



    //ui->label->resize(ui->label->pixmap()->size());




}
MainWindow::~MainWindow(){
    delete ui;
}


void MainWindow::on_pushButton_clicked()
{
    show_video();
}
///////////////////////////////////////////////////////////////////////////////


QImage MainWindow::putImage(const cv::Mat& mat)
  {




      if(mat.type()==CV_8UC1)
      {
           // Set the color table (used to translate color indexes to qRgb values)
         QVector<QRgb> colorTable;
           for (int i = 0; i<256; i++)
             colorTable.push_back( qRgb(i,i,i) );
            /*
             *
           if (deepCopy)
         {
               QImage img(mat.cols, mat.rows, QImage::Format_Indexed8);
               for (int i = 0; i < img.height(); i++)
                   // scanLine returns a ptr to the start of the data for that row
                memcpy( img.scanLine(i), mat.ptr(i), img.bytesPerLine() );  //correct
              return img;
          }
          else
           {
             // Copy input Mat
              const uchar *qImageBuffer = (const uchar*)mat.data;

              // Create QImage with same dimensions as input Mat
               QImage img(qImageBuffer, mat.cols, mat.rows, mat.step, QImage::Format_Indexed8);
              img.setColorTable(colorTable);
              return img;
           }
          */
       }
       else if (mat.type()==CV_16UC1)
      {
          cv::Mat ucharMatScaled;
         cv::Mat ushortMatScaled;
          cv::Mat floatMatScaled;
        double minImage, maxImage;
          cv::minMaxLoc(mat, &minImage, &maxImage);
         mat.convertTo(floatMatScaled, CV_32FC1);

          // to ensure [0-1.0]
          floatMatScaled = (floatMatScaled - minImage) / (maxImage - minImage);
          floatMatScaled.convertTo(ucharMatScaled, CV_8UC1, 255, 0);
           return putImage(ucharMatScaled);
       }
      else if (mat.type()==CV_32FC1)
      {
          cv::Mat ucharMatScaled;
           cv::Mat floatMatScaled;
          double minImage, maxImage;
         cv::minMaxLoc(mat, &minImage, &maxImage);
           // to ensure [0-1.0]
         floatMatScaled = (mat - minImage) / (maxImage - minImage);
          floatMatScaled.convertTo(ucharMatScaled, CV_8UC1, 255, 0);

          return putImage(ucharMatScaled);
      }
    else if (mat.type() == CV_32FC3)
      {
          cv::Mat ucharMatScaled;
          cv::Mat floatMatScaled;
          double minImage, maxImage;
         cv::minMaxLoc(mat, &minImage, &maxImage);

          normalize(mat, floatMatScaled, 0.0, 1.0, cv::NORM_MINMAX);
       cv::pow(floatMatScaled, 1. / 5, floatMatScaled); // apply gamma curve: img = img ** (1./5)
          mat.convertTo(ucharMatScaled, CV_8UC3, 255, 0);
          //qDebug() << "type ucharMatScaled = " << MiscFunctions::matTypeToText( ucharMatScaled.type() );

          return putImage(ucharMatScaled);
       }

      // 8-bits unsigned, NO. OF CHANNELS=3
     else
    {
       cv::Mat rgbMat;
        int qImageFormat = QImage::Format_RGB888;
        if(mat.type()==CV_8UC4)
        {
             qImageFormat = QImage::Format_ARGB32;
          rgbMat = mat;
       }
     else if (mat.type()==CV_8UC3)
      {
          rgbMat = mat;
       }
     else
       {
         cvtColor(mat, rgbMat,CV_BGR2RGB);
      }

      // Copy input Mat
     const uchar *qImageBuffer = (const uchar*)mat.data;

    // Create QImage with same dimensions as input Mat
     QImage img(qImageBuffer, mat.cols, mat.rows, mat.step,(QImage::Format) qImageFormat);
     return img.rgbSwapped();     // deep copy !!
       //}
    }
 }


////////////////////////////////////////
