#ifndef WORKITEM_H
#define WORKITEM_H

#include <QObject>
#include <QThread>
#include <QListWidgetItem>
#include <QFileInfo>

class WorkItem : public QObject
{
    Q_OBJECT
public:
    explicit WorkItem(QObject *parent = 0);
    ~WorkItem();
    QThread Mythread;
    bool stop;
signals:
    void emitObrFile(QString p,QStringList fileNames);
    void setDirFiles();
    void emitItem(QListWidgetItem* item);
public slots:
    void setSpis();
    void obrItem(QString p, QStringList fileNames);
private:
    QString D;
    int max(QSize size);
    void obrImage(const QString &fileName, QPixmap &img, const QFileInfo &inf);
    void obrVideo(const QString &file_prev, const QString &fileName, QPixmap &img, const QFileInfo &inf);
};

#endif // WORKITEM_H
