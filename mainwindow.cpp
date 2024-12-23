#include "mainwindow.h"
#include "./ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
    taskInput(new QLineEdit),
    addButton(new QPushButton("Добавить")),
    deleteButton(new QPushButton("Удалить")),
    taskList(new QListWidget),
    ui(new Ui::MainWindow) {
    //ui->setupUi(this); // Не стала возиться с QtDesigner, для такого небольшого интерфейса проще руками все виджеты, формы и кнопки сделать

    QWidget *mainWidget = new QWidget(this);
    QVBoxLayout *mainLayout = new QVBoxLayout(mainWidget);

    QHBoxLayout *inputLayout = new QHBoxLayout;
    inputLayout->addWidget(taskInput);
    inputLayout->addWidget(addButton);

    mainLayout->addLayout(inputLayout);
    mainLayout->addWidget(taskList);
    mainLayout->addWidget(deleteButton);

    setCentralWidget(mainWidget);

    connect(addButton, &QPushButton::clicked, this, &MainWindow::addTask);
    connect(deleteButton, &QPushButton::clicked, this, &MainWindow::deleteTask);
    connect(taskList, &QListWidget::itemChanged, this, &MainWindow::toggleTask);

    loadTasks();
}

MainWindow::~MainWindow() {
    saveTasks();
    delete ui;
}

void MainWindow::addTask() {
    QString taskText = taskInput->text().trimmed();
    if (taskText.isEmpty()) {
        QMessageBox::warning(this, "Ошибка", "Введите текст задачи!");
        return;
    }

    QListWidgetItem *item = new QListWidgetItem(taskText, taskList);
    item->setFlags(item->flags() | Qt::ItemIsUserCheckable);
    item->setCheckState(Qt::Unchecked);

    taskInput->clear();
}

void MainWindow::deleteTask() {
    QList<QListWidgetItem *> selectedItems = taskList->selectedItems();
    for (QListWidgetItem *item : selectedItems) {
        delete item;
    }
}

void MainWindow::toggleTask(QListWidgetItem *item) {
    if (item->checkState() == Qt::Checked) {
        auto tempFont = item->font();
        tempFont.setStrikeOut(true);
        item->setFont(tempFont);
    } else {
        item->setFont(QFont("", -1, QFont::Normal));
    }
}

void MainWindow::loadTasks() {
    QSettings settings("TaskList", "TaskListApp");
    int size = settings.beginReadArray("tasks");

    for (int i = 0; i < size; ++i) {
        settings.setArrayIndex(i);
        QString text = settings.value("text").toString();
        bool isCompleted = settings.value("completed").toBool();

        QListWidgetItem *item = new QListWidgetItem(text, taskList);
        item->setFlags(item->flags() | Qt::ItemIsUserCheckable);
        item->setCheckState(isCompleted ? Qt::Checked : Qt::Unchecked);
        toggleTask(item);
    }

    settings.endArray();

    resize(settings.value("windowSize", QSize(400, 300)).toSize());
}

void MainWindow::saveTasks() {
    QSettings settings("TaskList", "TaskListApp");
    settings.beginWriteArray("tasks");

    for (int i = 0; i < taskList->count(); ++i) {
        settings.setArrayIndex(i);
        QListWidgetItem *item = taskList->item(i);
        settings.setValue("text", item->text());
        settings.setValue("completed", item->checkState() == Qt::Checked);
    }

    settings.endArray();

    settings.setValue("windowSize", size());
}
