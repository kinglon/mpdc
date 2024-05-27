#include "toutiaocollector.h"
#include "browserwindow.h"

// 采集数据重传最大次数
#define MAX_COLLECT_DATA_RETRY_COUNT  10

ToutiaoCollector::ToutiaoCollector(QObject *parent)
    : CollectorBase{parent}
{

}

void ToutiaoCollector::onSubClassLoadUrlFinished(bool ok)
{
    if (m_currentStep == STEP_COLLECT_DATA)
    {
        if (ok)
        {
            // 页面跳转成功，进行第2阶段采集
            m_collectingDataRetryCount = 0;
            doStepCollectData2();
        }
        else
        {
            collectDataCompletely(false);
        }
    }
}

void ToutiaoCollector::runJsCodeFinish(bool ok, const QMap<QString, QString>& result)
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
                collectDataCompletely(false);
            }
        }
    }
}

void ToutiaoCollector::doStepCollectData()
{
    if (!runJsCodeFile("toutiao_collect_data"))
    {
        collectDataCompletely(false);
        return;
    }    

    m_collectDataTimer = new QTimer(this);
    connect(m_collectDataTimer, &QTimer::timeout, [this]() {
        qCritical("collecting data timeout");
        collectDataCompletely(false);
    });
    m_collectDataTimer->setInterval(20000);
    m_collectDataTimer->start();
}

void ToutiaoCollector::doStepCollectData2()
{
    m_collectDataTimer->stop();
    m_collectDataTimer->disconnect();
    connect(m_collectDataTimer, &QTimer::timeout, [this]() {
        runJsCodeFile("toutiao_collect_data2");
        m_collectingDataRetryCount++;
    });
    m_collectDataTimer->setInterval(1000);
    m_collectDataTimer->start();
}

void ToutiaoCollector::collectDataCompletely(bool ok)
{
    m_collectDataTimer->stop();
    m_collectDataTimer->deleteLater();
    m_collectDataTimer = nullptr;
    stepCollectDataFinish(ok);
}
