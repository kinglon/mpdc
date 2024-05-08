#pragma once

#include <QString>
#include <QVector>

class CSettingManager
{
protected:
	CSettingManager();

public:
	static CSettingManager* GetInstance();

public:
    void Save();

private:
	void Load();

public:
    int m_nLogLevel = 2;  // debug
};
