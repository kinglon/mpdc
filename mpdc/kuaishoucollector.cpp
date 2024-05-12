#include "kuaishoucollector.h"
#include "browserwindow.h"

// 采集数据重传最大次数
#define MAX_COLLECT_DATA_RETRY_COUNT  5

KuaiShouCollector::KuaiShouCollector(QObject *parent)
    : CollectorBase{parent}
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
    }
    else if (fun == "collect_data2")
    {
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
        else
        {
            if (m_collectingDataRetryCount >= MAX_COLLECT_DATA_RETRY_COUNT)
            {
                if (result.contains("login") && result["login"] == "0")
                {
                    m_collectError = COLLECT_ERROR_NOT_LOGIN;
                }
                collectDataCompletely(false);
            }
            else
            {
                m_collectDataTimer->stop();
                m_collectDataTimer->disconnect();
                connect(m_collectDataTimer, &QTimer::timeout, [this]() {
                    runJsCodeFile("kuaishou_collect_data2");
                    m_collectingDataRetryCount++;
                });
                m_collectDataTimer->setInterval(2000);
                m_collectDataTimer->start();
            }
        }
    }
}

void KuaiShouCollector::onSubClassLoadUrlFinished(bool ok)
{
    if (m_currentStep == STEP_COLLECT_DATA)
    {
        if (ok)
        {
            // 页面跳转成功，可以去采集其他信息
            if (!runJsCodeFile("kuaishou_collect_data2"))
            {
                collectDataCompletely(false);
                return;
            }
        }
    }
}

bool KuaiShouCollector::isReady(const QMap<QString, QString>& result, bool& hasVideo)
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
                hasVideo = true;
                return true;
            }
            else if (result["ready"] == "2") // 没有视频
            {
                hasVideo = false;
                return true;
            }
        }
    }

    return false;
}

void KuaiShouCollector::doStepCollectData()
{
    if (!runJsCodeFile("kuaishou_collect_data"))
    {
        collectDataCompletely(false);
        return;
    }

    m_collectDataTimer = new QTimer(this);
    connect(m_collectDataTimer, &QTimer::timeout, [this]() {
        qCritical("collecting data timeout");
        collectDataCompletely(false);
    });
    m_collectDataTimer->setInterval(60000);
    m_collectDataTimer->start();
}

void KuaiShouCollector::collectDataCompletely(bool ok)
{
    m_collectDataTimer->stop();
    m_collectDataTimer->deleteLater();
    m_collectDataTimer = nullptr;
    stepCollectDataFinish(ok);
}
