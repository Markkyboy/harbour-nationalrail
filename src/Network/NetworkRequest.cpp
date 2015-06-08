#include "NetworkRequest.h"
#include <QDebug>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QEventLoop>
#include <QDomDocument>

#include <XMLGenerator.h>
#include <ServiceObject.h>

NetworkRequest::NetworkRequest()
{
}

void NetworkRequest::sendXYZRequest(QString operation,int numRows, QString CRS, QString filterCRS, QString filterType, QString timeOffset, QString timeWindow){

    QString url = "https://lite.realtime.nationalrail.co.uk/OpenLDBWS/ldb6.asmx";

    XMLGenerator generator;

    QByteArray xml=generator.createOperationXML(operation,numRows, CRS, filterCRS, filterType, timeOffset, timeWindow);

    mgr = new QNetworkAccessManager(this);

    QNetworkRequest request;
    request.setHeader(QNetworkRequest::ContentTypeHeader, "text/xml");
    request.setUrl(url);

    QSslConfiguration config = request.sslConfiguration();
    config.setPeerVerifyMode(QSslSocket::VerifyNone);
    config.setProtocol(QSsl::TlsV1_0);
    request.setSslConfiguration(config);

    mgr->post(request, xml);
    connect(mgr, SIGNAL(finished(QNetworkReply*)),
            this, SLOT(XYZReplyFinished(QNetworkReply*)));
    eventLoop.exec();    
}

void NetworkRequest::XYZReplyFinished(QNetworkReply *reply){
    if (reply->error() == QNetworkReply::NoError) {
        xmlDoc.setContent(reply->readAll(),0,0,0,0);
        delete reply;
        //qDebug() << xmlDoc.toString(1);
        eventLoop.exit(0);
        processXYZReply();
    }
    else {
        qDebug() << "Failure" <<reply->errorString();
        delete reply;
        eventLoop.exit(0);
    }

}

void NetworkRequest::processXYZReply(){

    m_services.clear();

    QDomNodeList services = xmlDoc.elementsByTagName("service");
    for (int i = 0; i < services.size(); i++) {
        QDomNode service = services.item(i);

        //Need another for loop here, just for those two
        QDomElement origin = service.firstChildElement("origin");
        QDomNode location = origin.firstChild();
        QDomElement locationName = location.firstChildElement("locationName");
        QDomElement locationCRS =  location.firstChildElement("crs");

        QDomElement destination= service.firstChildElement("destination");
        QDomNode destinationNode = destination.firstChild();
        QDomElement destinationName = destinationNode.firstChildElement("locationName");
        QDomElement destinationCRS =  destinationNode.firstChildElement("crs");


        //Changes between sta and std but actually both can be used.
        QDomElement std = service.firstChildElement("std");
        QDomElement etd = service.firstChildElement("etd");

        QDomElement sta = service.firstChildElement("sta");
        QDomElement eta = service.firstChildElement("eta");
        QDomElement platform = service.firstChildElement("platform");
        QDomElement trainOperator = service.firstChildElement("operator");
        QDomElement operatorCode = service.firstChildElement("operatorCode");
        QDomElement serviceID = service.firstChildElement("serviceID");

        ServiceObject object(locationName.text(),locationCRS.text(), destinationName.text(),destinationCRS.text(), std.text(),etd.text(),sta.text(),eta.text(),platform.text(),trainOperator.text(),operatorCode.text(),serviceID.text());

        m_services.append(object);

    }

    emit dataProcessed(m_services);
}

QList<ServiceObject> NetworkRequest::getServicesList(){
    return m_services;
}




