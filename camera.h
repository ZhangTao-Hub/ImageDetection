#ifndef CAMERA_H
#define CAMERA_H

#include <QWidget>
#include <QCamera>
#include <QCameraInfo>
#include <QCameraViewfinder>
#include <QCameraImageCapture>

/*
 *  自定义相机类，封装相机相关信息
*/
class Camera : public QWidget
{
    Q_OBJECT
public:
    explicit Camera(QWidget *parent = nullptr);
    ~Camera();

private:
    // 照相机相关 照相机 取景器 镜头捕捉器
    QCamera *camera;
    QCameraViewfinder *viewfinder;
    QCameraImageCapture *imageCapture;

public:
    // 获取取景器
    QCameraViewfinder *getViewfinder();

    // 获取镜头捕捉
    QCameraImageCapture *getImageCapture();

    // 获取在线的所有相机设备
    QList<QCameraInfo> getAllCameras();

};

#endif // CAMERA_H
