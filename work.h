#ifndef WORK_H
#define WORK_H

#include <QObject>
#include <QThread>
#include <QMutex>
#include <QListWidgetItem>
#include "QDebug"
#include <workitem.h>

class Work : public QObject
{
    Q_OBJECT
public:
    WorkItem workItem;
    static QString Dir_Tek;
    QThread Mythread;
    Work();
    ~Work();
    static bool stop;
    static QString getMap(QString item);
    static void saveMap();
    static void insertMap(QString fileNames, QString s);
    static void removeMap(QString fileNames);
signals:
    void setDirFiles();
public slots:
    void nach_getDirFiles(QString s);
private slots:

private:
    QIcon icon;
    static QMap<QString,QString> map;
    int index;

    static QString D;
    static QMutex m;
    void getDirFiles(QString s);
    void loadMap();
};

#endif // WORK_H
