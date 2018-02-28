#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QDebug>
#include <QDir>
#include <QProcess>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , mFileWatcher(new QFileSystemWatcher({"."}, this))
    , mChangeSettleTimer(new QTimer(this))
{
    ui->setupUi(this);
    mChangeSettleTimer->setSingleShot(true);
    QObject::connect(mChangeSettleTimer, &QTimer::timeout, [this](){
        updateFiles();
    });
    QObject::connect(mFileWatcher, &QFileSystemWatcher::directoryChanged, [this](const QString &){
        mChangeSettleTimer->start(500);
    });
    updateFiles();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::updateFiles()
{
    QDir dir;
    dir.setFilter(QDir::Files);
    for (auto &fileInfo : dir.entryInfoList({"*.cpp", "*.c", "*.cc", "*.h", "*.hh", "*.hxx"})) {
        //qDebug() << fileInfo.absoluteFilePath();
        QFile sourceFile(fileInfo.absoluteFilePath());
        if (!sourceFile.open(QIODevice::ReadOnly)) {
            // TODO: throw
        }
        if (sourceFile.readAll().indexOf("int main(") == -1) continue;
        auto tabIt = mTabs.find(fileInfo.absoluteFilePath());
        if (tabIt == mTabs.end()) {
            QWebEngineView *view = new QWebEngineView(this);
            mTabs.emplace(fileInfo.absoluteFilePath(),
                    Tab(view, fileInfo.baseName(), fileInfo.absoluteFilePath()));
            ui->tabWidget->addTab(view, QStringLiteral("[&%1] %2").arg(mTabs.size()).arg(fileInfo.baseName()));
        }
        //mTabs.at(fileInfo.absoluteFilePath()).lastModified =
    }
    // generate Makefile
    {
        QFile makefile("build/Makefile");
        if (makefile.open(QIODevice::WriteOnly)) {
            // TODO: throw
        }
        makefile.write("all: ");
        for (const auto &tab : mTabs) {
            makefile.write((tab.second.baseName+' ').toUtf8());
        }
        makefile.write("\n");
        for (const auto &tab : mTabs) {
            makefile.write((tab.second.baseName+": "+tab.second.source+"\n").toUtf8());
            makefile.write("\tg++ -Wall -std=c++14 $< -o $@\n\n");
        }
    }
    {
        QProcess makeProcess;
        makeProcess.setProcessChannelMode(QProcess::MergedChannels);
        makeProcess.setWorkingDirectory("./build");
        makeProcess.start("make");
        if (!makeProcess.waitForFinished()) {
            // failed
        }
        qDebug() << makeProcess.readAll();
    }
}
