#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "browserwindow.h"
#include "debugdialog.h"
#include "collectorfactory.h"
#include "collectstatusmanager.h"
#include <QFileDialog>
#include <QMessageBox>
#include <QDesktopServices>
#include "xlsxdocument.h"
#include "xlsxchartsheet.h"
#include "xlsxcellrange.h"
#include "xlsxchart.h"
#include "xlsxrichstring.h"
#include "xlsxworkbook.h"
#include "Utility/ImPath.h"

using namespace QXlsx;

void RequestInterceptor::interceptRequest(QWebEngineUrlRequestInfo &info)
{
    if (info.resourceType() != QWebEngineUrlRequestInfo::ResourceTypeMainFrame)
    {
        return;
    }

    // 禁止找不到抖音视频，跳转到推荐页面
    if (info.requestUrl().toString().indexOf("recommend") != -1)
    {
        info.block(true);
        return;
    }
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    m_ctrlDShortcut = new QShortcut(QKeySequence("Ctrl+D"), this);
    connect(m_ctrlDShortcut, &QShortcut::activated, this, &MainWindow::onCtrlDShortcut);

    setWindowFlags(windowFlags() & ~Qt::WindowMaximizeButtonHint);
    setWindowFlag(Qt::MSWindowsFixedSizeDialogHint, true);

    ui->setupUi(this);
    initWindow();

    QWebEngineProfile::defaultProfile()->setRequestInterceptor(new RequestInterceptor(this));

    // 异步调用
    connect(this, &MainWindow::collectNextTask, this, &MainWindow::onCollectNextTask, Qt::QueuedConnection);
}

MainWindow::~MainWindow()
{
    delete ui;

    QWebEngineProfile::defaultProfile()->setRequestInterceptor(nullptr);
}

void MainWindow::initWindow()
{
    connect(ui->loginKuaiShouBtn, &QPushButton::clicked, [this]() {
        openLoginUrl("https://www.kuaishou.com/");
    });
    connect(ui->loginDouyinBtn, &QPushButton::clicked, [this]() {
        openLoginUrl("https://www.douyin.com/");
    });
    connect(ui->loginRedBookBtn, &QPushButton::clicked, [this]() {
        openLoginUrl("https://www.xiaohongshu.com/explore");
    });
    connect(ui->selectPathBtn, &QPushButton::clicked, this, &MainWindow::onSelectPathBtnClicked);
    connect(ui->collectBtn, &QPushButton::clicked, this, &MainWindow::onCollectBtnClicked);
    connect(ui->stopCollectBtn, &QPushButton::clicked, this, &MainWindow::onStopCollectBtnClicked);
    connect(ui->enableBrowserBtn, &QPushButton::clicked, []() {
        BrowserWindow::getInstance()->setEnabled(true);
    });

    updateCollectBtns();
}

void MainWindow::openLoginUrl(const QString& loginUrl)
{
    BrowserWindow::getInstance()->setEnabled(true);
    BrowserWindow::getInstance()->load(QUrl(loginUrl));
}

void MainWindow::updateCollectBtns()
{
    if (CollectStatusManager::getInstance()->hasTaskCollecting())
    {
        ui->collectBtn->setText(QString::fromWCharArray(L"继续采集"));
    }
    else
    {
        ui->collectBtn->setText(QString::fromWCharArray(L"开始采集"));
    }
    ui->collectBtn->setEnabled(!m_isCollecting);

    ui->stopCollectBtn->setEnabled(CollectStatusManager::getInstance()->hasTaskCollecting() && !m_isCollecting);
    ui->loginKuaiShouBtn->setEnabled(!m_isCollecting);
    ui->loginDouyinBtn->setEnabled(!m_isCollecting);
    ui->loginRedBookBtn->setEnabled(!m_isCollecting);
}

void MainWindow::onSelectPathBtnClicked(bool )
{
    // Create a QFileDialog object
    QFileDialog fileDialog;

    // Set the dialog's title
    fileDialog.setWindowTitle(QString::fromWCharArray(L"选择表格文件"));

    // Set the dialog's filters
    fileDialog.setNameFilters(QStringList() << "Excel files (*.xlsx *.xls)");

    // Open the dialog and get the selected files
    if (fileDialog.exec() == QDialog::Accepted)
    {
        QStringList selectedFiles = fileDialog.selectedFiles();
        if (selectedFiles.size() > 0)
        {
            ui->excelPathEdit->setText(selectedFiles[0]);
        }
    }
}

void MainWindow::onStopCollectBtnClicked(bool )
{
    m_isCollecting = false;
    updateCollectBtns();

    // 保存采集结果并打开保存目录
    if (!saveCollectResult())
    {
        showTip(QString::fromWCharArray(L"保存采集结果到表格失败"));
        return;
    }

    QString savePath = CollectorBase::getCollectResultSavePath(CollectStatusManager::getInstance()->getPlanName());
    QDesktopServices::openUrl(QUrl::fromLocalFile(savePath));

    CollectStatusManager::getInstance()->reset();
    updateCollectBtns();
}

