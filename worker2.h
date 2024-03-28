#ifndef WORKER2_H
#define WORKER2_H

#include <QRunnable>
#include <QImage>
#include <QBuffer>
#include <QJsonObject>
#include <QJsonDocument>

class Worker2 : public QRunnable
{
public:
    Worker2(QImage img);
    void run() override;

signals:
    void resultReady(QByteArray result);

private:
    QImage m_img;
};

#endif // WORKER2_H
