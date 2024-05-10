#include "collectorfactory.h"
#include "douyincollector.h"

CollectorFactory::CollectorFactory()
{

}

CollectorBase* CollectorFactory::createCollector(const QString& link)
{
    CollectorBase* collector = nullptr;
    if (link.contains("www.douyin.com/video"))
    {
         collector = new DouyinCollector();
    }

    return collector;
}