void MainWindow::onCollectBtnClicked(bool )
{
    // 没有任务待采集，选择表格采集
    if (!CollectStatusManager::getInstance()->hasTaskCollecting())
    {
        QString excelFilePath = ui->excelPathEdit->text();
        if (excelFilePath.isEmpty())
        {
            showTip(QString::fromWCharArray(L"请先选择表格"));
            return;
        }

        // 获取表格中的任务列表
        QVector<CollectTaskItem> tasks;
        loadTaskList(excelFilePath, tasks);
        if (tasks.isEmpty())
        {
            showTip(QString::fromWCharArray(L"加载表格失败"));
            return;
        }

        QString planName = QFileInfo(excelFilePath).baseName();
        CollectStatusManager::getInstance()->startNewTasks(planName, tasks);
        QDir().rmdir(CollectorBase::getCollectResultSavePath(planName));
    }

    // 如果采集已经完成，就结束采集
    if (CollectStatusManager::getInstance()->isFinish())
    {
        onStopCollectBtnClicked();
        return;
    }

    m_isCollecting = true;
    updateCollectBtns();
    ui->logEdit->setText("");

    BrowserWindow::getInstance()->setEnabled(false);
    BrowserWindow::getInstance()->showMaximized();

    onCollectNextTask();
}

void MainWindow::onCtrlDShortcut()
{
    DebugDialog debugDlg(this);
    debugDlg.show();
    debugDlg.exec();
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    BrowserWindow::getInstance()->setCanClose();
    BrowserWindow::getInstance()->close();
    event->accept();
}

void MainWindow::addCollectLog(const QString& log)
{
    qInfo(log.toStdString().c_str());
    QDateTime currentDateTime = QDateTime::currentDateTime();
    QString currentTimeString = currentDateTime.toString("[MM-dd hh:mm:ss] ");
    QString line = currentTimeString + log;
    ui->logEdit->append(line);
}

void MainWindow::loadTaskList(const QString& excelFilePath, QVector<CollectTaskItem>& tasks)
{
    Document xlsx(excelFilePath);
    if (xlsx.load())
    {
        CellRange range = xlsx.dimension();
        for (int row=2; row <= range.lastRow(); row++)
        {
            Cell* cell1 = xlsx.cellAt(row, 1);
            Cell* cell2 = xlsx.cellAt(row, 2);
            if (cell1 && cell2)
            {
                CollectTaskItem taskItem;
                taskItem.m_taskId = cell1->readValue().toString();
                taskItem.m_link = cell2->readValue().toString();
                if (taskItem.m_taskId.isEmpty() || taskItem.m_link.isEmpty())
                {
                    break;
                }
                else
                {
                    tasks.push_back(taskItem);
                }
            }
        }
    }
}

bool MainWindow::saveCollectResult()
{
    // 拷贝默认采集结果输出表格到保存目录
    QString excelFileName = QString::fromWCharArray(L"采集结果.xlsx");
    QString srcExcelFilePath = QString::fromStdWString(CImPath::GetConfPath()) + excelFileName;
    QString destExcelFilePath = CollectorBase::getCollectResultSavePath(
                CollectStatusManager::getInstance()->getPlanName()) + excelFileName;
    ::DeleteFile(destExcelFilePath.toStdWString().c_str());
    if (!::CopyFile(srcExcelFilePath.toStdWString().c_str(), destExcelFilePath.toStdWString().c_str(), TRUE))
    {
        qCritical("failed to copy the result excel file");
        return false;
    }

    // 从第2行开始写
    Document xlsx(destExcelFilePath);
    if (!xlsx.load())
    {
        qCritical("failed to load the result excel file");
        return false;
    }

    auto& datas = CollectStatusManager::getInstance()->getCollectDatas();
    int row = 2;
    for (const auto& data : datas)
    {
        if (!data.m_nickName.isEmpty())
        {
            xlsx.write(row, 1, data.m_id);
            xlsx.write(row, 2, data.m_title);
            xlsx.write(row, 3, data.m_link);
            xlsx.write(row, 4, data.m_userId);
            xlsx.write(row, 5, data.m_nickName);
            xlsx.write(row, 6, data.m_fanCount);
            row++;
        }
    }

    if (!xlsx.save())
    {
        qCritical("failed to save the result excel file");
        return false;
    }

    return true;
}

void MainWindow::finishCurrentTask(const DataModel& dataModel)
{
    CollectStatusManager::getInstance()->finishCurrentTask(dataModel);
    if (CollectStatusManager::getInstance()->isFinish())
    {
        // 结束采集计划
        onStopCollectBtnClicked();
    }
    else
    {
        // 异步调用
        emit collectNextTask();
    }
}

void MainWindow::showTip(QString tip)
{
    QMessageBox msgBox;
    msgBox.setWindowTitle(QString::fromWCharArray(L"提示"));
    msgBox.setText(tip);
    msgBox.setStandardButtons(QMessageBox::Ok);
    msgBox.exec();
}

void MainWindow::onCollectNextTask()
{
    CollectTaskItem task = CollectStatusManager::getInstance()->getNextTask();
    if (task.m_link.isEmpty())
    {
        qCritical("failed to get the next task");
        return;
    }

    QString link = task.m_link;
    CollectorBase* collector = CollectorFactory::createCollector(link);
    if (collector == nullptr)
    {
        addCollectLog(QString::fromWCharArray(L"链接%1不支持：%2").arg(task.m_taskId, link));
        finishCurrentTask(DataModel());
        return;
    }

    collector->getDataModel().m_id = task.m_taskId;
    collector->getDataModel().m_link = link;
    collector->setPlanName(CollectStatusManager::getInstance()->getPlanName());
    connect(collector, &CollectorBase::runFinish, [collector, this](bool ok) {
        if (ok || collector->getCollectError() == COLLECT_ERROR_INVALID_LINK)
        {
            finishCurrentTask(collector->getDataModel());
        }
        else
        {            
            m_isCollecting = false;
            updateCollectBtns();
        }
        collector->deleteLater();
    });
    connect(collector, &CollectorBase::collectLog, [this](const QString& log) {
        addCollectLog(log);
    });
    collector->run();
}
