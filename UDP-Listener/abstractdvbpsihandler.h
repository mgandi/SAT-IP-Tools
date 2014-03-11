#ifndef ABSTRACTDVBPSIHANDLER_H
#define ABSTRACTDVBPSIHANDLER_H

#include <QObject>

class AbstractDvbPsiHandlerPrivate;
class AbstractDvbPsiHandler : public QObject
{
    Q_OBJECT

public:
    enum MessageLevel
    {
        MSG_NONE  = -1, /*!< No messages */
        MSG_ERROR =  0, /*!< Error messages only */
        MSG_WARN  =  1, /*!< Error and Warning messages */
        MSG_DEBUG =  2  /*!< Error, warning and debug messages */
    };

    explicit AbstractDvbPsiHandler(QObject *parent = 0, MessageLevel messageLevel = MSG_ERROR);
    explicit AbstractDvbPsiHandler(const AbstractDvbPsiHandler &other);
    ~AbstractDvbPsiHandler();

    bool attached() const;
    virtual void push(QByteArray &packet);
    void push(QList<QByteArray> &packets);
    void operator <<(QByteArray &packet);
    void operator <<(QList<QByteArray> &packets);

public slots:
    virtual void attach() = 0;
    void dettachLater();
    virtual void dettach() = 0;

protected:
    AbstractDvbPsiHandlerPrivate *d;

    AbstractDvbPsiHandlerPrivate *abstractDvbPsiHandlerPrivate() const;

protected slots:
    bool event(QEvent *);

signals:
    void dataChanged();
};

#endif // ABSTRACTDVBPSIHANDLER_H
