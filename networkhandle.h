#ifndef NETWORKHANDLE_H
#define NETWORKHANDLE_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QSslSocket>
#include <QUrl>
#include <QUrlQuery>

class NetWorkHandle : public QObject
{
    Q_OBJECT
public:
    explicit NetWorkHandle(QObject *parent = nullptr);
    ~NetWorkHandle();

    QString getAccessToken();
    QNetworkAccessManager *getTokenManager();
    QNetworkAccessManager *getImageManager();

private:
    // 网络访问管理
    QNetworkAccessManager *tokenManager;
    QNetworkAccessManager *imageManager;

    // ssl配置
    QSslConfiguration sslConfiguration;

    // accessToken
    QString accessToken;

signals:


private slots:


};

#endif // NETWORKHANDLE_H
