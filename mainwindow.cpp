#include "mainwindow.h"
#include "./ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    connect(ui->buttonLoad, SIGNAL(clicked()), this, SLOT(loadData()));
    connect(ui->buttonSave, SIGNAL(clicked()), this, SLOT(saveData()));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::loadData() {
    QString filePath = ui->lineEditFilePath->text();
    QFile file(filePath);

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox::warning(this, "Ошибка", "Не удалось открыть файл: " + filePath);
        return;
    }

    QTextStream in(&file);
    ui->tableWidgetStudents->setRowCount(0);

    while (!in.atEnd()) {
        QString line = in.readLine();
        QStringList fields = line.split(",");

        int row = ui->tableWidgetStudents->rowCount();
        ui->tableWidgetStudents->insertRow(row);

        for (int i = 0; i < fields.size(); ++i) {
            QTableWidgetItem *item = new QTableWidgetItem(fields.at(i));
            ui->tableWidgetStudents->setItem(row, i, item);
        }
    }

    file.close();
}

void MainWindow::saveData() {
    QString filePath = ui->lineEditFilePath->text();
    QFile file(filePath);

    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::warning(this, "Ошибка", "Не удалось сохранить файл: " + filePath);
        return;
    }

    QTextStream out(&file);

    int rowCount = ui->tableWidgetStudents->rowCount();
    int columnCount = ui->tableWidgetStudents->columnCount();

    for (int i = 0; i < rowCount; ++i) {
        QStringList fields;
        for (int j = 0; j < columnCount; ++j) {
            QTableWidgetItem *item = ui->tableWidgetStudents->item(i, j);
            fields << item->text();
        }
        out << fields.join(",") << "\n";
    }

    file.close();
}
