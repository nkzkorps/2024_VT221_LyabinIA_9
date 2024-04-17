#include "mainwindow.h"
#include "./ui_mainwindow.h"

template <typename T>
class smart_ptr {
    T *obj;
public:
    smart_ptr(T *obj) : obj(obj) {}
    ~smart_ptr() { delete obj; }
    T* operator->() { return obj; }
    T& operator*() { return *obj; }
    T* get() const { return obj; }  // Добавлен метод get()
};

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    connect(ui->buttonLoad, SIGNAL(clicked()), this, SLOT(loadData()));
    connect(ui->buttonSave, SIGNAL(clicked()), this, SLOT(saveData()));
    connect(ui->buttonAddStudent, SIGNAL(clicked()), this, SLOT(addStudent()));
    connect(ui->buttonRemoveStudent, SIGNAL(clicked()), this, SLOT(removeStudent()));
    connect(ui->buttonAddSubject, SIGNAL(clicked()), this, SLOT(addSubject()));
    connect(ui->buttonRemoveSubject, SIGNAL(clicked()), this, SLOT(removeSubject()));
    ui->tableWidgetStudents->horizontalHeader()->setSectionsClickable(true);
    ui->tableWidgetStudents->horizontalHeader()->setSectionsMovable(true);
    ui->tableWidgetStudents->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tableWidgetStudents->setHorizontalHeaderLabels(QStringList() << "Студент");

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
    ui->tableWidgetStudents->setColumnCount(0); // Обнуляем столбцы для перенастройки

    int maxColumns = 0; // Для отслеживания максимального числа столбцов

    while (!in.atEnd()) {
        QString line = in.readLine();
        QStringList fields = line.split(",");

        if (fields.size() > maxColumns) {
            // Обновляем количество столбцов, если текущая строка имеет больше столбцов
            maxColumns = fields.size();
            ui->tableWidgetStudents->setColumnCount(maxColumns);
            ui->tableWidgetStudents->setHorizontalHeaderItem(0, new QTableWidgetItem("Студент")); // Первый столбец для студентов
            for (int i = 1; i < maxColumns; ++i) {
                ui->tableWidgetStudents->setHorizontalHeaderItem(i, new QTableWidgetItem(QString("Занятие %1").arg(i)));
            }
        }

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
    smart_ptr<QFile> file(new QFile(filePath));  // Используем smart_ptr для управления QFile

    if (!file->open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::warning(this, "Ошибка", "Не удалось сохранить файл: " + filePath);
        return;
    }

    QTextStream out(file.get());  // Теперь корректно используем .get() для доступа к QFile
    int rowCount = ui->tableWidgetStudents->rowCount();
    int columnCount = ui->tableWidgetStudents->columnCount();

    for (int i = 0; i < rowCount; ++i) {
        QStringList fields;
        for (int j = 0; j < columnCount; ++j) {
            QTableWidgetItem *item = ui->tableWidgetStudents->item(i, j);
            if (item) {
                fields << item->text();  // Сохраняем текст, даже если он пустой
            } else {
                fields << "";  // Добавляем пустую строку, если ячейка не существует
            }
        }
        out << fields.join(",") << "\n";
    }

    file->close();  // Закрываем файл
    QMessageBox::information(this, "Успешно", "Данные успешно сохранены.");
}

void MainWindow::addStudent() {
    int row = ui->tableWidgetStudents->rowCount();
    ui->tableWidgetStudents->insertRow(row);
}

void MainWindow::removeStudent() {
    int row = ui->tableWidgetStudents->currentRow();
    if (row >= 0) {
        ui->tableWidgetStudents->removeRow(row);
    }
}

void MainWindow::addSubject() {
    int column = ui->tableWidgetStudents->columnCount();
    ui->tableWidgetStudents->insertColumn(column);

    // Получаем следующий номер для нового занятия
    int nextSubjectNumber = column; // Поскольку номерация столбцов начинается с 0
    QString headerTitle = QString("%1").arg(nextSubjectNumber);

    // Устанавливаем заголовок с возможностью его редактирования
    QTableWidgetItem *headerItem = new QTableWidgetItem(headerTitle);
    headerItem->setFlags(headerItem->flags() | Qt::ItemIsEditable);
    ui->tableWidgetStudents->setHorizontalHeaderItem(column, headerItem);
}

void MainWindow::removeSubject() {
    int column = ui->tableWidgetStudents->currentColumn();
    if (column >= 0) {
        ui->tableWidgetStudents->removeColumn(column);
    }
}
