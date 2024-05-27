#include "collectorfactory.h"
#include "douyincollector.h"
#include "kuaishoucollector.h"
#include "weibocollector.h"
#include "weixincollector.h"
#include "toutiaocollector.h"

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
    else if (link.contains("mp.weixin.qq.com"))
    {
        collector = new WeixinCollector();
    }
    else if (link.contains("toutiao.com"))
    {
        collector = new ToutiaoCollector();
    }

    return collector;
}
