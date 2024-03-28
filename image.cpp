#include "image.h"
#include "ui_image.h"

Image::Image(QWidget *parent): QWidget(parent), ui(new Ui::Image)
{
    ui->setupUi(this);

    //创建封装的相机
    camera = new Camera();

    // 调用页面布局函数
    this->setImageLayout();

    // 全局线程池对象并设定线程个数为4
    threadPool = QThreadPool::globalInstance();
    threadPool->setMaxThreadCount(4);

    // 定时刷新拍照界面
    refreshTimer = new QTimer(this);
    connect(refreshTimer, &QTimer::timeout, this, &Image::takePicture);
    refreshTimer->start(20);

    // 镜头捕捉捕获当前照片
    connect(camera->getImageCapture(), &QCameraImageCapture::imageCaptured, this, &Image::imageCaptured);

    // 点击按钮开始检测
    connect(btnCapture, &QPushButton::click, this, &Image::preparePostData);

    // 网络请求对象
    networkHandle = new NetWorkHandle();
    // token请求完毕
    connect(networkHandle->getTokenManager(), &QNetworkAccessManager::finished, this, &Image::tokenReceived);

    // 在获取token之后出发触发postData的槽函数
    postTimer = new QTimer(this);
    connect(postTimer, &QTimer::timeout, this, &Image::preparePostData);
    connect(postTimer, &QTimer::timeout, this, &Image::preparePostData2);

    // postData完毕 接受图像人脸信息
    connect(networkHandle->getImageManager(), &QNetworkAccessManager::finished, this, &Image::imageReceived);

}

void Image::imageCaptured(int id, QImage img) {
    Q_UNUSED(id);
    this->img = img;
    // label中显示捕获的图片

    // 在img上画出人脸矩形框
    QPainter p(&img);
    p.setPen(QPen(Qt::red, 2));
    p.drawRect(faceLeft, faceTop, faceWidth, faceHeight);

    // 设置画笔的字体大小
    QFont f = p.font();
    f.setPixelSize(30);
    p.setFont(f);

    // 在旁边标注上信息
    p.drawText(faceLeft+faceWidth+5, faceTop, QString("%1").arg(age));
    p.drawText(faceLeft+faceWidth+5, faceTop+40, QString("%1").arg(gender));
    p.drawText(faceLeft+faceWidth+5, faceTop+80, QString("%1").arg(emotion));
    p.drawText(faceLeft+faceWidth+5, faceTop+120, QString("%1").arg(beauty));
    label->setPixmap(QPixmap::fromImage(this->img));
}

void Image::takePicture() {
    // 捕获当前图片
    camera->getImageCapture()->capture();
}

/*页面整体布局*/
void Image::setImageLayout() {

    // 设置布局控件
    label = new QLabel(this);
    label->setFixedSize(640, 480);
    btnCapture = new QPushButton("拍照");
    textBrowser = new QTextBrowser(this);
    textBrowser->setFixedSize(250, 300);
    comboBox = new QComboBox(this);

    // 查看所有的相机设备
    auto supportedCameras = camera->getAllCameras();
    for (const auto &item: supportedCameras) {
        // 相机复选框中添加可用相机
        comboBox->addItem(item.description(), QVariant(item.deviceName()));
    }

    // 开始界面布局
    // 左侧
    this->resize(1000, 800);
    vBoxLeft = new QVBoxLayout;
    vBoxLeft->addWidget(label);
    vBoxLeft->addWidget(btnCapture);

    // 右侧
    vBoxRight = new QVBoxLayout;
    vBoxRight->addWidget(comboBox);
    vBoxRight->addWidget(camera->getViewfinder());
    vBoxRight->addWidget(textBrowser);

    // 整体布局
    layout = new QHBoxLayout(this);
    layout->addLayout(vBoxLeft);
    layout->addLayout(vBoxRight);
    this->setLayout(layout);
    this->setFixedSize(1000, 600);
}

void Image::tokenReceived(QNetworkReply *reply){
    // 检查是否有错误
    if (reply->error() != QNetworkReply::NoError) {
        qDebug() << "Token received error:" << reply->errorString();
        return;
    }
    // 读取返回数据
    QByteArray replyData = reply->readAll();
    qDebug() << "Token received:" << replyData;

    // 解析json
    QJsonParseError jsonError;
    QJsonDocument json_doc = QJsonDocument::fromJson(replyData, &jsonError);
    if (jsonError.error == QJsonParseError::NoError) {
        QJsonObject json_obj = json_doc.object();
        // 检查返回的json是否包含access_token
        if (json_obj.contains("access_token")) {
            networkHandle->getAccessToken() = json_obj.take("access_token").toString();
        }
        // 将accessToken显示在textBrowser中
        textBrowser->setText(networkHandle->getAccessToken());
    } else {
        qDebug() << "JSON parsing error:" << jsonError.errorString();
    }

    reply->deleteLater();
    postTimer->start(1500);  // 获取token之后开启定时
}


// 创建线程的方法
void Image::preparePostData() {
    // 创建线程
    workThread = new QThread;
    // 创建工人
    worker = new Worker;
    // 将工人放进线程里
    worker->moveToThread(workThread);

    // 通知工人干活
    connect(this, &Image::beginWork, worker, &Worker::doWork);
    // 结束工作后，将数据传给下一个检测的槽函数
    connect(worker, &Worker::resultReady, this, &Image::beginFaceDetection);
    // 处理线程资源函数
    connect(workThread, &QThread::finished, worker, &QObject::deleteLater);

    // 开启线程
    workThread->start();
    // 发出信号
    emit beginWork(img, workThread);
}

