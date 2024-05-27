#ifndef WEIBOCOLLECTOR_H
#define WEIBOCOLLECTOR_H

#include "ccollectorbase.h"
#include <QTimer>

class WeiboCollector : public CollectorBase
{
public:
    explicit WeiboCollector(QObject *parent = nullptr);

protected:
    virtual void runJsCodeFinish(bool ok, const QMap<QString, QString>& result) override;

    virtual QString getWaitReadyJsFile() override { return "weibo_check_ready"; }

    virtual void doStepCollectData() override;

private:
    void collectDataCompletely(bool ok);

private:
    QTimer* m_collectDataTimer = nullptr;

    int m_collectingDataRetryCount = 0;
};

#endif // WEIBOCOLLECTOR_H
