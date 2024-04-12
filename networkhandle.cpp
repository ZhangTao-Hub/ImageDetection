#include "networkhandle.h"

NetWorkHandle::NetWorkHandle(QObject *parent)
    : QObject{parent}
{
    // 网络管理
    // 获取token的网络管理
    tokenManager = new QNetworkAccessManager(this);
    // 获取image的网络管理
    imageManager = new QNetworkAccessManager(this);
    // 输出支持的网络协议 需要自己添加https协议
    // qDebug() << tokenManager->supportedSchemes();

    // url对象
    QUrl url("https://aip.baidubce.com/oauth/2.0/token");
    // url query对象
    QUrlQuery query;
    query.addQueryItem("grant_type", "client_credentials");
    query.addQueryItem("client_id", "zNhZeCbTadjvr1GniL5bvu5P");
    query.addQueryItem("client_secret", "tjEQW00fcjvjt9EtzFvaXxDTZ1HBlunp");
    url.setQuery(query);

    // 是否支持ssl
    // if (QSslSocket::supportsSsl()){
    //     qDebug() << "SSL is supported";
    // } else {
    //     qDebug() << "SSL is not supported";
    // }

    // QString version = QSslSocket::sslLibraryBuildVersionString();
    // qDebug() << version;
    //"OpenSSL 1.1.1g  21 Apr 2020"
    // bool support = QSslSocket::supportsSsl();
    // qDebug() << support;


    // 配置ssl的信息
    sslConfiguration = QSslConfiguration::defaultConfiguration();
    sslConfiguration.setPeerVerifyMode(QSslSocket::QueryPeer);
    sslConfiguration.setProtocol(QSsl::TlsV1_2);

    // 组装url + ssl，发送请求
    QNetworkRequest request(url);
    request.setSslConfiguration(sslConfiguration);
    // 发送get请求
    tokenManager->get(request);
    // qDebug() << "token请求完毕";
}

QString NetWorkHandle::getAccessToken() {
    return accessToken;
}

QNetworkAccessManager * NetWorkHandle::getTokenManager() {
    return tokenManager;
}

QNetworkAccessManager * NetWorkHandle::getImageManager() {
    return imageManager;
}

NetWorkHandle::~NetWorkHandle() {
    delete tokenManager;
    delete imageManager;
}
