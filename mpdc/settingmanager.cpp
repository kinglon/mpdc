﻿#include "settingmanager.h"
#include <QFile>
#include "Utility/ImPath.h"
#include "Utility/ImCharset.h"
#include "Utility/LogMacro.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

CSettingManager::CSettingManager()
{
    Load();
}

CSettingManager* CSettingManager::GetInstance()
{
	static CSettingManager* pInstance = new CSettingManager();
	return pInstance;
}

void CSettingManager::Load()
{
    std::wstring strConfFilePath = CImPath::GetConfPath() + L"configs.json";    
    QFile file(QString::fromStdWString(strConfFilePath));
    if (!file.open(QIODevice::ReadOnly))
    {
        LOG_ERROR(L"failed to open the basic configure file : %s", strConfFilePath.c_str());
        return;
    }
    QByteArray jsonData = file.readAll();
    file.close();

    QJsonDocument jsonDocument = QJsonDocument::fromJson(jsonData);
    QJsonObject root = jsonDocument.object();
    m_nLogLevel = root["log_level"].toInt();
    m_enableWebviewLog = root["enable_webview_log"].toInt();
    m_cacheJsCode = root["cache_jscode"].toInt();
    m_browserWidth = root["browser_width"].toInt();
    m_browserHeight = root["browser_height"].toInt();
}
