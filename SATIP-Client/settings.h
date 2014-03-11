#ifndef SETTINGS_H
#define SETTINGS_H


#include <QSettings>

class Program;
class SettingsPrivate;
class Settings : public QSettings
{
    Q_OBJECT
public:
    explicit Settings(QObject *parent = 0);
    ~Settings();

    QString vlc() const;
    qint16 httpPort() const;
    QList<Program *> programs(const QString &uid);
    QString lastUsedDevice();

public slots:
    void setVlc(const QString &vlc);
    void setPrograms(const QString &uid, const QList<Program *> &programs);
    void setLastUsedDevice(const QString &uid);
};

#endif // SETTINGS_H
