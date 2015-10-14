#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSettings>

namespace Ui
{
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();
    Ui::MainWindow *ui;
//    QSettings *settings;
    QString verzio;

private slots:
    void binom();
    double trafo(double pont, double Sp, double Se);
    bool MenTXT();
    void showAbout();
    void showHelp();
    void kiir(double pbal, double pjobb, double prev, double Sp, double Se);    
    void settingClear();
    void printIt();
};

#endif // MAINWINDOW_H
