#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QWebEngineView>
#include <QFileSystemWatcher>
#include <QTimer>

#include <map>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:

    void updateFiles();

    struct Tab {
        Tab(QWebEngineView *content, const QString baseName, const QString &source)
            : content(content)
            , baseName(baseName)
            , source(source)
            , binary("./build/"+baseName)
        {}

        QWebEngineView *content;
        QString baseName;
        QString source;
        QString binary;
        QDateTime lastModified;
    };

    std::map<QString, Tab> mTabs;

    Ui::MainWindow *ui;
    QFileSystemWatcher *mFileWatcher;
    QTimer *mChangeSettleTimer;
};

#endif // MAINWINDOW_H