// 使用线程池的方法
void Image::preparePostData2() {
    worker2 = new Worker2(img);
    // 调用 start() 方法将任务提交给线程池
    threadPool->start(worker2);

    // 结束工作后，将数据传给下一个检测的槽函数
    connect(worker2, &Worker2::resultReady, this, &Image::beginFaceDetection2);
}

void Image::beginFaceDetection(QByteArray postData, QThread *thread) {

    // 关闭子线程
    thread->quit();
    thread->wait();

    // 拼接url
    QUrl url("https://aip.baidubce.com/rest/2.0/face/v3/detect");
    QUrlQuery query(url);
    query.addQueryItem("access_token", networkHandle->getAccessToken());
    url.setQuery(query);

    // 发送请求
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader,QVariant("application/json"));
    networkHandle->getImageManager()->post(request, postData);
}

void Image::beginFaceDetection2(QByteArray postData) {

    // 拼接url
    QUrl url("https://aip.baidubce.com/rest/2.0/face/v3/detect");
    QUrlQuery query(url);
    query.addQueryItem("access_token", networkHandle->getAccessToken());
    url.setQuery(query);

    // 发送请求
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader,QVariant("application/json"));
    networkHandle->getImageManager()->post(request, postData);
}

void Image::imageReceived(QNetworkReply* reply){
    // 定义一个 QByteArray 类型的变量 replyData，用于存储回复数据
    QByteArray replyData = reply->readAll();
    // 检查网络请求是否成功
    if (reply->error() != QNetworkReply::NoError){
        // 请求失败，打印错误信息并返回
        qDebug() << reply->errorString();
        return;
    }
    qDebug() << "replyData = " << replyData;

    // JSON 解析错误处理
    // 定义一个 QJsonParseError 类型的变量 error，用于存储 JSON 解析错误信息
    QJsonParseError error;

    // 将回复数据 replyData 解析为 JSON 文档 jsonDoc
    QJsonDocument jsonDoc = QJsonDocument::fromJson(replyData, &error);

    // 检查 JSON 解析是否成功
    if (error.error != QJsonParseError::NoError) {
        // 解析失败，打印错误信息并返回
        qDebug() << "JSON 解析错误：" << error.errorString();
        return;
    }

    // 将 JSON 文档 jsonDoc 转换为 JSON 对象 jsonObj
    QJsonObject jsonObj = jsonDoc.object();

    // 人脸信息
    QString faceInfo;

    // 处理JSON数据
    if (jsonObj.contains("result")) {
        QJsonObject resultObj = jsonObj.take("result").toObject();

        if (resultObj.contains("timestamp")) {
            int tmpTime = resultObj.take("timestamp").toInt();
            if (tmpTime < lastTime) {
                return;
            } else {
                lastTime = tmpTime;
            }
        }

        // 取出人脸列表
        if (resultObj.contains("face_list")) {
            QJsonArray faceArray = resultObj.take("face_list").toArray();
            // 拿到第一张人脸
            QJsonObject faceObj = faceArray.at(0).toObject();

            // 拿到人脸定位
            if (faceObj.contains("location")) {
                QJsonObject locationObj = faceObj.take("location").toObject();
                // x轴左边
                if (locationObj.contains("left")) {
                    faceLeft = locationObj.take("left").toDouble();
                }

                // y轴坐标
                if (locationObj.contains("top")) {
                    faceTop = locationObj.take("top").toDouble();
                }

                // 宽度
                if (locationObj.contains("width")) {
                    faceWidth = locationObj.take("width").toDouble();
                }

                // 高度
                if (locationObj.contains("height")) {
                    faceHeight = locationObj.take("height").toDouble();
                }
            }

            // 取出年龄信息
            if (faceObj.contains("age")) {
                age = faceObj.take("age").toDouble();
                faceInfo += QString("年龄：%1\n").arg(age);
            }

            // 取出性别信息
            if (faceObj.contains("gender")) {
                QJsonObject genderObj = faceObj.take("gender").toObject();
                if (genderObj.contains("type")) {
                    gender = genderObj.take("type").toString();
                    faceInfo += QString("性别：%1\n").arg(gender);
                }
            }

            // 取出表情
            if (faceObj.contains("emotion")) {
                QJsonObject emotionObj = faceObj.take("emotion").toObject();
                if (emotionObj.contains("type")) {
                    emotion = emotionObj.take("type").toString();
                    faceInfo += QString("表情：%1\n").arg(emotion);
                }
            }

            // 取出是否带口罩
            if (faceObj.contains("mask")) {
                QJsonObject maskObj =  faceObj.take("mask").toObject();
                if (maskObj.contains("type")) {
                    mask = maskObj.take("type").toInt();
                    if (mask == 1) {
                        faceInfo += QString("戴口罩");
                    } else {
                        faceInfo += QString("未戴口罩");
                    }

                }
            }

            // 取出颜值
            if (faceObj.contains("beauty")) {
                beauty = faceObj.take("beauty").toDouble();
                faceInfo += QString("颜值：%1\n").arg(beauty);
            }
        }
    }

    textBrowser->setText(faceInfo);
    reply->deleteLater();
}

Image::~Image()
{
    delete ui;
    delete camera;
    delete networkHandle;
    delete refreshTimer;
    delete postTimer;
    delete worker;
    delete workThread;

    threadPool->waitForDone(); // 等待所有任务完成
}
