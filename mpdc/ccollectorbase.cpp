#include "ccollectorbase.h"
#include "browserwindow.h"
#include "settingmanager.h"
#include "Utility/ImPath.h"
#include <QDir>

CollectorBase::CollectorBase(QObject *parent)
    : QObject{parent},
      m_runJsCodeTimer(new QTimer(this)),
      m_stepTimer(new QTimer(this))
{
    connect(BrowserWindow::getInstance(), &BrowserWindow::runJsCodeFinished,
            this, &CollectorBase::onRunJsCodeFinish);
    m_runJsCodeTimer->setInterval(m_runJsCodeTimeout);
    connect(m_runJsCodeTimer, &QTimer::timeout, this, &CollectorBase::onRunJsCodeTimeout);
    connect(m_stepTimer, &QTimer::timeout, this, &CollectorBase::onStepTimeout);
    connect(BrowserWindow::getInstance(), &BrowserWindow::loadFinished, this, &CollectorBase::onLoadUrlFinished);
}

void CollectorBase::run()
{
    if (m_dataModel.m_link.isEmpty() || m_dataModel.m_id.isEmpty())
    {
        qCritical("the data model is wrong");
        return;
    }

    if (m_dataSaveFolderName.isEmpty())
    {
        qCritical("the data folder is not setted");
        return;
    }

    QString log = QString::fromWCharArray(L"开始采集链接") + m_dataModel.m_id;
    emit collectLog(log);

    m_currentStep = STEP_LOADURL;
    m_stepRetryCount = 0;
    emit collectLog(QString::fromWCharArray(L"加载链接"));
    doStepLoadUrl();
}

bool CollectorBase::runJsCode(const QString& jsCode)
{
    if (jsCode.isEmpty())
    {
        qCritical("the js code is empty");
        return false;
    }

    // 上一次还没执行完，打下日志
    if (m_currentJsSessionId > 0)
    {
        qWarning("the last time of running js code not finish");
        m_runJsCodeTimer->stop();
    }

    // 返回结果变量固定为jsResult，把id添加到返回的结果，用于匹配
    static int sessionId = 1;
    ++sessionId;
    m_currentJsSessionId = sessionId;
    QString sessionJsCode = QString("; jsResult['sessionId']='%1'; jsResult;").arg(m_currentJsSessionId);
    QString newJsCode = jsCode + sessionJsCode;
    BrowserWindow::getInstance()->runJsCode(newJsCode);
    m_runJsCodeTimer->start();

    return true;
}

bool CollectorBase::runJsCodeFile(const QString& jsFileName)
{
    static QMap<QString, QString> jsCodes; // 缓存JS代码
    QString jsCode;
    if (!CSettingManager::GetInstance()->m_cacheJsCode || !jsCodes.contains(jsFileName))
    {
        // 从文件加载
        std::wstring jsFilePath = CImPath::GetSoftInstallPath() + L"\\Js\\" + jsFileName.toStdWString();
        QFile file(QString::fromStdWString(jsFilePath));
        if (file.open(QIODevice::ReadOnly))
        {
            QByteArray jsonData = file.readAll();
            file.close();
            jsCode = QString::fromUtf8(jsonData);
        }
    }
    else
    {
        jsCode = jsCodes[jsFileName];
    }

    if (jsCode.isEmpty())
    {
        qCritical("the js file (%s) not have content", jsFileName.toStdString().c_str());
        return false;
    }

    if (CSettingManager::GetInstance()->m_cacheJsCode)
    {
        jsCodes[jsFileName] = jsCode;
    }

    return runJsCode(jsCode);
}

void CollectorBase::onRunJsCodeFinish(const QVariant& result)
{
    if (m_currentJsSessionId == 0)
    {
        return;
    }

    if (result.type() != QVariant::Map)
    {
        qCritical("the js result is not a map");
        return;
    }

    QMap map = result.toMap();
    if (!map.contains("sessionId") || map["sessionId"].type() != QVariant::String)
    {
        qCritical("the js result map not have sessionId member");
        return;
    }

    if (map["sessionId"].toString() != QString::number(m_currentJsSessionId))
    {
        qWarning("the session id is old");
        return;
    }

    m_runJsCodeTimer->stop();
    m_currentJsSessionId = 0;

    QMap<QString, QString> newMap;
    QList<QString> keys = map.keys();
    for (QString key : keys)
    {
        newMap[key] = map[key].toString();
    }
    runJsCodeFinish(true, newMap);
}

