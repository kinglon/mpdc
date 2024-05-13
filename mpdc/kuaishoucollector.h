#ifndef KUAISHOUCOLLECTOR_H
#define KUAISHOUCOLLECTOR_H

#include "ccollectorbase.h"
#include <QTimer>

class KuaiShouCollector : public CollectorBase
{
public:
    explicit KuaiShouCollector(QObject *parent = nullptr);

protected:
    virtual void runJsCodeFinish(bool ok, const QMap<QString, QString>& result) override;

    virtual void onSubClassLoadUrlFinished(bool ok) override;

    virtual QString getWaitReadyJsFile() override { return "kuaishou_check_ready"; }

    virtual bool isReady(const QMap<QString, QString>& result, bool& validLink) override;

    virtual void doStepCollectData() override;

private:
    void collectDataCompletely(bool ok);

private:
    QTimer* m_collectDataTimer = nullptr;

    int m_collectingDataRetryCount = 0;
};

#endif // KUAISHOUCOLLECTOR_H
