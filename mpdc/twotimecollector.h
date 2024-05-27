#ifndef TWOTIMECOLLECTOR_H
#define TWOTIMECOLLECTOR_H

#include "simplecollector.h"
#include <QTimer>

// 二次采集器，第一次采集与SimpleCollector一样，第一次采集的同时会打开一个新链接，然后在第2个页面上继续采集
class TwoTimeCollector : public SimpleCollector
{
public:
    explicit TwoTimeCollector(QString readyJsFileName, QString collectDataJsFileName,
                              QString secondCollectJsFile, QObject *parent = nullptr);

protected:
    virtual void onSubClassLoadUrlFinished(bool ok) override;

    virtual void runJsCodeFinish(bool ok, const QMap<QString, QString>& result) override;

    virtual bool needCollectMoreData() override { return true; }

private:
    void doStepCollectData2();

private:
    QString m_secondCollectJsFile;

    int m_collectingDataRetryCount = 0;
};

#endif // TWOTIMECOLLECTOR_H
