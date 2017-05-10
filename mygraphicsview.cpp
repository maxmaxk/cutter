#include "mygraphicsview.h"
#include <QWheelEvent>
#include "qmath.h"
#include <QDebug>
#include <QScrollBar>

MyGraphicsView::MyGraphicsView(QWidget *parent):Zoom(-150)
{
    Q_UNUSED(parent)
    qreal scale = qPow(qreal(2), (Zoom - 250) / qreal(50));

    QMatrix matrix;
    matrix.scale(scale, scale);

    setMatrix(matrix);
}


#ifndef QT_NO_WHEELEVENT
void MyGraphicsView::wheelEvent(QWheelEvent *e)
{

   // this->setInteractive(true);

    if (e->delta() > 0)

        Zoom+=16;
    else
        Zoom-=16;




    if (Zoom<-150) Zoom=-150;
    if (Zoom>500) Zoom=500;

    qreal scale = qPow(qreal(2), (Zoom - 250) / qreal(50));

    QMatrix matrix;
    matrix.scale(scale, scale);

    setMatrix(matrix);

    e->accept();
}
#endif

void MyGraphicsView::mousePressEvent(QMouseEvent *evt)
{
    if (evt->buttons() & Qt::MidButton)
    {
      this->setDragMode(QGraphicsView::ScrollHandDrag);
      DrStartPos=evt->globalPos();
    }
    QGraphicsView::mousePressEvent(evt);
}

void MyGraphicsView::mouseMoveEvent(QMouseEvent *evt)
{
    if (this->dragMode()==QGraphicsView::ScrollHandDrag)
    {

        int yy=this->verticalScrollBar()->value()-evt->globalY()+DrStartPos.ry();
        this->verticalScrollBar()->setValue(yy);

        int xx=this->horizontalScrollBar()->value()-evt->globalX()+DrStartPos.rx();
        this->horizontalScrollBar()->setValue(xx);
        DrStartPos=evt->globalPos();
    }
    QGraphicsView::mouseMoveEvent(evt);
}

void MyGraphicsView::mouseReleaseEvent(QMouseEvent *evt)
{

    if (evt->button() & Qt::MidButton)
    {
      this->setDragMode(QGraphicsView::RubberBandDrag);
    }
    QGraphicsView::mouseReleaseEvent(evt);
}
