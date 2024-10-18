#include "Widget.h"
#include <QDebug>
#include <QVBoxLayout>
#include <QFormLayout>
#include <QHBoxLayout>
#include <QApplication>

#pragma execution_character_set("utf-8")

void CALLBACK fRadarCallback(DWORD dwType, void* lpBuffer, DWORD dwBufLen, void* pUserData)
{
    //回调函数中不可有耗时较长的操作，不能调用该SDK（HCNetSDK.dll）本身的接口。
    //以下代码仅供参考，实际应用中不建议在回调函数中直接处理数据，
    //例如可以使用消息的方式(PostMessage)在消息响应函数里进行处理。
    Widget* wgt = (Widget*)pUserData;
    if (dwType == NET_SDK_CALLBACK_TYPE_DATA){
        //数据信息
        NET_DVR_ALARM_SEARCH_RESULT struAlarmResult = {0};
        memcpy(&struAlarmResult, lpBuffer, sizeof(struAlarmResult));
//        qDebug("报警类型: " << "，报警设备信息：lUserID[%d]IP[%s]\n", struAlarmResult.dwAlarmComm, struAlarmResult.struAlarmer.lUserID, struAlarmResult.struAlarmer.sDeviceIP);
        qDebug() << "信息长度" << struAlarmResult.dwAlarmLen << ",deviceip:" << struAlarmResult.struAlarmer.sDeviceIP;
//        qDebug() << QString::fromUtf8(struAlarmResult.pAlarmInfo, struAlarmResult.dwAlarmLen);
        //这里将报警信息数据保存下来，雷达视频目标信息是透传的JSON数据
        if ((struAlarmResult.dwAlarmLen != 0) && (struAlarmResult.pAlarmInfo != NULL)){
            emit wgt->sigData(QString::fromUtf8(struAlarmResult.pAlarmInfo, struAlarmResult.dwAlarmLen));
        }
    }/*else if (dwType == NET_SDK_CALLBACK_TYPE_STATUS){
        //状态值
        DWORD dwStatus = *(DWORD*)lpBuffer;
        if (dwStatus == NET_SDK_CALLBACK_STATUS_SUCCESS){
            printf("获取成功并且结束\n");
            wgt->mLinkStatus = false;
        }else if (dwStatus == NET_SDK_CALLBACK_STATUS_PROCESSING){
            printf("正在获取事件，处理中...\n");
        }else if (dwStatus == NET_SDK_CALLBACK_STATUS_FAILED){
            printf("获取失败并且结束\n");
            wgt->mLinkStatus = false;
        }
    }*/
}


Widget::Widget(QWidget *parent)
    : QWidget(parent)
{
    //初始化sdk环境
    NET_DVR_Init();
    initDataWorker();
    setupUI();
    initConnection();
    mDataThr->start();
}

Widget::~Widget()
{
    //释放SDK资源
    NET_DVR_Cleanup();
}

