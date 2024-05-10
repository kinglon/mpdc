#ifndef DATAMODEL_H
#define DATAMODEL_H

#include <QString>

class DataModel
{
public:
    // ID
    QString m_id;

    // 链接
    QString m_link;

    // 标题
    QString m_title;

    // 用户ID
    QString m_userId;

    // 昵称
    QString m_nickName;

    // 粉丝数
    QString m_fanCount;
};

#endif // DATAMODEL_H
