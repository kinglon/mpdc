#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "browserwindow.h"
#include "debugdialog.h"
#include "collectorfactory.h"

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
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::initWindow()
{
    connect(ui->collectBtn, &QPushButton::clicked, this, &MainWindow::onCollectBtnClicked);
}

void MainWindow::onCollectBtnClicked(bool checked)
{
    (void)checked;

    BrowserWindow::getInstance()->showMaximized();

    QString link = "https://www.douyin.com/video/7366078938549914917";
    CollectorBase* collector = CollectorFactory::createCollector(link);
    if (collector == nullptr)
    {
        return;
    }
    collector->getDataModel().m_id = "1";
    collector->getDataModel().m_link = link;
    collector->setDataSaveFolder("test");
    connect(collector, &CollectorBase::runFinish, [collector, this](bool ok) {
        if (ok)
        {
            QString log = QString("title: %1, id: %2, nick: %3, fan: %4").arg(
                        collector->getDataModel().m_title,
                        collector->getDataModel().m_userId,
                        collector->getDataModel().m_nickName,
                        collector->getDataModel().m_fanCount);
            AddCollectLog(log);
        }
        collector->deleteLater();
    });
    connect(collector, &CollectorBase::collectLog, [this](const QString& log) {
        AddCollectLog(log);
    });
    collector->run();
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

void MainWindow::AddCollectLog(const QString& log)
{
    QDateTime currentDateTime = QDateTime::currentDateTime();
    QString currentTimeString = currentDateTime.toString("[MM-dd hh:mm:ss] ");
    QString line = currentTimeString + log;
    ui->logEdit->append(line);
}
