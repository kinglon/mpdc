#include "douyincollector.h"
#include "browserwindow.h"


DouyinCollector::DouyinCollector(QObject *parent)
    : CollectorBase{parent}
{

}

void DouyinCollector::runJsCodeFinish(bool ok, const QMap<QString, QString>& result)
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

    if (fun == "close_login_window")
    {
        if (!m_hasCaptured || (result.contains("login_window_visible") && result["login_window_visible"] == "1"))
        {
            m_hasCaptured = true;
            QString imageSavePath = getCaptureImageSavePath();
            bool ok = BrowserWindow::getInstance()->captrueImage(imageSavePath);
            if (!ok)
            {
                captureImageCompletely(false);
            }
            else
            {
                // 继续查询登录窗口是否显示，如果显示要关闭再次截图
                runJsCodeFile("douyin_close_login_window");
            }
        }
        else
        {
            captureImageCompletely(true);
        }
    }
    else if (fun == "collect_data")
    {
        if (result.contains("title"))
        {
            m_dataModel.m_title = result["title"];
        }

        if (result.contains("nickName"))
        {
            m_dataModel.m_nickName = result["nickName"];
        }

        if (result.contains("fanCount"))
        {
            m_dataModel.m_fanCount = result["fanCount"];
        }
    }
    else if (fun == "get_id")
    {
        if (result.contains("id"))
        {
            m_dataModel.m_userId = result["id"];
        }
        collectDataCompletely(true);
    }
}

void DouyinCollector::onSubClassLoadUrlFinished(bool ok)
{
    if (m_currentStep == STEP_COLLECT_DATA)
    {
        if (ok)
        {
            // 页面跳转成功，可以去采集用户ID
            if (!runJsCodeFile("douyin_get_id"))
            {
                collectDataCompletely(false);
                return;
            }
        }
    }
}

bool DouyinCollector::isReady(const QMap<QString, QString>& result, bool& validLink)
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
            else if (result["ready"] == "2") // 没有视频
            {
                validLink = false;
                return true;
            }
        }
    }

    return false;
}

void DouyinCollector::doStepCaptureImage()
{   
    // 先关闭登录窗口
    if (!runJsCodeFile("douyin_close_login_window"))
    {
        captureImageCompletely(false);
        return;
    }

    m_captureImageTimer = new QTimer(this);
    connect(m_captureImageTimer, &QTimer::timeout, [this]() {
        captureImageCompletely(false);
    });
    m_captureImageTimer->setInterval(10000);
    m_captureImageTimer->start();
}

void DouyinCollector::doStepCollectData()
{
    if (!runJsCodeFile("douyin_collect_data"))
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

void DouyinCollector::captureImageCompletely(bool ok)
{
    m_captureImageTimer->stop();
    m_captureImageTimer->deleteLater();
    m_captureImageTimer = nullptr;
    stepCaptureImageFinish(ok);
}

void DouyinCollector::collectDataCompletely(bool ok)
{
    m_collectDataTimer->stop();
    m_collectDataTimer->deleteLater();
    m_collectDataTimer = nullptr;
    stepCollectDataFinish(ok);
}
