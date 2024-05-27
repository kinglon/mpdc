#include "simplecollector.h"
#include "browserwindow.h"

SimpleCollector::SimpleCollector(QString readyJsFileName, QString collectDataJsFileName, QObject *parent)
    : CollectorBase{parent}
{
    m_readyJsFileName = readyJsFileName;
    m_collectDataJsFileName = collectDataJsFileName;
}

void SimpleCollector::runJsCodeFinish(bool ok, const QMap<QString, QString>& result)
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

        collectDataCompletely(true);
    }
}

void SimpleCollector::doStepCollectData()
{
    if (!runJsCodeFile(m_collectDataJsFileName))
    {
        collectDataCompletely(false);
        return;
    }

    m_collectDataTimer = new QTimer(this);
    connect(m_collectDataTimer, &QTimer::timeout, [this]() {
        qCritical("collecting data timeout");
        collectDataCompletely(false);
    });
    m_collectDataTimer->setInterval(5000);
    m_collectDataTimer->start();
}

void SimpleCollector::collectDataCompletely(bool ok)
{
    m_collectDataTimer->stop();
    m_collectDataTimer->deleteLater();
    m_collectDataTimer = nullptr;
    stepCollectDataFinish(ok);
}
