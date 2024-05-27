#include "kuaishoucollector.h"
#include "browserwindow.h"

// 采集数据重传最大次数
#define MAX_COLLECT_DATA_RETRY_COUNT  5

KuaiShouCollector::KuaiShouCollector(QString readyJsFileName, QString collectDataJsFileName,
                                     QString secondCollectJsFile, QObject *parent)
    : TwoTimeCollector(readyJsFileName, collectDataJsFileName, secondCollectJsFile, parent)
{

}

void KuaiShouCollector::runJsCodeFinish(bool ok, const QMap<QString, QString>& result)
{
    __super::runJsCodeFinish(ok, result);

    if (!ok)
    {
        return;
    }

    QString fun;
    if (result.contains("fun"))
    {
        fun = result["fun"];
    }
    if (fun.isEmpty())
    {
        qCritical("js result not have fun");
        return;
    }

    if (fun == "collect_data2")
    {
        if (result.contains("login") && result["login"] == "0")
        {
            m_collectError = COLLECT_ERROR_NOT_LOGIN;
            collectDataCompletely(false);
        }
    }
}
