#ifndef MYGRAPHICSVIEW_H
#define MYGRAPHICSVIEW_H

#include <QObject>
#include <QGraphicsView>

class MyGraphicsView : public QGraphicsView
{
    Q_OBJECT
public:
    MyGraphicsView(QWidget *parent = 0);

protected:
#ifndef QT_NO_WHEELEVENT
   void wheelEvent(QWheelEvent *);
#endif
   void mousePressEvent(QMouseEvent *evt);
   void mouseMoveEvent(QMouseEvent *evt);
   void mouseReleaseEvent(QMouseEvent *evt);
private:
   int Zoom;
   QPoint DrStartPos;

public slots:
};

#endif // MYGRAPHICSVIEW_H
