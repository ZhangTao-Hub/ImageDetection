#include "worker2.h"

Worker2::Worker2(QImage img): m_img(img)
{

}

void Worker2::run()
{
    // 这一段代码太消耗事件, 开一个线程, 单独处理

    // 转成64编码
    QByteArray ba;
    QBuffer buffer(&ba);
    m_img.save(&buffer, "png");
    QString base64_img = ba.toBase64();

    //body封装
    QJsonObject body;
    QJsonDocument doc;
    body.insert("image", base64_img);
    body.insert("image_type", "BASE64");
    body.insert("face_field", "age,beauty,expression,face_shape,gender,glasses,quality,eye_status,emotion,face_type,mask,spoofing");
    doc.setObject(body);
    QByteArray postData = doc.toJson(QJsonDocument::Compact);

    emit resultReady(postData);
}


