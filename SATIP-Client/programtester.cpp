#include "programtester.h"
#include "gatewaydevice.h"
#include "event.h"
#include "program.h"
#include "rtspsession.h"

#include <QDateTime>
#include <QTimer>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QNetworkAccessManager>
#include <QFile>
#include <QDir>
#include <QTimerEvent>


#define C_SYNCH_BYTE       0x47
#define C_STATE_MASK       0xFF000000
#define C_STATE_SUB_MASK   0x00FFFFFF

#define C_STATE_NO_SYNCH   0x01000000
#define C_STATE_HEAD_PID   0x02000000
#define C_STATE_HEAD_SEQU  0x03000000
#define C_STATE_PAYL_SEQU  0x04000000
#define C_STATE_PAYL_DATA  0x05000000

#define C_TS_DATA_END_SEQU_OFFS (188-3)


class ProgramTesterPrivate
{
public:

    ProgramTesterPrivate(GatewayDevice *d, Program *prog, Program::Scheme scheme, ProgramTester *parent) :
        p(parent),
        device(d),
        event(0),
        program(prog),
        manager(p),
        scheme(scheme),
        session(0),

        numTs(912), pidRef(17),

        tsCnt(0), tsOffs(0), err(0),
        pseq_ref(0x80000000),
        state(C_STATE_NO_SYNCH),
        pid(0), hseq(0x10), pseq(0),

        status(ProgramTester::Idle)
    {}

    ProgramTester *p;
    GatewayDevice *device;
    Event *event;
    Program *program;
    QNetworkAccessManager manager;
    QNetworkReply *reply;
    ProgramTester::Status status;
    Program::Scheme scheme;

    RTSPSession *session;

    /* Config */
    quint32 numTs, pidRef, maxTs;
 
    /* result */ 
    quint32 tsCnt, tsOffs, err, progress;
    quint32 pseq_ref;  
    int state; 
    quint16 pid;
    quint8  hseq;
    quint32 pseq;
};


ProgramTester::ProgramTester(GatewayDevice *device, Program *program,
                             Program::Scheme scheme, QObject *parent) :
    QObject(parent),
    d(new ProgramTesterPrivate(device, program, scheme, this))
{
}

ProgramTester::~ProgramTester()
{
    delete d;
}

Program *ProgramTester::program() const
{
    return d->program;
}

ProgramTester::Status ProgramTester::status() const
{
    return d->status;
}
quint32 ProgramTester::pidRef() const
{
    return d->pidRef;
}

quint32 ProgramTester::numTs() const
{
    return d->numTs;
}

quint32 ProgramTester::tsCnt() const
{
    return d->tsCnt;
}

quint32 ProgramTester::tsOffs() const
{
    return d->tsOffs;
}

quint32 ProgramTester::err() const
{
    return d->err;
}

void ProgramTester::start()
{
    if (d->scheme == Program::HTTP) {
        QUrl url = d->program->toUrl(Program::HTTP, d->device->host());
        QNetworkRequest request(url);
        d->reply = d->manager.get(request);
        connect(d->reply, SIGNAL(readyRead()),
                this, SLOT(readyRead()));
        connect(d->reply, SIGNAL(finished()),
                this, SLOT(finished()));
    } else {
        if (d->program) {
            d->session = d->device->addSession(d->program);
            connect(d->session, SIGNAL(packetsAvailable(quint32,QList<QByteArray>&)),
                    this, SLOT(packetsAvailable(quint32,QList<QByteArray>&)));
        }
    }

    d->status = Testing;
    emit statusChanged(d->status);
}

void ProgramTester::stop()
{
    if (d->scheme == Program::HTTP) {
        if (d->reply) {
            d->reply->close();
            d->reply = 0;
        }
    } else {
        if (d->session) {
            d->device->removeSession(d->session);
            d->session = 0;
        }
        finished();
    }
}

void ProgramTester::clearError()
{
    d->err = 0;
    emit errChanged(d->err);
}

void ProgramTester::packetsAvailable(quint32 /*ssrc*/, QList<QByteArray> &packets)
{
    foreach (QByteArray packet, packets) {
        test(packet);
    }
}

void ProgramTester::readyRead()
{
   test(d->reply->readAll());
}

void ProgramTester::finished()
{
    d->status = Done;
    emit statusChanged(d->status);
    emit testingDone();
}



