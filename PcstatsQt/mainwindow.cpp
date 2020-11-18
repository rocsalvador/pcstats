#include "mainwindow.h"
#include "ui_mainwindow.h"

pcstats stats;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->progressBar->setValue(stats.get_cpu_usage());
    ui->progressBar_2->setValue(stats.get_ram_usage());
    ui->progressBar_3->setValue(stats.get_avg_cpu_usage());
    ui->progressBar_4->setValue(stats.get_avg_ram_usage());
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButton_clicked()
{
   ui->progressBar->setValue(stats.get_cpu_usage());
   ui->progressBar_2->setValue(stats.get_ram_usage());
   ui->progressBar_3->setValue(stats.get_avg_cpu_usage());
   ui->progressBar_4->setValue(stats.get_avg_ram_usage());
}
