#ifndef COLLECTORFACTORY_H
#define COLLECTORFACTORY_H

#include "ccollectorbase.h"

class CollectorFactory
{
public:
    CollectorFactory();

public:
    // 根据链接，创建收集器
    static CollectorBase* createCollector(const QString& link);
};

#endif // COLLECTORFACTORY_H
