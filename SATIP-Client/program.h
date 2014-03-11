#ifndef PROGRAM_H
#define PROGRAM_H

#include "rtspsession.h"
#include <QObject>

enum Msys
{
    DVBT  = 1,
    DVBS  = 2,
    DVBT2 = 4,
    DVBS2 = 8
};
Q_DECLARE_FLAGS(Systems, Msys)
Q_DECLARE_OPERATORS_FOR_FLAGS(Systems)

enum Mtype
{
    M_QPSK = 0,
    M_8PSK = 1,
    M_16QAM = 2
};
Q_DECLARE_FLAGS(Modulations, Mtype)
Q_DECLARE_OPERATORS_FOR_FLAGS(Modulations)

class Event;
class ElementaryStream;
class ProgramPrivate;
class Program : public QObject
{
    Q_OBJECT
public:
    enum Scheme {
        RTSP = 0x0,
        HTTP = 0x1
    };

    enum Purpose {
        Watch = 0x1,
        EPG   = 0x2,
        Scan  = 0x4,
        UpdateNoPid   = 0x8, /* SETUP without PIDs */
        UpdateOnlyPid = 0x10 /* PLAY with PIDs only */
    };
    Q_DECLARE_FLAGS(Purposes, Purpose)

    Program(QObject *parent = 0);
    Program(const Program &other);

    /* DVB-T */
    Program(double frequency, quint8 bandwidth, quint16 transportStreamID, quint16 number, quint16 pid,
            QObject *parent = 0);

    /* DVB-S or DVB-S2 */
    Program(double frequency, quint16 symbolRate, Msys system, Mtype modulation, bool vpolarisation,
            quint8 satPos, quint16 transportStreamID, quint16 number, quint16 pid, QObject *parent = 0);
    ~Program();

    void setFrequency(const double frequency);
    double frequency() const;
    void setBandwidth(const quint8 bandwidth);
    quint8 bandwidth() const;
    void setSymbolRate(const quint16 symbolRate);
    quint16 symbolRate() const;
    void setSystem(const Msys system);
    Msys system() const;
    void setModulation(const Mtype modulation);
    Mtype modulation() const;
    void setVPolarization(const bool vpol);
    bool vPolarization() const;
    void setSatPos(const quint8 satPos);
    quint8 satPos() const;
    void setNumber(const quint16 number);
    quint16 number() const;
    void setTransportStreamID(const quint16 transportStreamID);
    quint16 transportStreamID() const;
    void setPid(const quint16 pid);
    quint16 pid() const;
    void setPcrPid(const quint16 pcrPid);
    quint16 pcrPid() const;
    void setServiceProviderName(const QString &serviceProviderName);
    QString serviceProviderName() const;
    void setServiceName(const QString &serviceName);
    QString serviceName() const;
    void appendElementaryStream(quint16 pid, quint8 type);
    QList<ElementaryStream *> &elementaryStreams() const;
    void clearElementaryStreams();
    void setScrambled(bool value);
    bool scrambled() const;
    bool containsVideo() const;
    bool containsAudio() const;
    quint32 uid() const;

    Program &operator =(const Program &other);

    void appendScanPids(QList<quint16> pids);
    void appendScanPid(quint16 pid);
    void clearScanPids(void);
    QUrl toUrl(const Scheme scheme, const QString &host,
               const Program::Purposes &purposes = Program::Watch, RTSPSession *session = NULL);

    QList<quint16> pidList(const Program::Purposes &purposes = Program::Watch);

protected:
    void init();

protected slots:
    void objectAdded(QObject *object);

signals:
    void programChanged(Program *program);
    void elementaryStreamAdded(ElementaryStream *elementaryStream);
    void newEventAvailable(Event *event);

private:
    ProgramPrivate *d;
};

Q_DECLARE_OPERATORS_FOR_FLAGS(Program::Purposes)
Q_DECLARE_METATYPE(Program)

#endif // PROGRAM_H
