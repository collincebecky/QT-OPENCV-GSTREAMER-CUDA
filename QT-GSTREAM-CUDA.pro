#-------------------------------------------------
#
# Project created by QtCreator 2020-01-18T17:30:42
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = QT-GSTREAM-CUDA
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0
/////////////////////////////////////////////////////
CONFIG += link_pkgconfig


#INCLUDEPATH+=/usr/include/opencv
#INCLUDEPATH+=/usr/include/opencv
#QMAKE_CXXFLAGS += -Wall


#LIBS += `pkg-config opencv --libs` -lopencv_videoio  #-lopencv_dnn
#PKGCONFIG += opencv






////////////////////////////////////////////////////
PKGCONFIG += gstreamer-1.0 glib-2.0 gobject-2.0 gstreamer-app-1.0 gstreamer-pbutils-1.0


SOURCES += \
        main.cpp \
        mainwindow.cpp\
        gstCamera.cpp

HEADERS += \
        mainwindow.h\
        gstCamera.h\
        gstUtility.h\
        Mutex.h\
        Event.h\
        timespec.h\
        glUtility.h\
        glTexture.h

FORMS += \
        mainwindow.ui


#LIBS += -lopencv_core -lopencv_imgcodecs -lopencv_highgui -lopencv_shape -lopencv_videoio
LIBS += -lopencv_core -lopencv_imgproc -lopencv_highgui -lopencv_ml -lopencv_video -lopencv_features2d -lopencv_calib3d -lopencv_objdetect  -lopencv_flann


########################Jetson Inferencing ###########################
LIBS+=-ljetson-inference -ljetson-utils
INCLUDEPATH+=/usr/include/gstreamer-1.0/ /usr/include/glib-2.0
INCLUDEPATH+=/home/brance/Mine/jetson-inference/x86_64/include/jetson-utils/
INCLUDEPATH+=/home/brance/Mine/jetson-inference/c
LIBS+= `pkg-config --cflags gstreamer-plugins-base-1.0` #`pkg-config --cflags --libs opencv`

############################Tensorrt##################################
LIBS+= -lnvparsers -lnvinfer -lnvcaffe_parser
include (/home/brance/QT-GSTREAM-CUDA/opencv.pri)


LIBS +=-lnvcaffe_parser
LIBS +=-lnvinfer
LIBS +=-lnvinfer_plugin
#LIBS +=-lnvinfer_static
#LIBS +=-lnvonnxparser
#LIBS +=-lnvonnxparser_runtime
LIBS +=-lnvparsers
#LIBS +=-lnvparsers_statistics
#################cuda and blass#######################
#LIBS +=-lopenblas
LIBS +=-lcudnn
LIBS +=-L"/usr/local/cuda/lib64"
LIBS +=-lcublas
LIBS +=-lcudart
LIBS +=-lcurand
LIBS +=-lcusolver


UDA_SOURCES += process/kernel.cu\
process/util/cuda/cudaNormalize.cu\
process/util/cuda/cudaOverlay.cu\
process/util/cuda/cudaResize.cu\
process/util/cuda/cudaRGB.cu\
process/util/cuda/cudaYUV-NV12.cu\
process/util/cuda/cudaYUV-YUYV.cu\
process/util/cuda/cudaYUV-YV12.cu
# Project dir and outputs
PROJECT_DIR = $$system(pwd)
OBJECTS_DIR = $$PROJECT_DIR/Obj
DESTDIR = ./bin

# Path to cuda SDK install

# Path to cuda toolkit install
CUDA_DIR = /usr/local/cuda
# GPU architecture
CUDA_ARCH = sm_52
# nvcc flags (ptxas option verbose is always useful)
NVCCFLAGS = --compiler-options -fno-strict-aliasing -use_fast_math --ptxas-options=-v
# include paths
INCLUDEPATH += $$CUDA_DIR/include
INCLUDEPATH += $$CUDA_SDK/common/inc/
INCLUDEPATH += $$CUDA_SDK/../shared/inc/
# lib dirs
QMAKE_LIBDIR += $$CUDA_DIR/lib64
#QMAKE_LIBDIR += $$CUDA_SDK/lib
#QMAKE_LIBDIR += $$CUDA_SDK/common/lib
# libs - note than i'm using a x_86_64 machine
LIBS += -lcudart #-lcutil_x86_64
# join the includes in a line
CUDA_INC = $$join(INCLUDEPATH,' -I','-I',' ')

cuda.input = CUDA_SOURCES
cuda.output = ${OBJECTS_DIR}${QMAKE_FILE_BASE}_cuda.o

cuda.commands = $$CUDA_DIR/bin/nvcc -m64 -g -G -arch=$$CUDA_ARCH -c $$NVCCFLAGS $$CUDA_INC $$LIBS  ${QMAKE_FILE_NAME} -o ${QMAKE_FILE_OUT}

cuda.dependency_type = TYPE_C # there was a typo here. Thanks workmate!
cuda.depend_command = $$CUDA_DIR/bin/nvcc -g -G -M $$CUDA_INC $$NVCCFLAGS   ${QMAKE_FILE_NAME}
# Tell Qt that we want add more stuff to the Makefile
QMAKE_EXTRA_UNIX_COMPILERS += cuda


#unix:{
    # suppress the default RPATH if you wish
    #QMAKE_LFLAGS_RPATH=
    # add your own with quoting gyrations to make sure $ORIGIN gets to the command line unexpanded
 #   QMAKE_LFLAGS += "-Wl,-rpath,\'\$$ORIGIN\'"
#}
