#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowTitle("Compress");
    ui->tabCompressPasswordText->setEchoMode(QLineEdit::PasswordEchoOnEdit);
    ui->tabUncompressPasswordText->setEchoMode(QLineEdit::PasswordEchoOnEdit);

    memset(&iNodeHead, 0, sizeof(iNode));
    memset(&linkNodeHead, 0, sizeof(linkNodeHead));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_tabTarChooseSrc_clicked()
{
    QString tarSrcPath = QDir::toNativeSeparators(QFileDialog::getExistingDirectory(this,tr("Choose Src Directory"),QDir::currentPath()));
    ui->tabTarSrcText->setText(tarSrcPath);
}

void MainWindow::on_tabTarChooseDest_clicked()
{
    QString tarDestPath = QDir::toNativeSeparators(QFileDialog::getExistingDirectory(this,tr("Choose Dest Directory"),QDir::currentPath()));
    ui->tabTarDestText->setText(tarDestPath);
}

void MainWindow::on_tabTarStart_clicked()
{
    QString tarSrcPath = ui->tabTarSrcText->text();
    QFileInfo srcFileInfo(tarSrcPath);
    QString tarDestPath = ui->tabTarDestText->text()+"/"+srcFileInfo.fileName()+".tar";
    if (tarSrcPath.length() == 0)
    {
        QMessageBox::warning(this,"Warning!","Please choose src directory");
        return;
    }
    if (tarDestPath.length() == 0)
    {
        QMessageBox::warning(this,"Warning!","Please choose dest directory");
        return;
    }
    std::string s_tarDestPath = tarDestPath.toStdString();
    const char *c_tarDestPath = s_tarDestPath.c_str();
    FILE *fout = fopen(c_tarDestPath,"wb");
    if (!fout)
    {
        QMessageBox::warning(this,"Warning!","fopen error");
        return;
    }
    std::string s_tarSrcPath = tarSrcPath.toStdString();
    const char *c_tarSrcPath = s_tarSrcPath.c_str();
    if (tar((char *)c_tarSrcPath,fout)) QMessageBox::warning(this,"Warning!","Tar error");
    else QMessageBox::about(this,"Information!","Success");

    Record* lastRecord = (Record*)mallocAndReset(512, 0);
    for (int i = 0; i < 2; i++) printOneBlock(lastRecord, fout);
    free(lastRecord);

    fclose(fout);

    freeINode();
}

void MainWindow::on_tabUntarChooseSrc_clicked()
{
    QString untarSrcPath = QDir::toNativeSeparators(QFileDialog::getOpenFileName(this,tr("Choose Src File"),QDir::currentPath()));
    ui->tabUntarSrcText->setText(untarSrcPath);
}

void MainWindow::on_tabUntarChooseDest_clicked()
{
    QString untarDestPath = QDir::toNativeSeparators(QFileDialog::getExistingDirectory(this,tr("Choose Src Directory"),QDir::currentPath()));
    ui->tabUntarDestText->setText(untarDestPath);
}

void MainWindow::on_tabUntarStart_clicked()
{
    QString untarSrcPath = ui->tabUntarSrcText->text();
    QString untarDestPath = ui->tabUntarDestText->text();
    if (untarSrcPath.length() == 0)
    {
        QMessageBox::warning(this,"Warning!","Please choose src file");
        return;
    }
    if (untarDestPath.length() == 0)
    {
        QMessageBox::warning(this,"Warning!","Please choose dest directory");
        return;
    }
    std::string s_untarSrcPath = untarSrcPath.toStdString();
    const char *c_untarSrcPath = s_untarSrcPath.c_str();
    FILE *fin = fopen(c_untarSrcPath,"rb");
    if (!fin)
    {
        QMessageBox::warning(this,"Warning!","fopen error");
        return;
    }
    std::string s_untarDestPath = untarDestPath.toStdString();
    const char *c_untarDestPath = s_untarDestPath.c_str();
    if (untar((char *)c_untarDestPath,fin)) QMessageBox::warning(this,"Warning!","Untar error");
    else QMessageBox::about(this,"Information!","Success");
    fclose(fin);
}

void MainWindow::on_tabCompressChooseSrc_clicked()
{
    QString compressSrcPath = QDir::toNativeSeparators(QFileDialog::getOpenFileName(this,tr("Choose Src File"),QDir::currentPath()));
    ui->tabCompressSrcText->setText(compressSrcPath);
}

void MainWindow::on_tabCompressChooseDest_clicked()
{
    QString compressDestPath = QDir::toNativeSeparators(QFileDialog::getExistingDirectory(this,tr("Choose Src Directory"),QDir::currentPath()));
    ui->tabCompressDestText->setText(compressDestPath);
}

