#include "camera.h"

Camera::Camera(QWidget *parent)
    : QWidget{parent}
{
    // 创建相机
    camera = new QCamera;

    // 创建取景器，并设置取景器
    viewfinder = new QCameraViewfinder();
    camera->setViewfinder(viewfinder);

    // 创建图像捕捉
    imageCapture = new QCameraImageCapture(camera);

    // 设置捕捉方式以及图像捕捉保存地址
    camera->setCaptureMode(QCamera::CaptureStillImage);
    imageCapture->setCaptureDestination(QCameraImageCapture::CaptureToBuffer);

    // 启动相机
    camera->start();
}

QCameraImageCapture* Camera::getImageCapture(){
    return imageCapture;
}

QCameraViewfinder* Camera::getViewfinder() {
    return viewfinder;
}

QList<QCameraInfo> Camera::getAllCameras() {
    return QCameraInfo::availableCameras();
}

Camera::~Camera() {
    delete camera;
    delete viewfinder;
    delete imageCapture;
}
