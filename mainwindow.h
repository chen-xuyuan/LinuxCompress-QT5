#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "QFileDialog"
#include "QDir"
#include "compress.h"
#include "stdio.h"
#include "QMessageBox"
#include "aes128.h"
#include "QCryptographicHash"
#include "QTemporaryFile"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_tabTarChooseSrc_clicked();

    void on_tabTarChooseDest_clicked();

    void on_tabTarStart_clicked();

    void on_tabUntarChooseSrc_clicked();

    void on_tabUntarChooseDest_clicked();

    void on_tabUntarStart_clicked();

    void on_tabCompressChooseSrc_clicked();

    void on_tabCompressChooseDest_clicked();

    void on_tabCompressStart_clicked();

    void on_tabUncompressChooseSrc_clicked();

    void on_tabUncompressChooseDest_clicked();

    void on_tabUncompressStart_clicked();

    void on_tabVerifyChooseSrc_clicked();

    void on_tabVerifyChooseDest_clicked();

    void on_tabVerifyStart_clicked();

private:
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
