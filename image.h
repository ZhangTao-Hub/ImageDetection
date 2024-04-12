#ifndef IMAGE_H
#define IMAGE_H

#include <QWidget>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPushButton>
#include <QTextBrowser>
#include <QLabel>
#include <QTimer>
#include <QDebug>
#include <QJsonDocument>
#include <QJsonParseError>
#include <QJsonObject>
#include <QImage>
#include <QBuffer>
#include <QJsonArray>
#include <QThread>
#include <QPainter>
#include <QCameraInfo>
#include <QComboBox>
#include "worker.h"
#include "worker2.h"
#include "camera.h"
#include "networkhandle.h"
#include <QNetworkReply>
#include <QThreadPool>
#include <QRunnable>

QT_BEGIN_NAMESPACE
namespace Ui {
class Image;
}
QT_END_NAMESPACE

class Image : public QWidget
{
    Q_OBJECT

public:
    Image(QWidget *parent = nullptr);
    ~Image();

    // 页面布局函数，封装页面布局相关操作
    void setImageLayout();

private:
    Ui::Image *ui; // UI
    Camera *camera; // 相机
    QTimer *refreshTimer; // 刷新定时器
    QTimer *postTimer; // postData定时器
    QImage img; // 检测图片
    NetWorkHandle *networkHandle;// 网络部分
    int lastTime; // 最后一次检测时间

    // 人脸信息
    double age;
    QString gender;
    QString emotion;
    int mask;
    double beauty;

    // 人脸位置
    double faceLeft;
    double faceTop;
    double faceWidth;
    double faceHeight;

    // 线程部分
    QThread *workThread;
    Worker *worker;

    // 使用线程池重构
    QThreadPool *threadPool;
    Worker2 *worker2;

    // 控件
    QPushButton *btnCapture;
    QLabel *label;
    QTextBrowser *textBrowser;
    QComboBox *comboBox;

    // 布局
    QHBoxLayout *layout;
    QVBoxLayout *vBoxLeft;
    QVBoxLayout *vBoxRight;

signals:
    void beginWork(QImage img, QThread *workThread);


private slots:
    // 处理图像捕捉的槽函数
    void imageCaptured(int id, QImage img);

    // 定时采集取景器中的图片
    void takePicture();

    // 处理token请求完毕的槽函数
    void tokenReceived(QNetworkReply *);

    // 处理获取postData数据的槽函数
    void preparePostData();
    void preparePostData2(); // 使用线程池重构

    // 处理工人线程返回的数据
    void beginFaceDetection(QByteArray postData, QThread *thread);
    void beginFaceDetection2(QByteArray postData);

    // 处理返回的图片信息
    void imageReceived(QNetworkReply* reply);

};
#endif // IMAGE_H
