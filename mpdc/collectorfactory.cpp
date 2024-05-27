#include "collectorfactory.h"
#include "douyincollector.h"
#include "kuaishoucollector.h"
#include "weibocollector.h"
#include "weixincollector.h"
#include "toutiaocollector.h"
#include "simplecollector.h"

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
        collector = new SimpleCollector("weibo_check_ready", "weibo_collect_data");
    }
    else if (link.contains("mp.weixin.qq.com"))
    {
        collector = new SimpleCollector("weixin_check_ready", "weixin_collect_data");
    }
    else if (link.contains("toutiao.com"))
    {
        collector = new ToutiaoCollector();
    }
    else if (link.contains("ixigua.com"))
    {
        collector = new SimpleCollector("xigua_check_ready", "xigua_collect_data");
    }

    return collector;
}