void MainWindow::on_tabCompressStart_clicked()
{
    QString compressSrcPath = ui->tabCompressSrcText->text();
    QFileInfo srcFileInfo(compressSrcPath);
    QString compressDestPath = ui->tabCompressDestText->text()+"/"+srcFileInfo.fileName()+".hf";
    if (compressSrcPath.length() == 0)
    {
        QMessageBox::warning(this,"Warning!","Please choose src file");
        return;
    }
    if (compressDestPath.length() == 0)
    {
        QMessageBox::warning(this,"Warning!","Please choose dest directory");
        return;
    }
    std::string s_compressSrcPath = compressSrcPath.toStdString();
    const char *c_compressSrcPath = s_compressSrcPath.c_str();
    FILE *fin = fopen(c_compressSrcPath,"rb");
    if (!fin)
    {
        QMessageBox::warning(this,"Warning!","fopen error");
        return;
    }
    std::string s_compressDestPath = compressDestPath.toStdString();
    const char *c_comrpessDestPath = s_compressDestPath.c_str();
    FILE *fout = fopen(c_comrpessDestPath,"wb");
    if (!fout)
    {
        QMessageBox::warning(this,"Warning!","fopen error");
        return;
    }
    countFileFrequency((char *)c_compressSrcPath);
    if (compress(fin,fout)) QMessageBox::warning(this,"Warning!","Compress error");
    else QMessageBox::about(this,"Information!","Success");
    fclose(fin);
    fclose(fout);
    if (ui->tabCompressPasswordCheckBox->isChecked())
    {
        QString encryptDestPath = compressDestPath + ".encrypt";
        std::string s_encryptDestPath = encryptDestPath.toStdString();
        const char*c_encryptDestPath = s_encryptDestPath.c_str();
        QString password = ui->tabCompressPasswordText->text();
        if (password.length() == 0)
        {
            QMessageBox::warning(this,"Warning!","Please input password");
        }
        password = password+"0000000000000000";
        std::string s_password = password.toStdString();
        const char*c_password = s_password.c_str();
        encryptFile((char *)c_password,(char *)c_comrpessDestPath,(char *)c_encryptDestPath);
    }
}

void MainWindow::on_tabUncompressChooseSrc_clicked()
{
    QString uncompressSrcPath = QDir::toNativeSeparators(QFileDialog::getOpenFileName(this,tr("Choose Src File"),QDir::currentPath()));
    ui->tabUncompressSrcText->setText(uncompressSrcPath);
}

void MainWindow::on_tabUncompressChooseDest_clicked()
{
    QString uncompressDestPath = QDir::toNativeSeparators(QFileDialog::getExistingDirectory(this,tr("Choose Src File"),QDir::currentPath()));
    ui->tabUncompressDestText->setText(uncompressDestPath);
}

void MainWindow::on_tabUncompressStart_clicked()
{
    QString uncompressSrcPath = ui->tabUncompressSrcText->text();
    if (uncompressSrcPath.length() == 0)
    {
        QMessageBox::warning(this,"Warning!","Please choose src file");
        return;
    }
    std::string s_uncompressSrcPath = uncompressSrcPath.toStdString();
    const char *c_uncompressSrcPath = s_uncompressSrcPath.c_str();
    if (ui->tabUncompressPasswordCheckBox->isChecked())
    {
        QFileInfo srcFileInfo(uncompressSrcPath);
        if (srcFileInfo.suffix()=="encrypt")
        {
            QString decryptDestPath = srcFileInfo.path()+"/"+srcFileInfo.completeBaseName();
            std::string s_decryptDestPath = decryptDestPath.toStdString();
            const char *c_decryptDestPath = s_decryptDestPath.c_str();
            QString password = ui->tabUncompressPasswordText->text();
            if (password.length() == 0)
            {
                QMessageBox::warning(this,"Warning!","Please input password");
                return;
            }
            password = password+"0000000000000000";
            std::string s_password = password.toStdString();
            const char*c_password = s_password.c_str();
            decryptFile((char *)c_password,(char *)c_uncompressSrcPath,(char *)c_decryptDestPath);
            QFileInfo decryptFileInfo(decryptDestPath);
            if (decryptFileInfo.suffix() == "hf")
            {
                FILE *fin = fopen(c_decryptDestPath,"rb");
                if (!fin)
                {
                    QMessageBox::warning(this,"Warning!","fopen error");
                    return;
                }
                QString uncompressDestPath = decryptFileInfo.path()+"/"+decryptFileInfo.completeBaseName();
                std::string s_uncompressDestPath = uncompressDestPath.toStdString();
                const char *c_uncomrpessDestPath = s_uncompressDestPath.c_str();
                FILE *fout = fopen(c_uncomrpessDestPath,"wb");
                if (!fout)
                {
                    QMessageBox::warning(this,"Warning!","fopen error");
                    return;
                }
                if (uncompress(fin,fout)) QMessageBox::warning(this,"Warning!","Uncompress error");
                else QMessageBox::about(this,"Information!","Success");
                fclose(fin);
                fclose(fout);
                return;
            }
            else
            {
                QMessageBox::warning(this,"Warning!","Not hf file");
                return;
            }
        }
        else
        {
            QMessageBox::warning(this,"Warning!","Please choose encrypt file");
            return;
        }
    }
    QFileInfo uncompressFileInfo(uncompressSrcPath);
    if (uncompressFileInfo.suffix() != "hf")
    {
        QMessageBox::warning(this,"Warning!","Not hf file");
        return;
    }
    FILE *fin = fopen(c_uncompressSrcPath,"rb");
    if (!fin)
    {
        QMessageBox::warning(this,"Warning!","fopen error");
        return;
    }
    QString uncompressDestPath = ui->tabUncompressDestText->text()+"/"+uncompressFileInfo.completeBaseName();
    if (uncompressDestPath.length() == 0)
    {
        QMessageBox::warning(this,"Warning!","Please choose dest directory");
        return;
    }
    std::string s_uncompressDestPath = uncompressDestPath.toStdString();
    const char *c_uncomrpessDestPath = s_uncompressDestPath.c_str();
    FILE *fout = fopen(c_uncomrpessDestPath,"wb");
    if (!fout)
    {
        QMessageBox::warning(this,"Warning!","fopen error");
        return;
    }
    if (uncompress(fin,fout)) QMessageBox::warning(this,"Warning!","Uncompress error");
    else QMessageBox::about(this,"Information!","Success");
    fclose(fin);
    fclose(fout);
}

