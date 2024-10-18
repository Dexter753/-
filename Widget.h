#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QLineEdit>
#include <QSpinBox>
#include <QPushButton>
#include <QThread>
#include <QTextEdit>
#include "Windows.h"
#include "HCNetSDK.h"
#include "dataworker.h"

using namespace std;


class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();

signals:
    void sigData(QString info);
private:
    void initDataWorker();
    void setupUI();
    void initConnection();
    void connectRader();
    void disconnectRader();

public:
    int64_t mCallbackNum = 0;
    bool mLinkStatus = false;
    LONG mHandle = -1;
    LONG mUserID = -1;
    QThread* mDataThr;
    DataWorker* mDataWorker;

private:
    QLineEdit* ledtIP;
    QSpinBox* spboxPort;
    QLineEdit* ledtUserName;
    QLineEdit* ledtUserPassword;
    QPushButton* pbtnConnect;
    QPushButton* pbtnDisconnect;
    QTextEdit* txtedtInfo;



};
#endif // WIDGET_H
