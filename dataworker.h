#ifndef DATAWORKER_H
#define DATAWORKER_H

#include <QObject>
#include <windows.h>
#include <QTimer>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

class DataWorker : public QObject
{
    Q_OBJECT
public:
    explicit DataWorker(QObject *parent = nullptr);
    ~DataWorker();

public slots:
    void onInit();
    void onQuit();
    void onAppendData(QString info);
signals:
    void sigInitResult(bool isSucceed);
private:
    void openFile();
    void closeFile();
    QJsonObject qStringToQJsonObject(const QString &jsonString);
private:
    QString mFilePath;
    QFile* mFile;
    qint64 mFrameId = -1;
};

#endif // DATAWORKER_H