void CollectorBase::onRunJsCodeTimeout()
{
    qCritical("running js code time out");
    m_runJsCodeTimer->stop();
    m_currentJsSessionId = 0;
    runJsCodeFinish(false, QMap<QString, QString>());
}

QString CollectorBase::getCaptureImageSavePath()
{
    std::wstring dataPath = CImPath::GetDataPath();
    QString qDataPath = QString::fromStdWString(dataPath);
    qDataPath += m_dataSaveFolderName + "\\";
    if (!QDir(qDataPath).exists())
    {
      QDir().mkdir(qDataPath);
    }
    return qDataPath + m_dataModel.m_id + ".png";
}

void CollectorBase::doStepLoadUrl()
{
    m_stepTimer->setInterval(60000);
    m_stepTimer->start();
    BrowserWindow::getInstance()->load(QUrl(m_dataModel.m_link));
}

void CollectorBase::stepWaitReadyFinish(bool ok)
{
    if (!ok)
    {
        if (m_collectError == COLLECT_ERROR_NOT_HAVE_VIDEO)
        {
            emit collectLog(QString::fromWCharArray(L"无效视频"));
        }
        else
        {
            emit collectLog(QString::fromWCharArray(L"等待页面就绪超时"));
        }
        runFinish(false);
    }
    else
    {
        m_currentStep = STEP_CAPTURE_IMAGE;
        m_stepRetryCount = 0;
        emit collectLog(QString::fromWCharArray(L"准备截图"));
        doStepCaptureImage();
    }
}

void CollectorBase::doStepCaptureImage()
{
    QString imageSavePath = getCaptureImageSavePath();
    bool ok = BrowserWindow::getInstance()->captrueImage(imageSavePath);
    stepCaptureImageFinish(ok);
}

void CollectorBase::stepCaptureImageFinish(bool ok)
{
    if (!ok)
    {
        emit collectLog(QString::fromWCharArray(L"截图失败"));
        runFinish(false);
    }
    else
    {
        m_currentStep = STEP_COLLECT_DATA;
        m_stepRetryCount = 0;
        emit collectLog(QString::fromWCharArray(L"准备采集数据"));
        doStepCollectData();
    }
}

void CollectorBase::stepCollectDataFinish(bool ok)
{
    if (!ok)
    {
        emit collectLog(QString::fromWCharArray(L"采集数据失败"));
        runFinish(false);
    }
    else
    {
        m_currentStep = STEP_FINISH;
        m_stepRetryCount = 0;
        emit collectLog(QString::fromWCharArray(L"采集完毕"));
        runFinish(true);
    }
}

void CollectorBase::onLoadUrlFinished(bool ok)
{
    onSubClassLoadUrlFinished(ok);

    if (m_currentStep == STEP_LOADURL)
    {
        m_stepTimer->stop();
        if (!ok)
        {
            if (m_stepRetryCount < 1)
            {
                emit collectLog(QString::fromWCharArray(L"加载链接失败，重试"));
                doStepLoadUrl();
            }
            else
            {
                emit collectLog(QString::fromWCharArray(L"采集失败：加载链接失败"));
                emit runFinish(false);
            }
        }
        else
        {
            m_currentStep = STEP_WAIT_READY;
            m_stepRetryCount = 0;
            emit collectLog(QString::fromWCharArray(L"等待页面就绪"));
            doStepWaitReady();
        }
    }
}

void CollectorBase::onStepTimeout()
{
    m_stepTimer->stop();

    if (m_currentStep == STEP_LOADURL)
    {
        if (m_stepRetryCount < 1)
        {
            emit collectLog(QString::fromWCharArray(L"加载链接超时，重试"));
            doStepLoadUrl();
        }
        else
        {
            emit collectLog(QString::fromWCharArray(L"采集失败：加载链接超时"));
            emit runFinish(false);
        }
    }
}
