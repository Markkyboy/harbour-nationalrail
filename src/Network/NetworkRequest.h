#ifndef NETWORKREQUEST_H
#define NETWORKREQUEST_H

#include <QObject>
#include <XMLGenerator.h>
#include <QNetworkReply>
#include <QEventLoop>
#include <QDomDocument>
#include <ServiceObject.h>
#include <MessageObject.h>

class NetworkRequest : public QObject
{
    Q_OBJECT
public:
    NetworkRequest();
    Q_INVOKABLE void sendXYZRequest(QString operation,int numRows, QString CRS, QString filterCRS, QString filterType, QString timeOffset, QString timeWindow);
    void processXYZReply();
    QDomDocument xmlDoc;

public slots:
    void XYZReplyFinished(QNetworkReply *reply);
    QList<ServiceObject> getServicesList();

signals:
     void dataProcessed(QList<ServiceObject> services);
     void messagesProcessed(QList<MessageObject> messages);
private:
     QNetworkAccessManager *mgr;
     QEventLoop eventLoop;
     QList<ServiceObject> m_services;
     QList<MessageObject> m_messages;
};

#endif // NETWORKREQUEST_H
