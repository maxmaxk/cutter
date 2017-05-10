#ifndef LOGGER_H
#define LOGGER_H

#include <QWidget>
#include <QStandardItemModel>

namespace Ui {
class Logger;
}

class Logger : public QWidget
{
    Q_OBJECT  

public:
    explicit Logger(QWidget *parent = 0);
    ~Logger();
    QStandardItemModel * ListSourseModel;

signals:
    void StopSignal();

private:
    Ui::Logger *ui;

};

#endif // LOGGER_H
