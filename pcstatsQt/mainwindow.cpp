#include "mainwindow.h"
#include "ui_mainwindow.h"

pcstats stats;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ui->progressBar_2->setRange(0, stats.getTotalRam());
    ui->progressBar_4->setRange(0, stats.getTotalRam());

    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(timeout()));
    timer->setInterval(1000);
    timer->start();
}

void MainWindow::timeout() {
    double ramUsage = stats.getRamUsage();
    double cpuUsage = stats.getCpuUsage();
    double cpuFreq = stats.getCpuFreq();

    ui->progressBar->setValue(cpuUsage);
    ui->progressBar_2->setValue(ramUsage);

    QString usedRam = QString::number(ramUsage);
    usedRam += "/";
    usedRam += QString::number(stats.getTotalRam());
    usedRam += " GB";
    ui->label_5->setText(usedRam);

    QString cpuFreqS = QString::number(cpuFreq);
    cpuFreqS += " GHz";
    ui->label_6->setText(cpuFreqS);

    ui->progressBar_3->setValue(stats.getAvgCpuUsage());
    ui->progressBar_4->setValue(stats.getAvgRamUsage());

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButton_clicked()
{
    stats.resetSavedStats();
}

void MainWindow::on_spinBox_valueChanged(const QString &refreshRate)
{
    timer->setInterval(refreshRate.toInt()*1000);
}
