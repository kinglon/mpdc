#include "twotimecollector.h"
#include "browserwindow.h"

// 采集数据重传最大次数
#define MAX_COLLECT_DATA_RETRY_COUNT  10

TwoTimeCollector::TwoTimeCollector(QString readyJsFileName, QString collectDataJsFileName,
                                   QString secondCollectJsFile, QObject *parent)
    : SimpleCollector(readyJsFileName, collectDataJsFileName, parent)
{
    m_secondCollectJsFile = secondCollectJsFile;
}

void TwoTimeCollector::onSubClassLoadUrlFinished(bool ok)
{
    if (m_currentStep == STEP_COLLECT_DATA)
    {
        // 进行第2阶段采集
        m_collectingDataRetryCount = 0;
        doStepCollectData2();
    }
}

void TwoTimeCollector::runJsCodeFinish(bool ok, const QMap<QString, QString>& result)
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

void TwoTimeCollector::doStepCollectData2()
{
    m_collectDataTimer->stop();
    m_collectDataTimer->disconnect();
    connect(m_collectDataTimer, &QTimer::timeout, [this]() {
        runJsCodeFile(m_secondCollectJsFile);
        m_collectingDataRetryCount++;
    });
    m_collectDataTimer->setInterval(1000);
    m_collectDataTimer->start();
}
