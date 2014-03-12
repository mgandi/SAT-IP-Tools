#include "gatewaydevice.h"
#include "rtspsession.h"
#include "sessionmodel.h"
#include "program.h"
#include "settings.h"
#include "objectpool.h"
#include "ssdpclient.h"

#include <QUrl>
#include <QUrlQuery>
#include <QBuffer>
#include <QtDebug>
#include <QtNetwork/QNetworkRequest>
#include <QtNetwork/QNetworkReply>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QHostAddress>
#include <QtXml/QXmlSimpleReader>
#include <QPlainTextEdit>
#include <QHash>

class GatewayXmlContentHandler: public QXmlDefaultHandler
{
public:
    GatewayXmlContentHandler(GatewayDevicePrivate *d);

    bool startElement(const QString &namespaceURI, const QString &localName, const QString &qName, const QXmlAttributes &atts);
    bool endElement(const QString &namespaceURI, const QString &localName, const QString &qName);
    bool characters(const QString &ch);

private:
    GatewayDevicePrivate *device;
    QStringList prefixes;
    QString currentText;
};



class GatewayDevicePrivate
{
public:
    GatewayDevicePrivate(GatewayDevice *parent);

    GatewayDevice *p;
    QNetworkAccessManager manager;
    SessionModel model;
    QList<Program *> programs;

    QUrl location;
    QString searchTarget;
    QByteArray description;
    QString major;
    QString minor;
    QString deviceType;
    QString friendlyName;
    QString manufacturer;
    QString manufacturerURL;
    QString modelDescription;
    QString modelName;
    QString modelNumber;
    QString modelURL;
    QString serialNumber;
    QString UDN;
    QString UPC;
    QString presentationURL;
    QString capability;
    QHash<Msys, quint32> fes;
};



GatewayXmlContentHandler::GatewayXmlContentHandler(GatewayDevicePrivate *d):
    device(d)
{
}


bool GatewayXmlContentHandler::startElement(const QString &, const QString &, const QString &qName, const QXmlAttributes &)
{
    if ((qName == "root") || (qName == "specVersion") || (qName == "device")) {
        prefixes += qName;
        return true;
    }

    currentText.clear();

    return true;
}

bool GatewayXmlContentHandler::endElement(const QString &, const QString &, const QString &qName)
{
    if ((qName == "root") || (qName == "specVersion") || (qName == "device")) {
        if (prefixes.last() != qName)
            return false;
        prefixes.removeLast();
        currentText.clear();
        return true;
    }

    QString element = prefixes.join("/").append("/").append(qName);

    if (element == "root/specVersion/major") {
        device->major = currentText;
    } else if (element == "root/specVersion/minor") {
        device->minor = currentText;
    } else if (element == "root/device/deviceType") {
        device->deviceType = currentText;
    } else if (element == "root/device/friendlyName") {
        device->friendlyName = currentText;
    } else if (element == "root/device/manufacturer") {
        device->manufacturer = currentText;
    } else if (element == "root/device/manufacturerURL") {
        device->manufacturerURL = currentText;
    } else if (element == "root/device/modelDescription") {
        device->modelDescription = currentText;
    } else if (element == "root/device/modelName") {
        device->modelName = currentText;
    } else if (element == "root/device/modelNumber") {
        device->modelNumber = currentText;
    } else if (element == "root/device/modelURL") {
        device->modelURL = currentText;
    } else if (element == "root/device/serialNumber") {
        device->serialNumber = currentText;
    } else if (element == "root/device/UDN") {
        device->UDN = currentText;
    } else if (element == "root/device/UPC") {
        device->UPC = currentText;
    } else if (element == "root/device/presentationURL") {
        device->presentationURL = currentText;
    } else if (element == "root/device/satip:X_SATIPCAP") {
        device->capability = currentText;
        if (!device->capability.isEmpty()) {
            QRegExp regExp("(DVB(S|S2|T|T2))-(\\d+)");
            QStringList capabilities = device->capability.split(QRegExp("\\s+,\\s+"), QString::SkipEmptyParts);

            foreach (QString capability, capabilities) {
                regExp.indexIn(capability);
                if (regExp.captureCount() == 3) {
                    if (regExp.cap(1) == "DVBT") {
                        device->fes.insert(DVBT, regExp.cap(3).toUInt());
                    } else if (regExp.cap(1) == "DVBS") {
                        device->fes.insert(DVBS, regExp.cap(3).toUInt());
                    } else if (regExp.cap(1) == "DVBT2") {
                        device->fes.insert(DVBT2, regExp.cap(3).toUInt());
                    } else if (regExp.cap(1) == "DVBS2") {
                        device->fes.insert(DVBS2, regExp.cap(3).toUInt());
                    }
                }
            }
        }
    }

    return true;
}

