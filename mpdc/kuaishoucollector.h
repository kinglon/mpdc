#ifndef KUAISHOUCOLLECTOR_H
#define KUAISHOUCOLLECTOR_H

#include "twotimecollector.h"
#include <QTimer>

class KuaiShouCollector : public TwoTimeCollector
{
public:
    explicit KuaiShouCollector(QString readyJsFileName, QString collectDataJsFileName,
                               QString secondCollectJsFile, QObject *parent = nullptr);

protected:
    virtual void runJsCodeFinish(bool ok, const QMap<QString, QString>& result) override;
};

#endif // KUAISHOUCOLLECTOR_H
