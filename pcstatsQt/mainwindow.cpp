#include "mainwindow.h"
#include "ui_mainwindow.h"

pcstats stats;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ui->progressBar_2->setRange(0, stats.getTotalRam());

    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(timeout()));
    timer->setInterval(1000);
    timer->start();
}

void MainWindow::timeout() {
    ui->progressBar->setValue(stats.getCpuUsage());
    ui->progressBar_2->setValue(stats.getRamUsage());

    QString usedRam = QString::number(stats.getRamUsage());
    usedRam += "/";
    usedRam += QString::number(stats.getTotalRam());
    usedRam += " GB";
    ui->label_5->setText(usedRam);

    QString cpuFreqS = QString::number(stats.getCpuFreq());
    cpuFreqS += " GHz";
    ui->label_6->setText(cpuFreqS);

    ui->label->setText("Max. Cpu usage: " + QString::number(stats.getMaxCpuUsage()) + " %");
    ui->label_2->setText("Max. Cpu freq.: " + QString::number(stats.getMaxCpuFreq()) + " Hz");
    ui->label_7->setText("Max. Ram usage: " + QString::number(stats.getMaxRamUsage()) + " GB");

    ui->label_8->setText("Avg. Cpu usage: " + QString::number(stats.getAvgCpuUsage()) + " %");
    ui->label_9->setText("Avg. Cpu freq.: " + QString::number(stats.getAvgCpuFreq()) + " Hz");
    ui->label_10->setText("Avg. Ram usage: " + QString::number(stats.getAvgRamUsage()) + " GB");
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
