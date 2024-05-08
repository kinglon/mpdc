#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "browserwindow.h"
#include "debugdialog.h"

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
