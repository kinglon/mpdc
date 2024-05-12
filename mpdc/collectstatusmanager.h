#ifndef COLLECTSTATUSMANAGER_H
#define COLLECTSTATUSMANAGER_H

#include <QString>
#include <QVector>
#include "datamodel.h"

class CollectTaskItem
{
public:
    // 任务ID
    QString m_taskId;

    // 采集链接
    QString m_link;
};

class CollectStatusManager
{
public:
    CollectStatusManager();

public:
    static CollectStatusManager* getInstance();

public:
    void save();

    // 查询是否有任务待采集
    bool hasTaskCollecting() { return m_collectTasks.size() > 0; }

    // 启动新任务采集, planName 计划名字
    void startNewTasks(QString planName, const QVector<CollectTaskItem>& tasks);

    CollectTaskItem getNextTask();

    QString getPlanName() { return m_planName; }

    QVector<DataModel>& getCollectDatas() { return m_collectDatas; }

    void finishCurrentTask(const DataModel& dataModel);

    bool isFinish() { return m_nextTaskIndex >= m_collectTasks.size(); }

    void reset();

private:
    void load();

private:
    // 计划名字
    QString m_planName;

    // 采集任务列表
    QVector<CollectTaskItem> m_collectTasks;

    // 下一次采集任务索引
    int m_nextTaskIndex = 0;

    // 采集的结果
    QVector<DataModel> m_collectDatas;
};

#endif // COLLECTSTATUSMANAGER_H
