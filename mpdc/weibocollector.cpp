#include "weibocollector.h"
#include "browserwindow.h"

// 采集数据重传最大次数
#define MAX_COLLECT_DATA_RETRY_COUNT  5

WeiboCollector::WeiboCollector(QObject *parent)
    : CollectorBase{parent}
{

}

void WeiboCollector::runJsCodeFinish(bool ok, const QMap<QString, QString>& result)
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

    if (fun == "collect_data")
    {
        if (result.contains("title"))
        {
            m_dataModel.m_title = result["title"];
        }

        if (result.contains("nickName"))
        {
            m_dataModel.m_nickName = result["nickName"];
        }

        if (result.contains("id"))
        {
            m_dataModel.m_userId = result["id"];
        }

        if (result.contains("fanCount"))
        {
            m_dataModel.m_fanCount = result["fanCount"];
        }

        if (!m_dataModel.m_fanCount.isEmpty())
        {
            collectDataCompletely(true);
        }
    }
}

void WeiboCollector::doStepCollectData()
{
    m_collectDataTimer = new QTimer(this);
    connect(m_collectDataTimer, &QTimer::timeout, [this]() {
        if (m_collectingDataRetryCount >= MAX_COLLECT_DATA_RETRY_COUNT)
        {
            collectDataCompletely(false);
            return;
        }

        runJsCodeFile("weibo_collect_data");
        m_collectingDataRetryCount++;
    });
    m_collectDataTimer->setInterval(2000);
    m_collectDataTimer->start();
}

void WeiboCollector::collectDataCompletely(bool ok)
{
    m_collectDataTimer->stop();
    m_collectDataTimer->deleteLater();
    m_collectDataTimer = nullptr;
    stepCollectDataFinish(ok);
}