void Widget::initDataWorker()
{
    mDataThr = new QThread();
    mDataWorker = new DataWorker();
    mDataWorker->moveToThread(mDataThr);
    connect(mDataThr, &QThread::started, mDataWorker, &DataWorker::onInit);
    connect(qApp, &QApplication::aboutToQuit, mDataWorker, &DataWorker::onQuit);
    connect(mDataWorker, &DataWorker::destroyed, mDataThr, &QThread::quit);
    connect(mDataThr, &QThread::finished, mDataThr, &QThread::deleteLater);
    connect(mDataWorker, &DataWorker::sigInitResult, this, [&](bool isSucceed){
        txtedtInfo->append(isSucceed ? "文件初始化成功" : "文件初始化失败");
//        QFile file(QApplication::applicationDirPath() + "/data.txt");
//        file.open(QIODevice::ReadOnly);
//        QString str = file.readAll();
//        mDataWorker->onAppendData(str);
//        QString str = "{\"ipAddress\":\"192.1.0.66\",\"portNo\":8000,\"protocol\":\"HTTP\",\"macAddress\":\"44:a6:42:b6:b6:a1\",\"channelID\":1,\"dateTime\":\"2024-09-18T16:42:41.287+08:00\",\"activePostCount\":1,\"eventType\":\"radarVideoDetection\",\"eventState\":\"active\",\"eventDescription\":\"Radar Video Detection\",\"stopLineDistance\":20,\"freezingTimeInfo\":{\"freezingTimestamp\":2240078,\"freezingSystemDateTime\":\"2024-09-18T16:42:41.092\"},\"Datas\":[{\"targetID\":9389,\"UUID\":\"03134b78-1dd2-11b2-bef8-a52b031d8033\",\"GPS\":{\"longitude\":119.97290537657146,\"latitude\":30.523545209302956},\"videoDetectionID\":8087,\"trackingTargetStatus\":\"tracking\",\"trackingTargetRect\":{\"height\":0.34210526943206787,\"width\":0.1882440447807312,\"x\":0.527529776096344,\"y\":0.64736843109130859},\"targetProfile\":{\"length\":4.5,\"width\":2,\"height\":1.7999999523162842},\"globalDirection\":-1,\"globalSpeed\":{\"speed\":0,\"angle\":0},\"targetType\":\"vehicle\",\"vehicleSize\":\"light\",\"plateNo\":\"\",\"plateColor\":\"other\",\"vehicleLogo\":0,\"vehicleSublogo\":0,\"vehicleColor\":\"unknown\",\"laneNo\":2,\"position\":[-2.3338851928710938,16.491170883178711],\"speed\":61.6784896850586,\"horizonSpeed\":0.876933455467224,\"radarDetected\":true,\"vehicleType\":\"unknown\",\"radarDirection\":179.8927001953125,\"checksum\":\"32\"},{\"targetID\":9391,\"UUID\":\"035f8b50-1dd2-11b2-bef8-a52b031d8033\",\"GPS\":{\"longitude\":119.97295729276208,\"latitude\":30.523748402748389},\"videoDetectionID\":8090,\"trackingTargetStatus\":\"tracking\",\"trackingTargetRect\":{\"height\":0.15657894313335419,\"width\":0.0892857164144516,\"x\":0.66741073131561279,\"y\":0.19210526347160339},\"targetProfile\":{\"length\":4.5,\"width\":2,\"height\":1.7999999523162842},\"globalDirection\":-1,\"globalSpeed\":{\"speed\":0,\"angle\":0},\"targetType\":\"vehicle\",\"vehicleSize\":\"light\",\"plateNo\":\"\",\"plateColor\":\"other\",\"vehicleLogo\":0,\"vehicleSublogo\":0,\"vehicleColor\":\"unknown\",\"laneNo\":3,\"position\":[0.83897632360458374,39.45379638671875],\"speed\":48.1185417175293,\"horizonSpeed\":0.720107138156891,\"radarDetected\":true,\"vehicleType\":\"unknown\",\"radarDirection\":179.00917053222656,\"checksum\":\"32\"},{\"targetID\":9385,\"UUID\":\"026b8280-1dd2-11b2-bef8-a52b031d8033\",\"GPS\":{\"longitude\":119.97282151234015,\"latitude\":30.523773396820228},\"videoDetectionID\":8084,\"trackingTargetStatus\":\"tracking\",\"trackingTargetRect\":{\"height\":0.12368421256542206,\"width\":0.0877976194024086,\"x\":0.174851194024086,\"y\":0.16842105984687805},\"targetProfile\":{\"length\":4.5,\"width\":2,\"height\":1.7999999523162842},\"globalDirection\":-1,\"globalSpeed\":{\"speed\":0,\"angle\":0},\"targetType\":\"vehicle\",\"vehicleSize\":\"light\",\"plateNo\":\"\",\"plateColor\":\"other\",\"vehicleLogo\":0,\"vehicleSublogo\":0,\"vehicleColor\":\"unknown\",\"laneNo\":1,\"position\":[-12.372383117675781,41.196990966796875],\"speed\":36.786552429199219,\"horizonSpeed\":-5.6437101364135742,\"radarDetected\":true,\"vehicleType\":\"unknown\",\"radarDirection\":191.50588989257812,\"checksum\":\"32\"},{\"targetID\":9334,\"UUID\":\"fc4ae04e-1dd1-11b2-bef8-a52b031d8033\",\"GPS\":{\"longitude\":119.97274209802139,\"latitude\":30.523844326166952},\"videoDetectionID\":8040,\"trackingTargetStatus\":\"tracking\",\"trackingTargetRect\":{\"height\":0.10131578892469406,\"width\":0.0758928582072258,\"x\":0.0029761905316263437,\"y\":0.10921052843332291},\"targetProfile\":{\"length\":4.5,\"width\":2,\"height\":1.7999999523162842},\"globalDirection\":-1,\"globalSpeed\":{\"speed\":0,\"angle\":0},\"targetType\":\"vehicle\",\"vehicleSize\":\"light\",\"plateNo\":\"\",\"plateColor\":\"other\",\"vehicleLogo\":0,\"vehicleSublogo\":0,\"vehicleColor\":\"unknown\",\"laneNo\":1,\"position\":[-20.59657096862793,48.4707374572754],\"speed\":6.9971432685852051,\"horizonSpeed\":0.18609842658042908,\"radarDetected\":true,\"vehicleType\":\"unknown\",\"radarDirection\":178.69747924804688,\"checksum\":\"32\"},{\"targetID\":9394,\"UUID\":\"03f55018-1dd2-11b2-bef8-a52b031d8033\",\"GPS\":{\"longitude\":119.97278130790376,\"latitude\":30.523902259641275},\"videoDetectionID\":8093,\"trackingTargetStatus\":\"tracking\",\"trackingTargetRect\":{\"height\":0.0828947350382805,\"width\":0.02083333395421505,\"x\":0.1540178507566452,\"y\":0.085526317358016968},\"targetProfile\":{\"length\":2,\"width\":1.2000000476837158,\"height\":1.7000000476837158},\"globalDirection\":-1,\"globalSpeed\":{\"speed\":0,\"angle\":0},\"targetType\":\"nonMotor\",\"vehicleSize\":\"light\",\"plateNo\":\"\",\"plateColor\":\"other\",\"vehicleLogo\":0,\"vehicleSublogo\":0,\"vehicleColor\":\"unknown\",\"laneNo\":1,\"position\":[-17.356746673583984,55.203372955322266],\"speed\":45.220161437988281,\"horizonSpeed\":1.4201791286468506,\"radarDetected\":true,\"vehicleType\":\"unknown\",\"radarDirection\":178.49192810058594,\"checksum\":\"48\"},{\"targetID\":9401,\"UUID\":\"04ec494a-1dd2-11b2-bef8-a52b031d8033\",\"GPS\":{\"longitude\":119.97305385216271,\"latitude\":30.5238864343411},\"videoDetectionID\":8095,\"trackingTargetStatus\":\"tracking\",\"trackingTargetRect\":{\"height\":0.088157892227172852,\"width\":0.0632440447807312,\"x\":0.890625,\"y\":0.11184210330247879},\"targetProfile\":{\"length\":4.5,\"width\":2,\"height\":1.7999999523162842},\"globalDirection\":-1,\"globalSpeed\":{\"speed\":0,\"angle\":0},\"targetType\":\"vehicle\",\"vehicleSize\":\"light\",\"plateNo\":\"\",\"plateColor\":\"other\",\"vehicleLogo\":0,\"vehicleSublogo\":0,\"vehicleColor\":\"unknown\",\"laneNo\":1,\"position\":[8.85840606689453,55.518768310546875],\"speed\":20.307441711425781,\"horizonSpeed\":1.107677698135376,\"radarDetected\":true,\"vehicleType\":\"unknown\",\"radarDirection\":2.4777882099151611,\"checksum\":\"32\"},{\"targetID\":9360,\"UUID\":\"ff029f5c-1dd1-11b2-bef8-a52b031d8033\",\"GPS\":{\"longitude\":119.97274653940214,\"latitude\":30.5239238540789},\"videoDetectionID\":8059,\"trackingTargetStatus\":\"tracking\",\"trackingTargetRect\":{\"height\":0.0828947350382805,\"width\":0.0565476194024086,\"x\":0.0669642835855484,\"y\":0.077631577849388123},\"targetProfile\":{\"length\":4.5,\"width\":2,\"height\":1.7999999523162842},\"globalDirection\":-1,\"globalSpeed\":{\"speed\":0,\"angle\":0},\"targetType\":\"vehicle\",\"vehicleSize\":\"light\",\"plateNo\":\"\",\"plateColor\":\"other\",\"vehicleLogo\":0,\"vehicleSublogo\":0,\"vehicleColor\":\"unknown\",\"laneNo\":1,\"position\":[-20.873867034912109,57.337306976318359],\"speed\":7.7172646522521973,\"horizonSpeed\":0.544521391391754,\"radarDetected\":true,\"vehicleType\":\"unknown\",\"radarDirection\":176.5584716796875,\"checksum\":\"32\"},";
//                str += "{\"targetID\":9392,\"UUID\":\"038a593e-1dd2-11b2-bef8-a52b031d8033\",\"GPS\":{\"longitude\":119.9728117927383,\"latitude\":30.523933601411184},\"videoDetectionID\":8091,\"trackingTargetStatus\":\"tracking\",\"trackingTargetRect\":{\"height\":0.0894736871123314,\"width\":0.0543154776096344,\"x\":0.224702388048172,\"y\":0.0671052634716034},\"targetProfile\":{\"length\":4.5,\"width\":2,\"height\":1.7999999523162842},\"globalDirection\":-1,\"globalSpeed\":{\"speed\":0,\"angle\":0},\"targetType\":\"vehicle\",\"vehicleSize\":\"light\",\"plateNo\":\"\",\"plateColor\":\"other\",\"vehicleLogo\":0,\"vehicleSublogo\":0,\"vehicleColor\":\"unknown\",\"laneNo\":1,\"position\":[-14.71688175201416,58.916229248046875],\"speed\":34.094646453857422,\"horizonSpeed\":0.930905044078827,\"radarDetected\":true,\"vehicleType\":\"unknown\",\"radarDirection\":177.65328979492188,\"checksum\":\"32\"},{\"targetID\":9399,\"UUID\":\"0494ef56-1dd2-11b2-bef8-a52b031d8033\",\"GPS\":{\"longitude\":119.97307998635749,\"latitude\":30.523980489883339},\"videoDetectionID\":8096,\"trackingTargetStatus\":\"tracking\",\"trackingTargetRect\":{\"height\":0.0736842080950737,\"width\":0.0744047611951828,\"x\":0.86458331346511841,\"y\":0.060526315122842789},\"targetProfile\":{\"length\":4.5,\"width\":2,\"height\":1.7999999523162842},\"globalDirection\":-1,\"globalSpeed\":{\"speed\":0,\"angle\":0},\"targetType\":\"vehicle\",\"vehicleSize\":\"light\",\"plateNo\":\"\",\"plateColor\":\"other\",\"vehicleLogo\":0,\"vehicleSublogo\":0,\"vehicleColor\":\"unknown\",\"laneNo\":1,\"position\":[10.528273582458496,66.163856506347656],\"speed\":21.866876602172852,\"horizonSpeed\":0.1052008792757988,\"radarDetected\":true,\"vehicleType\":\"unknown\",\"radarDirection\":0.84487003087997437,\"checksum\":\"32\"},{\"targetID\":9372,\"UUID\":\"0030935c-1dd2-11b2-bef8-a52b031d8033\",\"GPS\":{\"longitude\":119.97274719434314,\"latitude\":30.524004935079265},\"videoDetectionID\":8073,\"trackingTargetStatus\":\"tracking\",\"trackingTargetRect\":{\"height\":0.0644736811518669,\"width\":0.0558035708963871,\"x\":0.1101190447807312,\"y\":0.05000000074505806},\"targetProfile\":{\"length\":4.5,\"width\":2,\"height\":1.7999999523162842},\"globalDirection\":-1,\"globalSpeed\":{\"speed\":0,\"angle\":0},\"targetType\":\"vehicle\",\"vehicleSize\":\"light\",\"plateNo\":\"\",\"plateColor\":\"other\",\"vehicleLogo\":0,\"vehicleSublogo\":0,\"vehicleColor\":\"unknown\",\"laneNo\":1,\"position\":[-21.527139663696289,66.347572326660156],\"speed\":9.0102176666259766,\"horizonSpeed\":0.0798015519976616,\"radarDetected\":true,\"vehicleType\":\"unknown\",\"radarDirection\":181.86042785644531,\"checksum\":\"32\"},{\"targetID\":9406,\"UUID\":\"0577495a-1dd2-11b2-bef8-a52b031d8033\",\"GPS\":{\"longitude\":119.97310331539126,\"latitude\":30.523987453016783},\"videoDetectionID\":8101,\"trackingTargetStatus\":\"tracking\",\"trackingTargetRect\":{\"height\":0.060526315122842789,\"width\":0.0476190485060215,\"x\":0.944940447807312,\"y\":0.068421050906181335},\"targetProfile\":{\"length\":4.5,\"width\":2,\"height\":1.7999999523162842},\"globalDirection\":-1,\"globalSpeed\":{\"speed\":0,\"angle\":0},\"targetType\":\"vehicle\",\"vehicleSize\":\"light\",\"plateNo\":\"\",\"plateColor\":\"other\",\"vehicleLogo\":0,\"vehicleSublogo\":0,\"vehicleColor\":\"unknown\",\"laneNo\":1,\"position\":[12.698772430419922,67.1146697998047],\"speed\":8.7129745483398438,\"horizonSpeed\":0.482199490070343,\"radarDetected\":true,\"vehicleType\":\"unknown\",\"radarDirection\":180,\"checksum\":\"32\"},{\"targetID\":9404,\"UUID\":\"0516e600-1dd2-11b2-bef8-a52b031d8033\",\"GPS\":{\"longitude\":119.97293283789072,\"latitude\":30.524024030866375},\"videoDetectionID\":8100,\"trackingTargetStatus\":\"tracking\",\"trackingTargetRect\":{\"height\":0.102631576359272,\"width\":0.0543154776096344,\"x\":0.5,\"y\":0.011842105537652969},\"targetProfile\":{\"length\":4.5,\"width\":2,\"height\":1.7999999523162842},\"globalDirection\":-1,\"globalSpeed\":{\"speed\":0,\"angle\":0},\"targetType\":\"vehicle\",\"vehicleSize\":\"light\",\"plateNo\":\"\",\"plateColor\":\"other\",\"vehicleLogo\":0,\"vehicleSublogo\":0,\"vehicleColor\":\"unknown\",\"laneNo\":1,\"position\":[-3.9344801902771,69.880500793457031],\"speed\":67.334434509277344,\"horizonSpeed\":0.69849354028701782,\"radarDetected\":true,\"vehicleType\":\"unknown\",\"radarDirection\":179.77230834960938,\"checksum\":\"32\"},{\"targetID\":9388,\"UUID\":\"031347b8-1dd2-11b2-bef8-a52b031d8033\",\"GPS\":{\"longitude\":119.9730979950191,\"latitude\":30.524043443701142},\"videoDetectionID\":8086,\"trackingTargetStatus\":\"lost\",\"trackingTargetRect\":{\"height\":0.053947366774082184,\"width\":0.0520833320915699,\"x\":0.883184552192688,\"y\":0.0355263166129589},\"targetProfile\":{\"length\":4.5,\"width\":2,\"height\":1.7999999523162842},\"globalDirection\":-1,\"globalSpeed\":{\"speed\":0,\"angle\":0},\"targetType\":\"vehicle\",\"vehicleSize\":\"light\",\"plateNo\":\"\",\"plateColor\":\"other\",\"vehicleLogo\":0,\"vehicleSublogo\":0,\"vehicleColor\":\"unknown\",\"laneNo\":1,\"position\":[11.695361137390137,73.292816162109375],\"speed\":12.601579666137695,\"horizonSpeed\":0.6534041166305542,\"radarDetected\":true,\"vehicleType\":\"unknown\",\"radarDirection\":9.6843061447143555,\"checksum\":\"32\"},{\"targetID\":9398,\"UUID\":\"045fc164-1dd2-11b2-bef8-a52b031d8033\",\"GPS\":{\"longitude\":119.97278201420703,\"latitude\":30.52408648993919},\"videoDetectionID\":8094,\"trackingTargetStatus\":\"tracking\",\"trackingTargetRect\":{\"height\":0.0763157904148102,\"width\":0.0505952388048172,\"x\":0.2038690447807312,\"y\":0.01315789483487606},\"targetProfile\":{\"length\":4.5,\"width\":2,\"height\":1.7999999523162842},\"globalDirection\":-1,\"globalSpeed\":{\"speed\":0,\"angle\":0},\"targetType\":\"vehicle\",\"vehicleSize\":\"light\",\"plateNo\":\"\",\"plateColor\":\"other\",\"vehicleLogo\":0,\"vehicleSublogo\":0,\"vehicleColor\":\"unknown\",\"laneNo\":1,\"position\":[-18.915901184082031,75.6703338623047],\"speed\":35.732250213623047,\"horizonSpeed\":0.0765029862523079,\"radarDetected\":true,\"vehicleType\":\"unknown\",\"radarDirection\":177.936279296875,\"checksum\":\"32\"},{\"targetID\":9378,\"UUID\":\"01839c2c-1dd2-11b2-bef8-a52b031d8033\",\"GPS\":{\"longitude\":119.97309316636941,\"latitude\":30.524080270190051},\"videoDetectionID\":8078,\"trackingTargetStatus\":\"tracking\",\"trackingTargetRect\":{\"height\":0.053947366774082184,\"width\":0.0424107126891613,\"x\":0.8467261791229248,\"y\":0.034210525453090668},\"targetProfile\":{\"length\":4.5,\"width\":2,\"height\":1.7999999523162842},\"globalDirection\":-1,\"globalSpeed\":{\"speed\":0,\"angle\":0},\"targetType\":\"vehicle\",\"vehicleSize\":\"light\",\"plateNo\":\"\",\"plateColor\":\"other\",\"vehicleLogo\":0,\"vehicleSublogo\":0,\"vehicleColor\":\"unknown\",\"laneNo\":1,\"position\":[10.908210754394531,77.3462371826172],\"speed\":8.16349411010742,\"horizonSpeed\":-0.16712991893291473,\"radarDetected\":true,\"vehicleType\":\"unknown\",\"radarDirection\":356.9190673828125,\"checksum\":\"32\"},{\"targetID\":9387,\"UUID\":\"02bbe3e2-1dd2-11b2-bef8-a52b031d8033\",\"GPS\":{\"longitude\":119.97274918033109,\"latitude\":30.5241051970519},\"videoDetectionID\":8079,\"trackingTargetStatus\":\"tracking\",\"trackingTargetRect\":{\"height\":0.052631579339504242,\"width\":0.0528273805975914,\"x\":0.1488095223903656,\"y\":0.02500000037252903},\"targetProfile\":{\"length\":4.5,\"width\":2,\"height\":1.7999999523162842},\"globalDirection\":-1,\"globalSpeed\":{\"speed\":0,\"angle\":0},\"targetType\":\"vehicle\",\"vehicleSize\":\"light\",\"plateNo\":\"\",\"plateColor\":\"other\",\"vehicleLogo\":0,\"vehicleSublogo\":0,\"vehicleColor\":\"unknown\",\"laneNo\":1,\"position\":[-22.222433090209961,77.498306274414062],\"speed\":9.14147090911865,\"horizonSpeed\":-0.031262040138244629,\"radarDetected\":true,\"vehicleType\":\"unknown\",\"radarDirection\":182.86933898925781,\"checksum\":\"32\"},{\"targetID\":9405,\"UUID\":\"054e6e36-1dd2-11b2-bef8-a52b031d8033\",\"GPS\":{\"longitude\":119.97297371970123,\"latitude\":30.5241016070179},\"videoDetectionID\":8102,\"trackingTargetStatus\":\"tracking\",\"trackingTargetRect\":{\"height\":0.077631577849388123,\"width\":0.045386902987957,\"x\":0.574404776096344,\"y\":0},\"targetProfile\":{\"length\":4.5,\"width\":2,\"height\":1.7999999523162842},\"globalDirection\":-1,\"globalSpeed\":{\"speed\":0,\"angle\":0},\"targetType\":\"vehicle\",\"vehicleSize\":\"light\",\"plateNo\":\"\",\"plateColor\":\"other\",\"vehicleLogo\":0,\"vehicleSublogo\":0,\"vehicleColor\":\"unknown\",\"laneNo\":1,\"position\":[-0.70813882350921631,78.807479858398438],\"speed\":50.657516479492188,\"horizonSpeed\":-1.1056380271911621,\"radarDetected\":true,\"vehicleType\":\"unknown\",\"radarDirection\":179.43359375,\"checksum\":\"32\"},{\"targetID\":9386,\"UUID\":\"027b4b0c-1dd2-11b2-bef8-a52b031d8033\",\"GPS\":{\"longitude\":119.97307179009562,\"latitude\":30.52409724655125},\"videoDetectionID\":8081,\"trackingTargetStatus\":\"tracking\",\"trackingTargetRect\":{\"height\":0.056578949093818665,\"width\":0.0498511902987957,\"x\":0.7834821343421936,\"y\":0.021052632480859756},\"targetProfile\":{\"length\":4.5,\"width\":2,\"height\":1.7999999523162842},\"globalDirection\":-1,\"globalSpeed\":{\"speed\":0,\"angle\":0},\"targetType\":\"vehicle\",\"vehicleSize\":\"light\",\"plateNo\":\"\",\"plateColor\":\"other\",\"vehicleLogo\":0,\"vehicleSublogo\":0,\"vehicleColor\":\"unknown\",\"laneNo\":1,\"position\":[8.7131576538085938,79.069122314453125],\"speed\":19.490348815917969,\"horizonSpeed\":-0.30053228139877319,\"radarDetected\":true,\"vehicleType\":\"unknown\",\"radarDirection\":358.71322631835938,\"checksum\":\"32\"},{\"targetID\":9393,\"UUID\":\"03d09aca-1dd2-11b2-bef8-a52b031d8033\",\"GPS\":{\"longitude\":119.97311996958535,\"latitude\":30.524095004894225},\"videoDetectionID\":8088,\"trackingTargetStatus\":\"tracking\",\"trackingTargetRect\":{\"height\":0.0671052634716034,\"width\":0.0446428582072258,\"x\":0.8928571343421936,\"y\":0.014473684132099152},\"targetProfile\":{\"length\":4.5,\"width\":2,\"height\":1.7999999523162842},\"globalDirection\":-1,\"globalSpeed\":{\"speed\":0,\"angle\":0},\"targetType\":\"vehicle\",\"vehicleSize\":\"light\",\"plateNo\":\"\",\"plateColor\":\"other\",\"vehicleLogo\":0,\"vehicleSublogo\":0,\"vehicleColor\":\"unknown\",\"laneNo\":1,\"position\":[13.342477798461914,79.1866226196289],\"speed\":13.740986824035645,\"horizonSpeed\":0.32671913504600525,\"radarDetected\":true,\"vehicleType\":\"unknown\",\"radarDirection\":4.36372947692871,\"checksum\":\"32\"},{\"targetID\":9403,\"UUID\":\"04f8b1ee-1dd2-11b2-bef8-a52b031d8033\",\"GPS\":{\"longitude\":119.97314617441941,\"latitude\":30.524121749747039},\"videoDetectionID\":8099,\"trackingTargetStatus\":\"lost\",\"trackingTargetRect\":{\"height\":0.055263157933950424,\"width\":0.0520833320915699,\"x\":0.9382440447807312,\"y\":0.00789473671466112},\"targetProfile\":{\"length\":4.5,\"width\":2,\"height\":1.7999999523162842},\"globalDirection\":-1,\"globalSpeed\":{\"speed\":0,\"angle\":0},\"targetType\":\"vehicle\",\"vehicleSize\":\"light\",\"plateNo\":\"\",\"plateColor\":\"other\",\"vehicleLogo\":0,\"vehicleSublogo\":0,\"vehicleColor\":\"unknown\",\"laneNo\":1,\"position\":[15.613448143005371,82.3563690185547],\"speed\":8.4136848449707031,\"horizonSpeed\":-3.8078536987304688,\"radarDetected\":true,\"vehicleType\":\"unknown\",\"radarDirection\":180,\"checksum\":\"32\"},{\"targetID\":9400,\"UUID\":\"04dcd15e-1dd2-11b2-bef8-a52b031d8033\",\"GPS\":{\"longitude\":119.97281847677924,\"latitude\":30.524147318168165},\"videoDetectionID\":8097,\"trackingTargetStatus\":\"tracking\",\"trackingTargetRect\":{\"height\":0.063157893717288971,\"width\":0.0461309514939785,\"x\":0.277529776096344,\"y\":0.00263157906010747},\"targetProfile\":{\"length\":4.5,\"width\":2,\"height\":1.7999999523162842},\"globalDirection\":-1,\"globalSpeed\":{\"speed\":0,\"angle\":0},\"targetType\":\"vehicle\",\"vehicleSize\":\"light\",\"plateNo\":\"\",\"plateColor\":\"other\",\"vehicleLogo\":0,\"vehicleSublogo\":0,\"vehicleColor\":\"unknown\",\"laneNo\":1,\"position\":[-15.9644832611084,82.703582763671875],\"speed\":37.5085792541504,\"horizonSpeed\":1.1440087556838989,\"radarDetected\":true,\"vehicleType\":\"unknown\",\"radarDirection\":180.60855102539062,\"checksum\":\"32\"},{\"targetID\":9368,\"UUID\":\"ffbb54c0-1dd1-11b2-bef8-a52b031d8033\",\"GPS\":{\"longitude\":119.97309582520411,\"latitude\":30.5241459491112},\"videoDetectionID\":8067,\"trackingTargetStatus\":\"tracking\",\"trackingTargetRect\":{\"height\":0.05000000074505806,\"width\":0.0372023805975914,\"x\":0.8117559552192688,\"y\":0.011842105537652969},\"targetProfile\":{\"length\":4.5,\"width\":2,\"height\":1.7999999523162842},\"globalDirection\":-1,\"globalSpeed\":{\"speed\":0,\"angle\":0},\"targetType\":\"vehicle\",\"vehicleSize\":\"light\",\"plateNo\":\"\",\"plateColor\":\"other\",\"vehicleLogo\":0,\"vehicleSublogo\":0,\"vehicleColor\":\"unknown\",\"laneNo\":1,\"position\":[10.582655906677246,84.6611099243164],\"speed\":4.1099328994750977,\"horizonSpeed\":-0.551734447479248,\"radarDetected\":true,\"vehicleType\":\"unknown\",\"radarDirection\":4.8679895401000977,\"checksum\":\"32\"},{\"targetID\":9377,\"UUID\":\"01032c90-1dd2-11b2-bef8-a52b031d8033\",\"GPS\":{\"longitude\":119.97312949003793,\"latitude\":30.524222709156035},\"videoDetectionID\":8075,\"trackingTargetStatus\":\"tracking\",\"trackingTargetRect\":{\"height\":0.0381578952074051,\"width\":0.0305059514939785,\"x\":0.8407738208770752,\"y\":0},\"targetProfile\":{\"length\":4.5,\"width\":2,\"height\":1.7999999523162842},\"globalDirection\":-1,\"globalSpeed\":{\"speed\":0,\"angle\":0},\"targetType\":\"vehicle\",\"vehicleSize\":\"light\",\"plateNo\":\"\",\"plateColor\":\"other\",\"vehicleLogo\":0,\"vehicleSublogo\":0,\"vehicleColor\":\"unknown\",\"laneNo\":1,\"position\":[13.125727653503418,93.4425506591797],\"speed\":9.70521354675293,\"horizonSpeed\":-0.40600103139877319,\"radarDetected\":true,\"vehicleType\":\"unknown\",\"radarDirection\":353.82955932617188,\"checksum\":\"32\"},{\"targetID\":9355,\"UUID\":\"fe8d5544-1dd1-11b2-bef8-a52b031d8033\",\"GPS\":{\"longitude\":119.97310697708697,\"latitude\":30.524224613633223},\"videoDetectionID\":8057,\"trackingTargetStatus\":\"tracking\",\"trackingTargetRect\":{\"height\":0.026315789669752121,\"width\":0.0438988097012043,\"x\":0.7842261791229248,\"y\":0},\"targetProfile\":{\"length\":4.5,\"width\":2,\"height\":1.7999999523162842},\"globalDirection\":-1,\"globalSpeed\":{\"speed\":0,\"angle\":0},\"targetType\":\"vehicle\",\"vehicleSize\":\"light\",\"plateNo\":\"\",\"plateColor\":\"other\",\"vehicleLogo\":0,\"vehicleSublogo\":0,\"vehicleColor\":\"unknown\",\"laneNo\":1,\"position\":[10.955004692077637,93.4828338623047],\"speed\":6.9391674995422363,\"horizonSpeed\":0.2713586688041687,\"radarDetected\":true,\"vehicleType\":\"unknown\",\"radarDirection\":356.719482421875,\"checksum\":\"32\"}],\"totalsum\":\"22255\"}";

//        mDataWorker->onAppendData(str);
    });
    connect(this, &Widget::sigData, mDataWorker, &DataWorker::onAppendData);
}

