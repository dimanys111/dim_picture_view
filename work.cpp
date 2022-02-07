#include "work.h"
#include "QDir"
#include "QApplication"
#include "mainwindow.h"

QString Work::Dir_Tek;
QString Work::D;
bool Work::stop;
QMutex Work::m;
QMap<QString,QString> Work::map;

QStringList slTipsFiles;

Work::Work()
{
    slTipsFiles<<"*.bmp"<<"*.jpg"<<"*.jpeg"<<"*.png"<<"*.gif"<<"*.jpeg"<<"*.avi"<<"*.mp4"<<"*.3gp"<<"*.mov"<<"*.m4v"<<"*.mkv";
    icon.addFile(qApp->applicationDirPath()+"/cher.jpg");
    QDir path;
    Work::D=qApp->applicationDirPath()+"/cache";
    path.mkpath(Work::D);
    stop=false;
    moveToThread(&Mythread);
    Mythread.start();
    loadMap();
}

Work::~Work()
{
    workItem.stop=true;
    workItem.Mythread.exit();
    workItem.Mythread.wait();
}

QString Work::getMap(QString item)
{
    m.lock();
    QString s=map.value(item,"");
    m.unlock();
    return s;
}

void Work::insertMap(QString fileNames,QString s)
{
    m.lock();
    map.insert(fileNames,s);
    m.unlock();
}

void Work::removeMap(QString fileNames)
{
    m.lock();
    map.remove(fileNames);
    m.unlock();
}

void Work::loadMap()
{
    QFile file(qApp->applicationDirPath()+"/cache.cache");
    if (!file.open(QIODevice::ReadOnly)) {
        Work::Dir_Tek=qApp->applicationDirPath();
        return;
    }
    QDataStream in(&file);
    in.setVersion(QDataStream::Qt_4_6);
    m.lock();
    in >> map;
    m.unlock();
    in >> Work::Dir_Tek;
    file.close();
}

void Work::saveMap()
{
    QFile file(qApp->applicationDirPath()+"/cache.cache");
    if (!file.open(QIODevice::WriteOnly)) {
        return;
    }
    QDataStream out(&file);
    out.setVersion(QDataStream::Qt_4_6);
    m.lock();
    out << map;
    m.unlock();
    out << Work::Dir_Tek;
    file.close();
    qDebug() << "sav";
}


void Work::nach_getDirFiles(QString s)
{
    Work::Dir_Tek=s;
    stop=false;
    emit setDirFiles();
    getDirFiles(s);
}

void Work::getDirFiles(QString s)
{
    if(stop)
    {
        return;
    }
    QDir dir(s);
    if (!dir.exists())
    {
        return;
    }
    QStringList fileNames;

    QStringList fileList = dir.entryList(slTipsFiles, QDir::Files);
    QStringList dirList = dir.entryList(QDir::Dirs);
    for (QStringList::Iterator fit = fileList.begin(); fit != fileList.end(); ++fit)
    {
        fileNames<<dir.absolutePath() + "/" + *fit;
    }
    if(!fileNames.isEmpty())
    {
        emit workItem.emitObrFile(s,fileNames);
    }
    dirList.removeOne(".");
    dirList.removeOne("..");
    for (QStringList::Iterator dit = dirList.begin(); dit != dirList.end(); ++dit) {
        QDir curDir = dir;
        curDir.cd(*dit);
        getDirFiles(curDir.absolutePath());
    }
}
