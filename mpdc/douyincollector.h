#ifndef DOUYINCOLLECTOR_H
#define DOUYINCOLLECTOR_H

#include "ccollectorbase.h"
#include <QTimer>

class DouyinCollector : public CollectorBase
{
public:
    explicit DouyinCollector(QObject *parent = nullptr);

public slots:
    void onWaitReadyTimer();

protected:
    virtual void runJsCodeFinish(bool ok, const QMap<QString, QString>& result) override;

    virtual void onSubClassLoadUrlFinished(bool ok) override;

    virtual void doStepWaitReady() override;

    virtual void doStepCaptureImage() override;

    virtual void doStepCollectData() override;

private:
    void waitReadyCompletely(bool ok);

    void captureImageCompletely(bool ok);

    void collectDataCompletely(bool ok);

private:
    QTimer* m_waitReadyTimer = nullptr;

    int m_waitReadyCount = 0;

    bool m_hasCaptured = false;

    QTimer* m_captureImageTimer = nullptr;

    QTimer* m_collectDataTimer = nullptr;
};

#endif // DOUYINCOLLECTOR_H
