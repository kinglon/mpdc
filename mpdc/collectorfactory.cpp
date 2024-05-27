#include "collectorfactory.h"
#include "douyincollector.h"
#include "kuaishoucollector.h"
#include "simplecollector.h"
#include "twotimecollector.h"

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
        collector = new TwoTimeCollector("toutiao_check_ready", "toutiao_collect_data", "toutiao_collect_data2");
    }
    else if (link.contains("ixigua.com"))
    {
        collector = new SimpleCollector("xigua_check_ready", "xigua_collect_data");
    }
    else if (link.contains("xiaohongshu.com"))
    {
        collector = new TwoTimeCollector("xiaohongshu_check_ready", "xiaohongshu_collect_data", "xiaohongshu_collect_data2");
    }

    return collector;
}
