#ifndef WORKER_H
#define WORKER_H

#include <QObject>
#include <QByteArray>
#include <QBuffer>
#include <QJsonObject>
#include <QJsonDocument>
#include <QImage>
#include <QThread>

class Worker : public QObject
{
    Q_OBJECT
public:
    explicit Worker(QObject *parent = nullptr);
    ~Worker();

signals:
    void resultReady(QByteArray result, QThread *thread);

public slots:
    // 处理工作的槽函数
    void doWork(QImage img, QThread * thread);
};

#endif // WORKER_H
