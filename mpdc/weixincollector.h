#ifndef WEIXINCOLLECTOR_H
#define WEIXINCOLLECTOR_H

#include "ccollectorbase.h"
#include <QTimer>

class WeixinCollector : public CollectorBase
{
public:
    explicit WeixinCollector(QObject *parent = nullptr);

protected:
    virtual void runJsCodeFinish(bool ok, const QMap<QString, QString>& result) override;

    virtual QString getWaitReadyJsFile() override { return "weixin_check_ready"; }

    virtual bool isReady(const QMap<QString, QString>& result, bool& validLink) override;

    virtual void doStepCollectData() override;

private:
    void collectDataCompletely(bool ok);

private:
    QTimer* m_collectDataTimer = nullptr;

    int m_collectingDataRetryCount = 0;
};

#endif // WEIXINCOLLECTOR_H