void Widget::setupUI()
{
    ledtIP = new QLineEdit("192.1.0.66");
    spboxPort = new QSpinBox();
    spboxPort->setRange(0, 100000);
    spboxPort->setValue(8000);
    ledtUserName = new QLineEdit("admin");
    ledtUserPassword = new QLineEdit("hik12345");
    pbtnConnect = new QPushButton("连接设备");
    pbtnDisconnect = new QPushButton("断开设备");
    txtedtInfo = new QTextEdit();

    QVBoxLayout* vLayout = new QVBoxLayout();
    this->setLayout(vLayout);
    QFormLayout* fLayout = new QFormLayout();
    vLayout->addLayout(fLayout);
    fLayout->addRow("设备IP", ledtIP);
    fLayout->addRow("设备端口", spboxPort);
    fLayout->addRow("用户名", ledtUserName);
    fLayout->addRow("密码", ledtUserPassword);
    fLayout->addRow(pbtnConnect, pbtnDisconnect);
    fLayout->addRow(txtedtInfo);

}

void Widget::initConnection()
{
    connect(pbtnConnect, &QAbstractButton::clicked, this, &Widget::connectRader);
    connect(pbtnDisconnect, &QAbstractButton::clicked, this, &Widget::disconnectRader);
}

void Widget::connectRader()
{
    //---------------------------------------
    //注册设备
    //登录参数，包括设备地址、登录用户、密码等
    NET_DVR_USER_LOGIN_INFO struLoginInfo = { 0 };
    struLoginInfo.bUseAsynLogin = 0; //同步登录方式
    strcpy(struLoginInfo.sDeviceAddress, ledtIP->text().toStdString().data()); //设备IP地址
    struLoginInfo.wPort = spboxPort->value(); //设备服务端口
    strcpy(struLoginInfo.sUserName, ledtUserName->text().toStdString().data()); //设备登录用户名
    strcpy(struLoginInfo.sPassword, ledtUserPassword->text().toStdString().data()); //设备登录密码
    //设备信息, 输出参数
    NET_DVR_DEVICEINFO_V40 struDeviceInfoV40 = { 0 };
    mUserID = NET_DVR_Login_V40(&struLoginInfo, &struDeviceInfoV40);
    if (mUserID < 0)
    {
        printf("Login failed, error code: %d\n", NET_DVR_GetLastError());
        txtedtInfo->append("连接失败");
        return;
    }

    //建立获取雷达视频目标信息的长连接
    NET_DVR_ALARM_SEARCH_COND struCond = { 0 };
    struCond.dwSize = sizeof(struCond);
    struCond.dwAlarmComm = 0x4993;//0x4993-智能检测报警(雷达视频目标检测报警)
    struCond.wEventType = 2; //0-表示所有事件, 1-混合目标检测（mixedTargetDetection）, 2-雷达视频目标检测（radarVideoDetection）
    struCond.wSubEventType = 0;//子事件类型，0-表示所有子事件
    struCond.bySupport = 1;

    //启动长连接
    mHandle = NET_DVR_StartRemoteConfig(mUserID, NET_DVR_GET_ALARM_INFO, &struCond, sizeof(struCond), fRadarCallback, this);
    if (mHandle == -1)
    {
        txtedtInfo->append("连接失败");
        printf("NET_DVR_GET_ALARM_INFO failed, error code: %d\n", NET_DVR_GetLastError());
        NET_DVR_Logout(mUserID);
        return;
    }
    txtedtInfo->append("连接成功");
    qDebug() << "连接成功";
    mLinkStatus = true;//长连接状态
}

void Widget::disconnectRader()
{
    //关闭长连接
    if (!NET_DVR_StopRemoteConfig(mHandle))
    {
        txtedtInfo->append("断开失败");
        printf("NET_DVR_StopRemoteConfig failed, error code: %d\n", NET_DVR_GetLastError());
    }
    //注销用户
    NET_DVR_Logout(mUserID);
    qDebug() << "断开成功";
    txtedtInfo->append("断开成功");
}



