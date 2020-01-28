#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "gstCamera.h"
//#include "glDisplay.h"

#include "detectNet.h"
#include "commandLine.h"
#include "cvmatandqimage.h"


#include <opencv2/highgui.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc.hpp>

//#include "opencv2/opencv.hpp"

#include <signal.h>
using namespace cv;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    ////////////////////////////
    static bool signal_recieved;
    void show_video();
    int usage();
    static void sig_handler(int signo);
    int32_t width=1280;
    int32_t height=720;


    char *prototxt_path="/home/brance/Mine/jetson-inference/data/networks/ped-100/deploy.prototxt";
    char *model_path="/home/brance/Mine/jetson-inference/data/networks/ped-100/snapshot_iter_70800.caffemodel";
    const QImage mat8ToImage(const cv::Mat &mat);
    QImage putImage(const Mat& mat);

    ////////////////////////////


signals:
    void                returntomain(const QImage & image);

private slots:
    void on_pushButton_clicked();
    void update_image(const QImage & image);

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
