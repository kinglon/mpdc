#include "weixincollector.h"
#include "browserwindow.h"

// 采集数据重传最大次数
#define MAX_COLLECT_DATA_RETRY_COUNT  5

WeixinCollector::WeixinCollector(QObject *parent)
    : CollectorBase{parent}
{

}

void WeixinCollector::runJsCodeFinish(bool ok, const QMap<QString, QString>& result)
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

        if (!m_dataModel.m_userId.isEmpty())
        {
            collectDataCompletely(true);
        }
    }
}

bool WeixinCollector::isReady(const QMap<QString, QString>& result, bool& validLink)
{
    QString fun;
    if (result.contains("fun"))
    {
        fun = result["fun"];
    }
    if (fun.isEmpty())
    {
        qCritical("js result not have fun");
        return false;
    }

    if (fun == "check_ready")
    {
        if (result.contains("ready"))
        {
            if (result["ready"] == "1") // 就绪
            {
                validLink = true;
                return true;
            }
            else if (result["ready"] == "2") // 内容不存在
            {
                validLink = false;
                return true;
            }
        }
    }

    return false;
}

void WeixinCollector::doStepCollectData()
{
    m_collectDataTimer = new QTimer(this);
    connect(m_collectDataTimer, &QTimer::timeout, [this]() {
        if (m_collectingDataRetryCount >= MAX_COLLECT_DATA_RETRY_COUNT)
        {
            collectDataCompletely(false);
            return;
        }

        runJsCodeFile("weixin_collect_data");
        m_collectingDataRetryCount++;
    });
    m_collectDataTimer->setInterval(2000);
    m_collectDataTimer->start();
}

void WeixinCollector::collectDataCompletely(bool ok)
{
    m_collectDataTimer->stop();
    m_collectDataTimer->deleteLater();
    m_collectDataTimer = nullptr;
    stepCollectDataFinish(ok);
}
