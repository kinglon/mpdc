#include "collectstatusmanager.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QFile>
#include <QFileInfo>
#include "Utility/ImPath.h"

CollectStatusManager::CollectStatusManager()
{
    load();
}

CollectStatusManager* CollectStatusManager::getInstance()
{
    static CollectStatusManager* instance = new CollectStatusManager();
    return instance;
}

void CollectStatusManager::save()
{
    QJsonObject root;
    root["plan_name"] = m_planName;
    root["next_task_index"] = m_nextTaskIndex;

    QJsonArray tasksJson;
    for (const auto& task : m_collectTasks)
    {
        QJsonObject taskJson;
        taskJson["id"] = task.m_taskId;
        taskJson["link"] = task.m_link;
        tasksJson.append(taskJson);
    }
    root["tasks"] = tasksJson;

    QJsonArray datasJson;
    for (const auto& data : m_collectDatas)
    {
        QJsonObject dataJson;
        dataJson["id"] = data.m_id;
        dataJson["link"] = data.m_link;
        dataJson["title"] = data.m_title;
        dataJson["user_id"] = data.m_userId;
        dataJson["nick_name"] = data.m_nickName;
        dataJson["fan_count"] = data.m_fanCount;
        datasJson.append(dataJson);
    }
    root["datas"] = datasJson;

    QJsonDocument jsonDocument(root);
    QByteArray jsonData = jsonDocument.toJson(QJsonDocument::Indented);
    std::wstring strConfFilePath = CImPath::GetConfPath() + L"collect_status.json";
    QFile file(QString::fromStdWString(strConfFilePath));
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        qCritical("failed to save the collecting status");
        return;
    }
    file.write(jsonData);
    file.close();
}

void CollectStatusManager::startNewTasks(QString planName, const QVector<CollectTaskItem>& tasks)
{
    reset();
    m_planName = planName;
    m_collectTasks = tasks;
    save();
}

CollectTaskItem CollectStatusManager::getNextTask()
{
    if (m_nextTaskIndex >= m_collectTasks.size())
    {
        return CollectTaskItem();
    }

    return m_collectTasks[m_nextTaskIndex];
}

void CollectStatusManager::finishCurrentTask(const DataModel& dataModel)
{
    m_collectDatas.push_back(dataModel);
    m_nextTaskIndex++;
    save();
}

void CollectStatusManager::reset()
{
    m_planName = "";
    m_nextTaskIndex = 0;
    m_collectTasks.clear();
    m_collectDatas.clear();
    save();
}

void CollectStatusManager::load()
{
    std::wstring strConfFilePath = CImPath::GetConfPath() + L"collect_status.json";
    if (!QFileInfo(QString::fromStdWString(strConfFilePath)).exists())
    {
        return;
    }

    QFile file(QString::fromStdWString(strConfFilePath));
    if (!file.open(QIODevice::ReadOnly))
    {
        qCritical("failed to load the collecting status configure file");
        return;
    }
    QByteArray jsonData = file.readAll();
    file.close();

    QJsonDocument jsonDocument = QJsonDocument::fromJson(jsonData);
    QJsonObject root = jsonDocument.object();
    m_planName = root["plan_name"].toString();
    m_nextTaskIndex = root["next_task_index"].toInt();

    m_collectTasks.clear();
    QJsonArray tasksJson = root["tasks"].toArray();
    for (int i=0; i < tasksJson.size(); i++)
    {
        CollectTaskItem task;
        task.m_taskId = tasksJson.at(i)["id"].toString();
        task.m_link = tasksJson.at(i)["link"].toString();
        m_collectTasks.push_back(task);
    }

    m_collectDatas.clear();
    QJsonArray datasJson = root["datas"].toArray();
    for (int i=0; i < datasJson.size(); i++)
    {
        auto dataJson = datasJson.at(i);
        DataModel data;
        data.m_id = dataJson["id"].toString();
        data.m_link = dataJson["link"].toString();
        data.m_title = dataJson["title"].toString();
        data.m_userId = dataJson["user_id"].toString();
        data.m_nickName = dataJson["nick_name"].toString();
        data.m_fanCount = dataJson["fan_count"].toString();
        m_collectDatas.append(data);
    }
}
