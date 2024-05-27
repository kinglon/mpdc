#ifndef TOUTIAOCOLLECTOR_H
#define TOUTIAOCOLLECTOR_H

#include "ccollectorbase.h"
#include <QTimer>

class ToutiaoCollector : public CollectorBase
{
public:
    explicit ToutiaoCollector(QObject *parent = nullptr);

protected:
    virtual void onSubClassLoadUrlFinished(bool ok) override;

    virtual void runJsCodeFinish(bool ok, const QMap<QString, QString>& result) override;

    virtual QString getWaitReadyJsFile() override { return "toutiao_check_ready"; }

    virtual void doStepCollectData() override;

private:
    void collectDataCompletely(bool ok);

    void doStepCollectData2();

private:
    QTimer* m_collectDataTimer = nullptr;

    int m_collectingDataRetryCount = 0;
};

#endif // TOUTIAOCOLLECTOR_H
