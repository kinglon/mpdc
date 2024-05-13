#include "collectorfactory.h"
#include "douyincollector.h"
#include "kuaishoucollector.h"
#include "weibocollector.h"

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
    else if(link.contains("www.kuaishou.com/short-video"))
    {
        collector = new KuaiShouCollector();
    }
    else if (link.contains("weibo.com"))
    {
        collector = new WeiboCollector();
    }

    return collector;
}
