#ifndef CCOLLECTORBASE_H
#define CCOLLECTORBASE_H

#include <QObject>
#include <QMap>
#include <QTimer>
#include "datamodel.h"

// 步骤
#define STEP_INIT           0
#define STEP_LOADURL        1
#define STEP_WAIT_READY     2
#define STEP_CAPTURE_IMAGE  3
#define STEP_COLLECT_DATA   4
#define STEP_FINISH         5

// 采集失败原因
#define COLLECT_ERROR                   1
#define COLLECT_ERROR_INVALID_LINK      2  // 无效链接
#define COLLECT_ERROR_NOT_LOGIN         3  // 未登录

class CollectorBase : public QObject
{
    Q_OBJECT
public:
    explicit CollectorBase(QObject *parent = nullptr);

public:
    // 获取数据模型对象
    DataModel& getDataModel() { return m_dataModel;}

    // 设置采集计划的名字
    void setPlanName(QString planName) { m_planName = planName; }

    // 获取采集结果保存的路径
    static QString getCollectResultSavePath(QString planName);

    // 运行
    void run();

    // 获取采集失败原因
    int getCollectError() { return m_collectError; }

signals:
    // 运行结束
    void runFinish(bool ok);

    // 采集日志
    void collectLog(const QString& log);

private slots:
    void onRunJsCodeFinish(const QVariant& result);

    void onRunJsCodeTimeout();

    void onStepTimeout();

    void onLoadUrlFinished(bool ok);

protected:
    // 执行JS脚本
    bool runJsCode(const QString& jsCode);

    // 执行JS脚本文件
    bool runJsCodeFile(const QString& jsFileName);

    // JS脚本执行结果
    virtual void runJsCodeFinish(bool ok, const QMap<QString, QString>& result);

    // 子类重载，URL加载完成后除非
    virtual void onSubClassLoadUrlFinished(bool ok) {(void)ok;}

    // 获取等待准备就绪的JS脚本文件
    virtual QString getWaitReadyJsFile() = 0;

    // 检查是否已准备就绪
    // result, JS代码运行的结果
    // validLink, 标志是否未有效链接
    // return， true表示就绪，就绪结果要看validLink
    virtual bool isReady(const QMap<QString, QString>& result, bool& validLink);

    // 获取截图保存路径
    QString getCaptureImageSavePath();

protected:
    void doStepLoadUrl();

    void doStepWaitReady();

    void stepWaitReadyFinish(bool ok);

    virtual void doStepCaptureImage();

    void stepCaptureImageFinish(bool ok);

    virtual void doStepCollectData() = 0;

    void stepCollectDataFinish(bool ok);

protected:
    DataModel m_dataModel;

    QString m_planName;

    // 当前采集步骤
    int m_currentStep = STEP_INIT;

    // 采集失败原因
    int m_collectError = COLLECT_ERROR;

private:
    // 当前JS执行会话ID，0表示没有JS在执行
    int m_currentJsSessionId = 0;

    // JS执行超时定时器
    QTimer* m_runJsCodeTimer = nullptr;

    // JS执行超时毫秒数
    int m_runJsCodeTimeout = 5000;    

    // 步骤定时器，用于控制超时
    QTimer* m_stepTimer = nullptr;

    // 步骤失败重试次数
    int m_stepRetryCount = 0;
};

#endif // CCOLLECTORBASE_H
