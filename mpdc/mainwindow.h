#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QShortcut>
#include <QWebEngineUrlRequestInterceptor>
#include <QWebEngineProfile>
#include "collectstatusmanager.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class RequestInterceptor : public QWebEngineUrlRequestInterceptor
{
    Q_OBJECT

public:
    RequestInterceptor(QObject *parent = Q_NULLPTR) : QWebEngineUrlRequestInterceptor(parent)
    {
        //
    }

    void interceptRequest(QWebEngineUrlRequestInfo &info);
};

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    void initWindow();

    // 更新采集按钮的状态
    void updateCollectBtns();

signals:
    void collectNextTask();

private slots:
    void onSelectPathBtnClicked(bool checked);

    void onStopCollectBtnClicked(bool checked=false);

    void onCollectBtnClicked(bool checked);

    void onCtrlDShortcut();

    void onCollectNextTask();

protected:
    void closeEvent(QCloseEvent *event) override;

private:
    void addCollectLog(const QString& log);

    void loadTaskList(const QString& excelFilePath, QVector<CollectTaskItem>& tasks);

    bool saveCollectResult();

    void finishCurrentTask(const DataModel& dataModel);

    void showTip(QString tip);

private:
    Ui::MainWindow* ui;

    // CTRL+D快捷键
    QShortcut* m_ctrlDShortcut = nullptr;

    // 标志采集器是否正在运行
    bool m_isCollecting = false;
};
#endif // MAINWINDOW_H
