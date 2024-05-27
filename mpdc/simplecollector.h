#ifndef SIMPLECOLLECTOR_H
#define SIMPLECOLLECTOR_H

#include "ccollectorbase.h"
#include <QTimer>

class SimpleCollector : public CollectorBase
{
public:
    explicit SimpleCollector(QString readyJsFileName, QString collectDataJsFileName, QObject *parent = nullptr);

protected:
    virtual void runJsCodeFinish(bool ok, const QMap<QString, QString>& result) override;

    virtual QString getWaitReadyJsFile() override { return m_readyJsFileName; }

    virtual void doStepCollectData() override;

private:
    void collectDataCompletely(bool ok);

private:
    QString m_readyJsFileName;

    QString m_collectDataJsFileName;

    QTimer* m_collectDataTimer = nullptr;
};

#endif // SIMPLECOLLECTOR_H