bool GatewayXmlContentHandler::characters(const QString &ch)
{
    currentText += ch;
    return true;
}




GatewayDevicePrivate::GatewayDevicePrivate(GatewayDevice *parent):
    p(parent),
    manager(p),
    model(p)
{
}



GatewayDevice::GatewayDevice(const QUrl &location, const QString &searchTarget, SSDPClient *parent) :
    QObject(parent),
    d(new GatewayDevicePrivate(this))
{
    d->location = location;
    d->searchTarget = searchTarget;

    // Get description XML file
    QNetworkRequest request(QUrl(d->location));
    QNetworkReply *reply = d->manager.get(request);
    connect(reply, SIGNAL(finished()),
            this, SLOT(descriptionReplyFinished()));
}

GatewayDevice::~GatewayDevice()
{
    clearPrograms();
    delete d;
}


QString GatewayDevice::host() const
{
    return d->location.host();
}

QString GatewayDevice::description() const
{
    return d->description;
}

QString GatewayDevice::localAddress() const
{
    return qobject_cast<SSDPClient *>(parent())->localAddress();
}


QString GatewayDevice::friendlyName() const
{
    return d->friendlyName;
}

QString GatewayDevice::manufacturer() const
{
    return d->manufacturer;
}

QString GatewayDevice::modelDescription() const
{
    return d->modelDescription;
}

QString GatewayDevice::deviceType() const
{
    return d->deviceType;
}

QString GatewayDevice::modelNumber() const
{
    return d->modelNumber;
}

QString GatewayDevice::modelName() const
{
    return d->modelName;
}

QString GatewayDevice::serialNumber() const
{
    return d->serialNumber;
}

Systems GatewayDevice::capabilities() const
{
    QList<Msys> msyss = d->fes.keys();
    Systems systems = 0;
    foreach (Msys msys, msyss) {
        systems |= msys;
    }
    return systems;
}

quint32 GatewayDevice::nbFEForSystem(Msys msys) const
{
    return d->fes.value(msys, 0);
}

QString GatewayDevice::uid() const
{
    QString uid;
    uid += d->modelName;
    uid += d->modelNumber;
    uid += d->serialNumber;
    uid = QString(uid.toLatin1().toBase64());
    return uid;
}

QList<Program *> GatewayDevice::programs() const
{
    return d->programs;
}


SessionModel *GatewayDevice::sessionModel()
{
    return &d->model;
}


RTSPSession *GatewayDevice::addSession(Program *program, bool unicast, const Program::Purposes &purposes)
{
    QUrl url = program->toUrl(Program::RTSP, host(), purposes);
    RTSPSession *session = new RTSPSession(localAddress(), url, unicast, true, this);
    if (d->model.addSession(session)) {
        return session;
    } else {
        session->stop();
//        session->wait();
        delete session;
        return 0;
    }
}

RTSPSession *GatewayDevice::addSession(const RequestParams &requestParams)
{
    QUrl url = requestToUrl(requestParams);
    RTSPSession *session = new RTSPSession(localAddress(), url, requestParams.unicast, true, this);
    if (d->model.addSession(session)) {
        return session;
    } else {
        session->stop();
//        session->wait();
        delete session;
        return 0;
    }
}

RTSPSession *GatewayDevice::addSession(RTSPSession *session)
{
    if (d->model.addSession(session)) {
        return session;
    } else {
        return 0;
    }
}

bool GatewayDevice::updateSession(RTSPSession *session, Program *program, int zapPeriod)
{
    if (!session->ready())
        return false;

    if (zapPeriod) {
        QUrl url = program->toUrl(Program::RTSP, host(), Program::Watch, session);
        session->zapUrl(url, zapPeriod);
    } else {
#if 1
        /* PLAY with new parameters */
        QUrl url = program->toUrl(Program::RTSP, host(), Program::Watch, session);
        session->setUrl(url); /* PLAY */
#else
        /* SETUP with new parameters and no PIDs */
        QUrl url = program->toUrl(Program::RTSP, host(), Program::Watch | Program::UpdateNoPid, session);
        session->updateSetup(url); /* SETUP */

        /* PLAY with only news PIDS */
        url = program->toUrl(Program::RTSP, host(), Program::Watch | Program::UpdateOnlyPid, session);
        session->setUrl(url); /* PLAY */
#endif
    }
    return true;
}


