#ifndef DOUYINCOLLECTOR_H
#define DOUYINCOLLECTOR_H

#include "ccollectorbase.h"
#include <QTimer>

class DouyinCollector : public CollectorBase
{
public:
    explicit DouyinCollector(QObject *parent = nullptr);

protected:
    virtual void runJsCodeFinish(bool ok, const QMap<QString, QString>& result) override;

    virtual void onSubClassLoadUrlFinished(bool ok) override;

    virtual QString getWaitReadyJsFile() override { return "douyin_check_ready"; }

    virtual void doStepCaptureImage() override;

    virtual void doStepCollectData() override;

private:
    void captureImageCompletely(bool ok);

    void collectDataCompletely(bool ok);

private:
    bool m_hasCaptured = false;

    QTimer* m_captureImageTimer = nullptr;

    QTimer* m_collectDataTimer = nullptr;
};

#endif // DOUYINCOLLECTOR_H
