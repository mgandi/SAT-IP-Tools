#include "settings.h"
#include "program.h"
#include "elementarystream.h"
#include "gatewaydevice.h"

Settings::Settings(QObject *parent) :
    QSettings(QSettings::IniFormat, QSettings::UserScope, "ALi", "SAT>IP Light Client", parent)
{
    // Register Program type if not yet registered;
    if (QMetaType::type("Program") == QMetaType::UnknownType) {
        qRegisterMetaType<Program>("Program");
    }
}

Settings::~Settings()
{
}


QString Settings::vlc() const
{
    return value("vlc", "/Applications/VLC.app/Contents/MacOS/VLC").toString();
}

qint16 Settings::httpPort() const
{
    return value("http port", 80).toUInt();
}

QList<Program *> Settings::programs(const QString &uid)
{
    Program *program = NULL;
    beginGroup(uid);

    QList<Program *> prgms;

    int nbProg = beginReadArray("programs");
    for (int i = 0; i < nbProg; ++i) {
        setArrayIndex(i);
        double frequency = value("frequency").toDouble();
        quint8 bandwidth = value("bandwidth").toUInt();
        Msys system = (Msys)value("system").toUInt();
        Mtype modulation = (Mtype)value("modulation").toUInt();
        quint16 symbolRate = value("symbolRate").toUInt();
        bool vPolarization = value("vPolarization").toBool();
        quint8 satPos = value("satPos").toUInt();
        quint16 transportStreamID = value("transportStreamID").toUInt();
        quint16 number = value("number").toUInt();
        quint16 pid = value("pid").toUInt();
        quint16 pcrPid = value("pcrPid").toUInt();
        QString serviceProviderName = value("serviceProviderName").toString();
        QString serviceName = value("serviceName").toString();

        switch (system) {
        case DVBT:
            program = new Program(frequency, bandwidth, transportStreamID, number, pid);
            break;
        case DVBS:
        case DVBS2:
            program = new Program(frequency, symbolRate, system, modulation, vPolarization, satPos,
                                  transportStreamID, number, pid);
            break;
        }
        program->setPcrPid(pcrPid);
        program->setServiceProviderName(serviceProviderName);
        program->setServiceName(serviceName);
        program->setScrambled(value("scrambled").toUInt());

        int nbEss = beginReadArray("es");
        for (int i = 0; i < nbEss; i++) {
            setArrayIndex(i);
            quint16 pid = value("pid").toUInt();
            quint8 type = value("type").toUInt();
            program->appendElementaryStream(pid, type);
        }
        endArray();

        prgms += program;
    }
    endArray();

    endGroup();

    return prgms;
}

QString Settings::lastUsedDevice()
{
    return value("lastUsedDevice", "").toString();
}


void Settings::setVlc(const QString &vlc)
{
    setValue("vlc", vlc);
}

void Settings::setPrograms(const QString &uid, const QList<Program *> &programs)
{
    beginGroup(uid);

    beginWriteArray("programs");
    int i = 0;
    foreach (Program *program, programs) {
        setArrayIndex(i++);
        setValue("frequency", program->frequency());
        setValue("bandwidth", program->bandwidth());
        setValue("system", program->system());
        setValue("modulation", program->modulation());
        setValue("symbolRate", program->symbolRate());
        setValue("vPolarization", program->vPolarization());
        setValue("satPos", program->satPos());
        setValue("transportStreamID", program->transportStreamID());
        setValue("number", program->number());
        setValue("pid", program->pid());
        setValue("pcrPid", program->pcrPid());
        setValue("serviceProviderName", program->serviceProviderName());
        setValue("serviceName", program->serviceName());
        setValue("scrambled", program->scrambled());
        beginWriteArray("es");
        int j = 0;
        foreach (ElementaryStream *es, program->elementaryStreams()) {
            setArrayIndex(j++);
            setValue("pid", es->pid());
            setValue("type", es->type());
        }
        endArray();
    }
    endArray();

    endGroup();
}

void Settings::setLastUsedDevice(const QString &uid)
{
    setValue("lastUsedDevice", uid);
}