#define STATE_ERROR(s) { qDebug() << "State error" << s; }

static inline int check_item(const char c[] ,quint32 act, quint32 exp, quint32 pseq)
{
   if(act != exp) {
      qDebug() << "error ts " << c << ": " << QString("0x%1").arg(act, 1, 16)
               << " != " << QString("0x%1").arg(exp, 1, 16);
      return -1;
   }
   return 0;
}

void ProgramTester::test(QByteArray data)
{
   int i = 0;
   quint8 c;
   int ret;
   quint32 errPrev = d->err;

   if (data.size() < 2)
       return;

   while (i < data.size()) {
      c = data.at(i++);
      ret=0;
      switch(d->state & C_STATE_MASK) {
         case C_STATE_NO_SYNCH:
            if(c == C_SYNCH_BYTE) {
               d->state = C_STATE_HEAD_PID;
               d->tsCnt++;
               d->tsOffs = 0; 
               if(d->maxTs != 0 && d->tsCnt > d->maxTs) {
                  qDebug() << "Max Ts reached";
                  return;
               }
               if (d->tsCnt % 1000 == 0)
                  emit tsCntChanged(d->tsCnt);
            } else {
               //qDebug() << "Resync offset" << d->tsOffs << "state " << (d->state & C_STATE_SUB_MASK);
               d->state = d->state++;
               if ((d->state & C_STATE_SUB_MASK) == C_STATE_SUB_MASK)
                  d->state &= C_STATE_MASK;
               d->hseq = 0x10;
               d->pseq_ref = 0x80000000;
            }
            break;
            
         case C_STATE_HEAD_PID:
            switch(d->state & C_STATE_SUB_MASK) {
               case 0:
                  d->pid = (c & 0xF)<<8;
                  break;
               case 1:
                  d->pid |= c;
                  ret = check_item("PID", d->pid, d->pidRef, d->pseq);
                  if(ret) {
                     d->state = C_STATE_NO_SYNCH;
                     d->err++;
                  }
                  else
                     d->state = C_STATE_HEAD_SEQU;
                  break;
               default:
                  STATE_ERROR(d->state);
                  break;
            }
            d->state++;
            break;
            
         case C_STATE_HEAD_SEQU:
            if(d->hseq <= 0xF){
               ret=check_item("HEAD SEQU",c & 0xF, d->hseq, d->pseq);
               if(ret){
                  d->state = C_STATE_NO_SYNCH;
                  d->err++;
                  break;
               }
            }
            d->hseq = (c+1) & 0xF;
            d->state = C_STATE_PAYL_SEQU;
            break;
            
         case C_STATE_PAYL_SEQU:
            switch(d->state & C_STATE_SUB_MASK) {
               case 0:
                  d->pseq = ((quint32)c)<<8;
                  d->state++;
                  break;
               case 1:
                  d->pseq |= c;
                  if(d->pseq_ref < 0x80000000) {
                     ret = check_item("PAYLOAD SEQU", d->pseq, d->pseq_ref, d->pseq);
                     if(ret) {
                        d->state = C_STATE_NO_SYNCH;
                        d->err++;
                        break;
                     }
                  } else {
                     qDebug() << "Resync pseq " << QString("0x%1").arg(d->pseq, 1, 16);
                     d->err++;
                  }
                  if(d->tsOffs < C_TS_DATA_END_SEQU_OFFS) {
                     d->pseq_ref = d->pseq;
                     d->state = C_STATE_PAYL_DATA;
                  } else {
                     if(++d->pseq_ref >= d->numTs)
                        d->pseq_ref = 0;
                        // End of TS
                     d->state = C_STATE_NO_SYNCH;
                  }
                  break;
               default:
                  STATE_ERROR(d->state);
                  break;
            }
            break;
         case C_STATE_PAYL_DATA:
            ret = check_item("PAYLOAD DATA", c, d->pidRef, d->pseq_ref);
            if(ret) {
               d->state = C_STATE_NO_SYNCH;
               d->err++;
               break;
            }
            if(d->tsOffs >= C_TS_DATA_END_SEQU_OFFS)
               d->state = C_STATE_PAYL_SEQU;
            break;
      }
      d->tsOffs++;
   }

   if (errPrev != d->err)
      emit errChanged(d->err);
}
