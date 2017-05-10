#include "logger.h"
#include "ui_logger.h"

Logger::Logger(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Logger)
{
    ui->setupUi(this);
    ListSourseModel = new QStandardItemModel(0,2);
    ui->tableView->setModel(ListSourseModel);
    ui->tableView->verticalHeader()->setDefaultSectionSize(20);
    ui->tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ListSourseModel->setHeaderData(0, Qt::Horizontal, "Выкройка", Qt::DisplayRole);
    ListSourseModel->setHeaderData(1, Qt::Horizontal, "Статус", Qt::DisplayRole);
    connect(ui->pushButton,SIGNAL(clicked(bool)),this,SIGNAL(StopSignal()));
}

Logger::~Logger()
{
    delete ui;
}