void MainWindow::on_tabVerifyChooseSrc_clicked()
{
    QString verifySrcPath = QDir::toNativeSeparators(QFileDialog::getOpenFileName(this,tr("Choose Src File"),QDir::currentPath()));
    ui->tabVerifySrcText->setText(verifySrcPath);
}

void MainWindow::on_tabVerifyChooseDest_clicked()
{
    QString verifyDestPath = QDir::toNativeSeparators(QFileDialog::getExistingDirectory(this,tr("Choose Src File"),QDir::currentPath()));
    ui->tabVerifyDestText->setText(verifyDestPath);
}

void MainWindow::on_tabVerifyStart_clicked()
{
    QString srcPath = ui->tabVerifySrcText->text();
    if (srcPath.length() == 0)
    {
        QMessageBox::warning(this,"Warning!","Please choose src file");
        return;
    }
    QTemporaryFile tempFile;
    if (!tempFile.open())
    {
        QMessageBox::warning(this,"Warning!","Tempfile error");
        return;
    }
    QString tempPath = tempFile.fileName();
    std::string s_tempPath = tempPath.toStdString();
    const char *c_tempPath = s_tempPath.c_str();
    FILE *fout = fopen(c_tempPath,"wb");
    if (!fout)
    {
        QMessageBox::warning(this,"Warning!","fopen error");
        return;
    }
    QString tarSrcPath = ui->tabVerifyDestText->text();
    if (tarSrcPath.length() == 0)
    {
        QMessageBox::warning(this,"Warning!","Please choose dest directory");
        return;
    }
    std::string s_tarSrcPath = tarSrcPath.toStdString();
    const char *c_tarSrcPath = s_tarSrcPath.c_str();

    if (tar((char *)c_tarSrcPath,fout)) QMessageBox::warning(this,"Warning!","Tar error");

    Record* lastRecord = (Record*)mallocAndReset(512, 0);
    for (int i = 0; i < 2; i++) printOneBlock(lastRecord, fout);
    free(lastRecord);

    fclose(fout);

    freeINode();

    QFile srcFile(srcPath);
    QFile destFile(tempPath);
    srcFile.open(QIODevice::ReadOnly);
    destFile.open(QIODevice::ReadOnly);
    QByteArray srcTemp = srcFile.read(1024);
    QByteArray destTemp = destFile.read(1024);
    while (srcTemp.length() == 1024 && destTemp.length() == 1024)
    {
        for (int i = 0;i<1024;i++)
        {
            if (srcTemp[i] == destTemp[i]) continue;
            else
            {
                QMessageBox::warning(this,"Warning!","Verify failed");
                return;
            }
        }
        srcTemp = srcFile.read(1024);
        destTemp = destFile.read(1024);
    }
    if (srcTemp.size()!=0 || destTemp.size()!=0)
    {
        if (srcTemp.size() == destTemp.size())
        {
            for (int i=0;i<srcTemp.size();i++)
            {
                if (srcTemp[i] == destTemp[i]) continue;
                else
                {
                    QMessageBox::warning(this,"Warning!","Verify failed");
                    return;
                }
            }
        }
        else
        {
            QMessageBox::warning(this,"Warning!","Verify failed");
            return;
        }
    }
    QMessageBox::about(this,"Information!","Verify Succeed");
    srcFile.close();
    destFile.close();
}
