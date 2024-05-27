#ifndef SIMPLECOLLECTOR_H
#define SIMPLECOLLECTOR_H

#include "ccollectorbase.h"
#include <QTimer>

// 加载链接准备就绪后，所有的数据采集都在当前网页上
class SimpleCollector : public CollectorBase
{
public:
    explicit SimpleCollector(QString readyJsFileName, QString collectDataJsFileName, QObject *parent = nullptr);

protected:
    virtual void runJsCodeFinish(bool ok, const QMap<QString, QString>& result) override;

    virtual QString getWaitReadyJsFile() override { return m_readyJsFileName; }

    virtual void doStepCollectData() override;

    // 是否需要继续获取更多数据，如果不需要的话当前网页采集完后就结束
    virtual bool needCollectMoreData() { return false; }

protected:
    void collectDataCompletely(bool ok);

protected:
    QString m_readyJsFileName;

    QString m_collectDataJsFileName;

    QTimer* m_collectDataTimer = nullptr;
};

#endif // SIMPLECOLLECTOR_H