bool GatewayDevice::removeSession(RTSPSession *session, bool force)
{
    if ((session->parent() != this) && !force)
        return false;

    bool ret = d->model.removeSession(session);
    if (ret && !force) {
        session->stop();
//        session->wait();
        delete session;
    }
    return ret;
}

void GatewayDevice::savePrograms()
{
    Settings settings;
    settings.setPrograms(uid(), d->programs);
}

void GatewayDevice::clearPrograms()
{
    foreach (Program *program, d->programs) {
        ObjectPool::instance()->removeObject(program);
    }

    qDeleteAll(d->programs);
    d->programs.clear();
}

void GatewayDevice::addProgram(Program *program)
{
    d->programs += program;
    ObjectPool::instance()->addObject(program);
}



QUrl GatewayDevice::requestToUrl(const RequestParams &requestParams)
{
    QUrl url;
    QUrlQuery urlQuery;

    /* Create url query */
    switch (requestParams.msys) {
    case DVBS:
    case DVBS2:
        urlQuery.addQueryItem("fe", QString("%1").arg(requestParams.fe));
        urlQuery.addQueryItem("msys", (requestParams.msys == DVBS) ? "dvbs" : "dvbs2");
        urlQuery.addQueryItem("src", QString("%1").arg(requestParams.src));
        urlQuery.addQueryItem("pol", requestParams.vpol ? "v" : "h");
        switch (requestParams.mtype) {
        case M_QPSK:
            urlQuery.addQueryItem("mtype", "qpsk");
            break;
        case M_8PSK:
            urlQuery.addQueryItem("mtype", "8psk");
            break;
        case M_16QAM:
            urlQuery.addQueryItem("mtype", "16qam");
            break;
        }
        switch (requestParams.fec) {
        case F_12:
            urlQuery.addQueryItem("fec", "12");
            break;
        case F_23:
            urlQuery.addQueryItem("fec", "23");
            break;
        case F_34:
            urlQuery.addQueryItem("fec", "34");
            break;
        case F_56:
            urlQuery.addQueryItem("fec", "56");
            break;
        case F_78:
            urlQuery.addQueryItem("fec", "78");
            break;
        case F_910:
            urlQuery.addQueryItem("fec", "78");
            break;
        case F_AUTO:
            urlQuery.addQueryItem("fec", "auto");
            break;
        }
        urlQuery.addQueryItem("sr", QString("%1").arg(requestParams.sr));
        break;
    case DVBT:
        urlQuery.addQueryItem("msys", "dvbt");
        urlQuery.addQueryItem("bw", QString("%1").arg(requestParams.bw));
        break;
    case DVBT2:
        break;
    }
    urlQuery.addQueryItem("freq", QString("%1").arg(requestParams.freq));
    urlQuery.addQueryItem("pids", requestParams.pids);

    /* Create url */
    if (requestParams.rtp) {
        url.setScheme("rtsp");
    } else {
        url.setScheme("http");
    }
    url.setHost(host());
    url.setPath("/");
    url.setQuery(urlQuery);

    return url;
}

void GatewayDevice::loadPrograms()
{
    d->programs.clear();

    Settings settings;
    d->programs = settings.programs(uid());

    foreach (Program *program, d->programs) {
        ObjectPool::instance()->addObject(program);
    }
}

void GatewayDevice::descriptionReplyFinished()
{
    // Get associated network reply
    QNetworkReply *reply = qobject_cast<QNetworkReply *>(sender());

    // Extract XML description
    d->description = reply->readAll();

    // Parse XML description
    QXmlSimpleReader reader;
    GatewayXmlContentHandler contentHandler(d);
    reader.setContentHandler(&contentHandler);
    reader.parse(new QXmlInputSource(new QBuffer(&d->description, this)), false);

    // Will delete network reply later
    reply->deleteLater();

    // Load programs associated to current devcie
    loadPrograms();

    // We can advertise current device
    emit ready(this);
}
