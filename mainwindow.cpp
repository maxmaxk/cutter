#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <QFileDialog>
#include <QGraphicsItem>
#include <QWheelEvent>
#include <qmath.h>
#include <QDomDocument>
#include <QMessageBox>
#include <QElapsedTimer>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow), FindNext(false), StopFlag(true)
{
   Inits();
}

void MainWindow::exit()
{
    QApplication::quit();
}

void MainWindow::closeEvent(QCloseEvent *e)\
{
    StopFlag=true;
    if (lg) delete lg;
    QWidget::closeEvent(e);
}

MainWindow::~MainWindow()
{
    StopFlag=true;
    QFile dbFile;
    dbFile.setFileName("db.xml");
    QDomDocument DBDOM;
    QDomElement dEr0=DBDOM.createElement("db");
    DBDOM.appendChild(dEr0);

    for (int i=0; i<ListSourseModel->rowCount(); ++i)
    {
       QDomElement dEr1=DBDOM.createElement("list");
       QModelIndex mi=ListSourseModel->index(i,0);
       dEr1.setAttribute("mtr",ListSourseModel->data(mi).toString());
       mi=ListSourseModel->index(i,1);
       dEr1.setAttribute("color",ListSourseModel->data(mi).toString());
       mi=ListSourseModel->index(i,2);
       dEr1.setAttribute("length",ListSourseModel->data(mi).toString());
       mi=ListSourseModel->index(i,3);
       dEr1.setAttribute("width",ListSourseModel->data(mi).toString());
       dEr0.appendChild(dEr1);
    }

    QDomElement dEr1=DBDOM.createElement("svg");
    dEr1.setAttribute("path",SVGPath);
    dEr0.appendChild(dEr1);

    QTextStream ts(&dbFile);
    if (dbFile.open(QIODevice::WriteOnly))
    {
        DBDOM.save(ts,0);
        dbFile.close();
    }


    delete ui;
}

void MainWindow::Inits()
{
    ui->setupUi(this);
    connect(ui->action,SIGNAL(triggered(bool)),this,SLOT(exit()));
    connect(ui->action_2,SIGNAL(triggered(bool)),this,SLOT(FolderChoose()));

    scene = new QGraphicsScene;
    ui->graphicsView->setScene(scene);

    QGraphicsTextItem *item1 = scene->addText("start");
    item1->setPos(QPointF(-50000,-50000));

    QGraphicsTextItem *item2 = scene->addText("end");
    item2->setPos(QPointF(50000,50000));

    ListGI = new QGraphicsRectItem;

    ui->splitter->setSizes(QList <int> () <<600 <<300);

    ListSourseModel = new QStandardItemModel(0,4);
    ListSourseModel->setHeaderData(0, Qt::Horizontal, "Материал", Qt::DisplayRole);
    ListSourseModel->setHeaderData(1, Qt::Horizontal, "Цвет", Qt::DisplayRole);
    ListSourseModel->setHeaderData(2, Qt::Horizontal, "Длина", Qt::DisplayRole);
    ListSourseModel->setHeaderData(3, Qt::Horizontal, "Ширина", Qt::DisplayRole);

    ui->tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);

    ui->tableView->setModel(ListSourseModel);
    ui->tableView->verticalHeader()->setDefaultSectionSize(20);

    ListSourseModelsm = new QItemSelectionModel(ListSourseModel);
    ui->tableView->setSelectionModel(ListSourseModelsm);

    connect(ListSourseModelsm,SIGNAL(currentRowChanged(QModelIndex,QModelIndex)),this,SLOT(IndexChange(QModelIndex)));
    connect(ListSourseModelsm,SIGNAL(currentChanged(QModelIndex,QModelIndex)),this,SLOT(FindNextDown()));

    QFile dbFile;
    dbFile.setFileName("db.xml");
    maxlwList=0;
    QDomDocument DBDOM;

    if (dbFile.open(QIODevice::ReadOnly))
    {
        if (DBDOM.setContent(&dbFile))
        {
          QDomElement MainEl = DBDOM.documentElement();
          QDomNode dN0=MainEl.firstChild();
          while (!dN0.isNull())
          {
              if (dN0.isElement())
              {
                 QDomElement dE0=dN0.toElement();
                 if (dE0.tagName()=="list")
                 {

                     int CurrRow=ListSourseModel->rowCount();

                     QList<QStandardItem*>itm;

                     ListSourseModel->insertRow(CurrRow,itm);
                     QModelIndex index = ListSourseModel->index(CurrRow, 0);
                     ListSourseModel->setData(index,dE0.attribute("mtr"));
                     index = ListSourseModel->index(CurrRow, 1);
                     ListSourseModel->setData(index,dE0.attribute("color"));
                     index = ListSourseModel->index(CurrRow, 2);
                     ListSourseModel->setData(index,dE0.attribute("length"));
                     bool ok;
                     float l=dE0.attribute("length").toFloat(&ok);
                     if (ok)
                         if (l>maxlwList) maxlwList=l;

                     index = ListSourseModel->index(CurrRow, 3);
                     ListSourseModel->setData(index,dE0.attribute("width"));
                     float w=dE0.attribute("width").toFloat(&ok);
                     if (ok)
                         if (w>maxlwList) maxlwList=w;

                 }
                 if (dE0.tagName()=="svg")
                 {
                    SVGPath=dE0.attribute("path");
                 }
              }
              dN0=dN0.nextSibling();
          }
        }
        dbFile.close();
    }

    lg = new Logger();

    connect(ui->pushButton_5,SIGNAL(clicked(bool)),this,SLOT(FindStr()));
    connect(ui->lineEdit_2,SIGNAL(returnPressed()),this,SLOT(FindStr()));
    connect(ui->pushButton_3,SIGNAL(clicked(bool)),this,SLOT(MakeChange()));
    connect(ui->pushButton,SIGNAL(clicked(bool)),this,SLOT(NewList()));
    connect(ui->pushButton_4,SIGNAL(clicked(bool)),this,SLOT(DelItem()));
    connect(ui->pushButton_6,SIGNAL(clicked(bool)),this,SLOT(RunSlot()));
    connect(ui->pushButton_2,SIGNAL(clicked(bool)),this,SLOT(ShowHideListPanel()));
    connect(lg,SIGNAL(StopSignal()),this,SLOT(StopSignal()));

    ui->pushButton_7->setIcon(QPixmap(":/g16782.png"));
    ui->pushButton_8->setIcon(QPixmap(":/g16780.png"));
    ui->pushButton_12->setIcon(QPixmap(":/g4495.png"));
    ui->pushButton_13->setIcon(QPixmap(":/g4511.png"));

    connect(ui->pushButton_7,SIGNAL(clicked(bool)),this,SLOT(RotateSelectedLeft()));
    connect(ui->pushButton_8,SIGNAL(clicked(bool)),this,SLOT(RotateSelectedRight()));

    connect(ui->pushButton_10,SIGNAL(clicked(bool)),this,SLOT(DeleteListFromScene()));
    connect(ui->pushButton_9,SIGNAL(clicked(bool)),this,SLOT(AddListToScene()));
}

void MainWindow::RunSlot()
{

    if (!StopFlag)
    {
       lg->show();
       lg->activateWindow();
       return;
    }

    StopFlag=false;

    lg->ListSourseModel->removeRows(0,lg->ListSourseModel->rowCount());
    lg->show();



    clearscene();
    GetAvailSvgFromFolder();
    GetMaxListSize();
    for (FigID=0; FigID<SVGFn.count(); FigID++)
    {
        int CurrRow=lg->ListSourseModel->rowCount();
        QList<QStandardItem*> itm;
        lg->ListSourseModel->insertRow(CurrRow,itm);
        QModelIndex ci=lg->ListSourseModel->index(CurrRow,0);
        QFileInfo fi(SVGFn.at(FigID));
        lg->ListSourseModel->setData(ci,fi.fileName());
    }

    QApplication::processEvents();

    for (FigID=0; FigID<SVGFn.count(); FigID++)
    {

        if (StopFlag) return;

        QModelIndex ci=lg->ListSourseModel->index(FigID,1);
        lg->ListSourseModel->setData(ci,"Открытие файла");
        QApplication::processEvents();
        OpenFile();
        if (maxpp.at(FigID)<0)
        {
            lg->ListSourseModel->setData(ci,"Не найдена выкройка");
            QList <SFigRect> nrect;
            rects.append(nrect);
            continue;
        }

        if (gpState.at(FigID).fs==FSTooBig)
        {
            lg->ListSourseModel->setData(ci,"Размеры выкройки больше размеров имеющихся листов");
            QList <SFigRect> nrect;
            rects.append(nrect);
            continue;
        }

        GetMaxMinPos();
        lg->ListSourseModel->setData(ci,"Преобразование");
        QApplication::processEvents();
        SetFigRect();
        bool q;
        for (int i=0; i<NQuality; ++i)
        {
            q=true;
            for (int j=0; j<rects.at(FigID).count(); ++j)
            {
                if ((rects.at(FigID).at(j).TypeCollise==Undefine) || (rects.at(FigID).at(j).TypeCollise==PartCollise))
                {
                    q=false;
                    SFigRect fr=rects.at(FigID).at(j);
                    GetTypeCollide(j,&fr);
                    QList <SFigRect> drects=rects.at(FigID);
                    drects.replace(j,fr);
                    rects.replace(FigID,drects);
                }
            }
            if (q) break;
            if (i<(NQuality-1))
            {
                int k=rects.at(FigID).count();
                for (int j=0; j<k; ++j)
                    if (rects.at(FigID).at(j).TypeCollise==PartCollise)
                        BrokeRect(j);
            }
        }
        GlueRects();
        int TotalRects=0;
        float FigSquare=0;
        for (int j=0; j<rects.at(FigID).count(); ++j)
        {
            if ((rects.at(FigID).at(j).TypeCollise==Collise) || (rects.at(FigID).at(j).TypeCollise==PartCollise))
            {
                FigSquare+=rects.at(FigID).at(j).Figrect->rect().width()*rects.at(FigID).at(j).Figrect->rect().height();
                rects.at(FigID).at(j).Figrect->setBrush(QBrush(QColor(18,73,114,50)));
                rects.at(FigID).at(j).Figrect->setParentItem(gpiRotate.at(FigID).at(maxpp.at(FigID)));
                ++TotalRects;
            }
        }
        FigSquare/=1e+06;
        SFigState fs=gpState.at(FigID);
        fs.Fsquare=FigSquare;
        gpState.replace(FigID,fs);

        lg->ListSourseModel->setData(ci,"Кол-во фигур: "+QString::number(TotalRects)+"; Площадь: "+QString::number(FigSquare));
        QApplication::processEvents();
    }

    AnalizeFigs();

    for (int jc=0; jc<gpState.count(); ++jc)
    {
        SFigState fs=gpState.at(jc);
        if (fs.fs==FSInProcess)
        {
            QModelIndex ci=lg->ListSourseModel->index(fs.nn,1);
            lg->ListSourseModel->setData(ci,"Не удалось разместить выкройку");
        }
    }

    StopFlag=true;

}

float GetLen(QPointF p2, QPointF p1)
{
    float dx=p2.x()-p1.x();
    float dy=p2.y()-p1.y();
    return sqrt(dx*dx+dy*dy);
}

void MainWindow::RotatePaths(float fi)
{
    QMatrix mtx;
    mtx.translate(5000,10000*FigID);
    mtx.rotate(fi);

    for (int i=0; i<gpiRotate.at(FigID).count(); ++i)
    {
        QPolygonF pf=gpiRotate.at(FigID).at(i)->path().toFillPolygon(mtx);
        QPainterPath pp;
        pp.addPolygon(pf);
        gpiRotate.at(FigID).at(i)->setPath(pp);
    }
}

void MainWindow::RotateToNormal(QLineF maxlenLine)
{
    float fi=maxlenLine.angle();
    fi-=90;

    QGraphicsPathItem * pi;


    QList <QGraphicsPathItem*> ngpiRotate;

    QList <QGraphicsPathItem*> dgpi=gpi.at(FigID);

    foreach (pi, dgpi) {
        QGraphicsPathItem * newpi = new QGraphicsPathItem;
        newpi->setPath(pi->path());
        ngpiRotate.append(newpi);
    }

    gpiRotate.append(ngpiRotate);

    scene->addItem(gpiRotate.at(FigID).at(maxpp.at(FigID)));
    gpiRotate.at(FigID).at(maxpp.at(FigID))->setBrush(QBrush(QColor(255,153,0,50)));
    gpiRotate.at(FigID).at(maxpp.at(FigID))->setFlags(QGraphicsItem::ItemIsSelectable | QGraphicsItem::ItemIsMovable);
    for (int i=0; i<gpiRotate.at(FigID).count(); ++i)
    {
        if (i!=maxpp.at(FigID))
        {
           scene->addItem(gpiRotate.at(FigID).at(i));
           gpiRotate.at(FigID).at(i)->setBrush(QBrush(QColor(222,170,87,100)));
           gpiRotate.at(FigID).at(i)->setParentItem(gpiRotate.at(FigID).at(maxpp.at(FigID)));
        }
    }


    RotatePaths(fi);

}

void MainWindow::OpenFile()
{

    float maxlen=0;
    QLineF maxlenLine;

    maxpp.append(-1);
    QList <QGraphicsPathItem*> ngpi;
    gpi.append(ngpi);
    int gpp=-1;

    QDomDocument SVGDOM;

    QFile sf(SVGFn.at(FigID));

    if (sf.open(QIODevice::ReadOnly))
    {
      if (SVGDOM.setContent(&sf))
      {
        QDomElement MainEl = SVGDOM.documentElement();
        QDomNode dN0=MainEl.firstChild();
        while (!dN0.isNull())
        {
            if (dN0.isElement())
            {
               QDomElement dE0=dN0.toElement();
               if (dE0.tagName()=="polygon")
               {
                   ++gpp;
                   QString pts=dE0.attribute("points");
                   QStringList ptssl=pts.split(" ");
                   QPainterPath pp;
                   QPolygonF pf;

                   for (int i=0; i<ptssl.count(); ++i)
                   {
                       QPointF pt;
                       QStringList xy=ptssl.at(i).split(",");
                       if (xy.count()==2)
                       {
                            QString ss=xy.at(0);
                            pt.setX(ss.toFloat());
                            ss=xy.at(1);
                            pt.setY(ss.toFloat());
                            pf.append(pt);
                            if (pf.count()>1)
                            {
                                int k=pf.count()-2;
                                float x1=pf.at(k).x();
                                float y1=pf.at(k).y();
                                QPointF pt1(x1,y1);
                                if (maxlen<GetLen(pt,pt1))
                                {
                                    maxlen=GetLen(pt,pt1);
                                    maxlenLine=QLineF(pt1,pt);
                                    maxpp.replace(FigID,gpp);
                                }
                            }
                       }
                   }

                   if (pf.count()>0) pf.append(pf.at(0));
                   // check last line
                   if (pf.count()>2)
                   {
                        QPointF pt=pf.at(pf.count()-2);
                        QPointF pt1=pf.at(pf.count()-1);
                        if (maxlen<GetLen(pt,pt1))
                        {
                            maxlen=GetLen(pt,pt1);
                            maxlenLine=QLineF(pt1,pt);
                            maxpp.replace(FigID,gpp);
                        }
                   }                   

                   pp.addPolygon(pf);
                   QGraphicsPathItem * newgpi = new QGraphicsPathItem;
                   newgpi->setPath(pp);
                   QList <QGraphicsPathItem *> dgpi;
                   dgpi=gpi.at(FigID);
                   dgpi.append(newgpi);
                   gpi.replace(FigID,dgpi);
               }
            }
            dN0=dN0.nextSibling();
        }
      }


    gpi.at(FigID).at(maxpp.at(FigID))->setBrush(QBrush(QColor(0,128,255,50)));
    gpi.at(FigID).at(maxpp.at(FigID))->setFlags(QGraphicsItem::ItemIsSelectable | QGraphicsItem::ItemIsMovable);
    gpi.at(FigID).at(maxpp.at(FigID))->setPos(0,10000*FigID);
    for (int i=0; i<gpi.at(FigID).count(); ++i)
    {
        if (i!=maxpp.at(FigID))
        {
           gpi.at(FigID).at(i)->setBrush(QBrush(QColor(222,170,87,100)));
           gpi.at(FigID).at(i)->setParentItem(gpi.at(FigID).at(maxpp.at(FigID)));
        }
    }


    if (maxlen>0)
    {
        RotateToNormal(maxlenLine);
        if (maxlen>maxlwList)
        {
            SFigState fs;
            fs.fs=FSTooBig;
            gpState.append(fs);
        }
        else
        {
            SFigState fs;
            fs.fs=FSInProcess;
            QFileInfo fi(SVGFn.at(FigID));
            fs.FName=fi.baseName();
            gpState.append(fs);
        }
    }

    sf.close();
   }

}

void MainWindow::SetListSize()
{
    QRectF rct(0,0,ui->lineEdit->text().toInt(),ui->lineEdit_3->text().toInt());
    ListGI->setRect(rct);
    ListGI->setFlags(QGraphicsItem::ItemIsSelectable | QGraphicsItem::ItemIsMovable);
    ListGI->setBrush(QBrush(QColor(10,10,10,50)));
}


void MainWindow::IndexChange(QModelIndex idx)
{
    QModelIndex idx0=ListSourseModel->index(idx.row(),0);
    ui->lineEdit->setText(ListSourseModel->data(idx0).toString());
    idx0=ListSourseModel->index(idx.row(),1);
    ui->lineEdit_3->setText(ListSourseModel->data(idx0).toString());
    idx0=ListSourseModel->index(idx.row(),2);
    ui->lineEdit_4->setText(ListSourseModel->data(idx0).toString());
    idx0=ListSourseModel->index(idx.row(),3);
    ui->lineEdit_5->setText(ListSourseModel->data(idx0).toString());
}

void MainWindow::FindNextDown()
{
    FindNext=false;
}

void MainWindow::FolderChoose()
{
    QFileDialog dialog;
    dialog.setFileMode(QFileDialog::Directory);
    dialog.setOption(QFileDialog::ShowDirsOnly);
    SVGPath=dialog.getExistingDirectory();
}



void MainWindow::DelItem()
{

    QMessageBox* pmbx;
    pmbx = new QMessageBox(QMessageBox::Information, "Удаление листа",
                        "Удалить текущий лист?");

    pmbx->addButton("Да",QMessageBox::AcceptRole);
    pmbx->addButton("Нет",QMessageBox::RejectRole);

    if (pmbx->exec()==QMessageBox::AcceptRole)
    {
        QModelIndex sti=ui->tableView->currentIndex();
        int Fcr=sti.row();
        ListSourseModel->removeRow(Fcr);
    }
}

bool MainWindow::HasYet(int Fcr)
{
    QList <QString> ts;
    ts<<ui->lineEdit->text() << ui->lineEdit_3->text()
      <<ui->lineEdit_4->text() << ui->lineEdit_5->text();
    bool q=false;
    for (int i=0; i<ListSourseModel->rowCount(); ++i)
        if (i!=Fcr)
        {
           bool q1=true;
           for (int j=0; j<ListSourseModel->columnCount(); ++j)
           {
             QModelIndex ci=ListSourseModel->index(i,j);
             if (ts.at(j)!=ListSourseModel->data(ci).toString()) q1=false;
           }

           if (q1) q=true;

        }
    return q;
}

void MainWindow::CheckInt()
{
   QString xx=ui->lineEdit_4->text();
   int xxi=xx.toInt();
   ui->lineEdit_4->setText(QString::number(xxi));

   xx=ui->lineEdit_5->text();
   xxi=xx.toInt();
   ui->lineEdit_5->setText(QString::number(xxi));
}

void MainWindow::NewList()
{

    CheckInt();

    QList <QString> ts;
    ts<<ui->lineEdit->text() << ui->lineEdit_3->text()
      <<ui->lineEdit_4->text() << ui->lineEdit_5->text();

    if (HasYet(-1))
    {
        QMessageBox messageBox;
        messageBox.critical(0,"Ошибка","Лист с такими параметрами уже существует!");
        messageBox.setFixedSize(500,200);
    }
    else
    {

        int CurrRow=ListSourseModel->rowCount();
        QList<QStandardItem*> itm;
        ListSourseModel->insertRow(CurrRow,itm);

        for (int j=0; j<ListSourseModel->columnCount(); ++j)
        {
          QModelIndex ci=ListSourseModel->index(CurrRow,j);
          ListSourseModel->setData(ci,ts.at(j));
        }
    }
}

void MainWindow::MakeChange()
{

   CheckInt();

   QModelIndex sti=ui->tableView->currentIndex();
   int Fcr=sti.row();

   QList <QString> ts;
   ts<<ui->lineEdit->text() << ui->lineEdit_3->text()
     <<ui->lineEdit_4->text() << ui->lineEdit_5->text();

   if (HasYet(Fcr))
   {
       QMessageBox messageBox;
       messageBox.critical(0,"Ошибка","Лист с такими параметрами уже существует!");
       messageBox.setFixedSize(500,200);
   }
   else
   {
       for (int j=0; j<ListSourseModel->columnCount(); ++j)
       {
         QModelIndex ci=ListSourseModel->index(Fcr,j);
         ListSourseModel->setData(ci,ts.at(j));
       }
   }

}

void MainWindow::FindStr()
{

    QModelIndex sti=ui->tableView->currentIndex();
    int Fcr=sti.row();
    int Fcc=sti.column();
    do {
        QModelIndex ci=ListSourseModel->index(Fcr,Fcc);
        QString Str=ListSourseModel->data(ci).toString();
        if (!FindNext)
            if (Str.contains(ui->lineEdit_2->text()))
            {
                ui->tableView->setCurrentIndex(ci);
             break;
            }
        FindNext=false;
        ++Fcc;
        if (Fcc>=ListSourseModel->columnCount())
        {
            Fcc=0;
            ++Fcr;
        }
        if (Fcr>=ListSourseModel->rowCount())
        {
            Fcr=0;
            Fcc=0;
        }
    }
    while (!((Fcr==sti.row()) && (Fcc==sti.column())));
    FindNext=true;

}

void MainWindow::GetMaxMinPos()
{
    if (!gpiRotate.at(FigID).at(maxpp.at(FigID)))
    {
        hip=0;
        lowp=0;
        leftp=0;
        rightp=0;
        return;
    }

    QPolygonF pf=gpiRotate.at(FigID).at(maxpp.at(FigID))->path().toFillPolygon();

    for (int i=0; i<pf.count(); ++i)
    {
        if (i==0)
        {
            leftp=pf.at(i).x();
            rightp=pf.at(i).x();
            hip=pf.at(i).y();
            lowp=pf.at(i).y();
        }
        else
        {
            if (pf.at(i).x()<leftp) leftp=pf.at(i).x();
            if (pf.at(i).x()>rightp) rightp=pf.at(i).x();
            if (pf.at(i).y()<hip) hip=pf.at(i).y();
            if (pf.at(i).y()>lowp) lowp=pf.at(i).y();

        }
    }

}

void MainWindow::SetFigRect()
{

    QList <SFigRect> nrect;
    rects.append(nrect);

    float width1=(rightp-leftp)/FirstKdiv;
    float height1=(lowp-hip)/FirstKdiv;
    SFigRect pi;
    for (int i=0; i<FirstKdiv; ++i)
    {
        for (int j=0; j<FirstKdiv; ++j)
        {
            QGraphicsRectItem * FigRect = new QGraphicsRectItem;
            FigRect->setRect(leftp+j*width1,hip+i*height1,width1,height1);
            pi.Figrect=FigRect;
            pi.TypeCollise=Undefine;
            QList<SFigRect> drects=rects.at(FigID);
            drects.append(pi);
            rects.replace(FigID,drects);
        }
    }

}

bool MainWindow::isPointinPath(QPointF pt)
{
    QPolygonF pf=gpiRotate.at(FigID).at(maxpp.at(FigID))->path().toFillPolygon();
    int CrossCount=0;
    if (pf.count()>2)
    {
        for (int i=0; i<pf.count()-1; ++i)
        {
            QLineF lf;
            lf.setP1(pf.at(i));
            lf.setP2(pf.at(i+1));

            float xMin=qMin(lf.p1().x(),lf.p2().x());
            float xMax=qMax(lf.p1().x(),lf.p2().x());
            float yMin=qMin(lf.p1().y(),lf.p2().y());

            if ((pt.x()<xMin) || (pt.x()>xMax)) continue;
            if (pt.y()<yMin) continue;

            float a=lf.p1().y();
            if (qAbs(lf.p2().x()-lf.p1().x())<0.001)
            {
                continue;
            }
            float b=(lf.p2().y()-lf.p1().y())/(lf.p2().x()-lf.p1().x());
            float crossY=a+b*(pt.x()-lf.p1().x());

            if (crossY<=pt.y())
                ++CrossCount;

        }


    }

    return (CrossCount % 2 != 0);
}

void MainWindow::GetTypeCollide(int j, SFigRect * fr)
{
    QRectF rct=rects.at(FigID).at(j).Figrect->rect();

    QPointF p1=rct.topLeft();
    p1.setX(p1.x()+0.1);
    p1.setY(p1.y()+0.1);
    bool q1=isPointinPath(p1);

    QPointF p2=rct.topRight();
    p2.setX(p2.x()-0.1);
    p2.setY(p2.y()+0.1);
    bool q2=isPointinPath(p2);

    QPointF p3=rct.bottomLeft();
    p3.setX(p3.x()+0.1);
    p3.setY(p3.y()-0.1);
    bool q3=isPointinPath(p3);

    QPointF p4=rct.bottomRight();
    p4.setX(p4.x()-0.1);
    p4.setY(p4.y()-0.1);
    bool q4=isPointinPath(p4);



    if (q1 && q2 && q3 && q4) fr->TypeCollise=Collise;
    else
        if (!q1 && !q2 && !q3 && !q4) fr->TypeCollise=NoCollise;
        else fr->TypeCollise=PartCollise;



}

void MainWindow::BrokeRect(int j)
{
    SFigRect fr=rects.at(FigID).at(j);
    fr.TypeCollise=isBroked;


    QRectF rct;

    QList <SFigRect> drects=rects.at(FigID);



    SFigRect fr1;
    fr1.Figrect = new QGraphicsRectItem;
    fr1.TypeCollise=Undefine;
    rct.setLeft(fr.Figrect->rect().left());
    rct.setTop(fr.Figrect->rect().top());
    rct.setWidth(fr.Figrect->rect().width()/2);
    rct.setHeight(fr.Figrect->rect().height()/2);
    fr1.Figrect->setRect(rct);

    drects.append(fr1);

    SFigRect fr2;
    fr2.Figrect = new QGraphicsRectItem;
    fr2.TypeCollise=Undefine;
    rct.setLeft(fr.Figrect->rect().left()+fr.Figrect->rect().width()/2);
    rct.setTop(fr.Figrect->rect().top());
    rct.setWidth(fr.Figrect->rect().width()/2);
    rct.setHeight(fr.Figrect->rect().height()/2);
    fr2.Figrect->setRect(rct);
    drects.append(fr2);

    SFigRect fr3;
    fr3.Figrect = new QGraphicsRectItem;
    fr3.TypeCollise=Undefine;
    rct.setLeft(fr.Figrect->rect().left());
    rct.setTop(fr.Figrect->rect().top()+fr.Figrect->rect().height()/2);
    rct.setWidth(fr.Figrect->rect().width()/2);
    rct.setHeight(fr.Figrect->rect().height()/2);
    fr3.Figrect->setRect(rct);
    drects.append(fr3);

    SFigRect fr4;
    fr4.Figrect = new QGraphicsRectItem;
    fr4.TypeCollise=Undefine;
    rct.setLeft(fr.Figrect->rect().left()+fr.Figrect->rect().width()/2);
    rct.setTop(fr.Figrect->rect().top()+fr.Figrect->rect().height()/2);
    rct.setWidth(fr.Figrect->rect().width()/2);
    rct.setHeight(fr.Figrect->rect().height()/2);
    fr4.Figrect->setRect(rct);
    drects.append(fr4);

    delete fr.Figrect;
    drects.replace(j,fr);

    rects.replace(FigID,drects);

}

void MainWindow::GlueRects()
{
    QList <int> GlueList;


    QRectF GlueRect;

    // horizontal glue

    int k=rects.at(FigID).count();
    for (int i=0; i<k; ++i)
        if ((rects.at(FigID).at(i).TypeCollise==Collise) || (rects.at(FigID).at(i).TypeCollise==PartCollise))
        {
            GlueRect=rects.at(FigID).at(i).Figrect->rect();
            GlueList.clear();
            GlueList.append(i);
            for (int j=i+1; j<k; ++j)
                if ((rects.at(FigID).at(j).TypeCollise==Collise) || (rects.at(FigID).at(j).TypeCollise==PartCollise))
                {
                    QRectF newrct=rects.at(FigID).at(j).Figrect->rect();
                    if ((qAbs(newrct.left()-GlueRect.right())<0.01) &&
                        (newrct.top()==GlueRect.top()) && (newrct.height()==GlueRect.height()))
                    {
                        GlueRect.setWidth(GlueRect.width()+newrct.width());
                        GlueList.append(j);
                    }
                }

            if (GlueList.count()>1)
            {

                QList <SFigRect> drects=rects.at(FigID);

                foreach (int j, GlueList) {
                    SFigRect fr=rects.at(FigID).at(j);
                    fr.TypeCollise=isBroked;
                    delete fr.Figrect;
                    drects.replace(j,fr);
                }

                SFigRect fr;
                QGraphicsRectItem * rcti = new QGraphicsRectItem;
                rcti->setRect(GlueRect);
                fr.Figrect=rcti;
                fr.TypeCollise=Collise;
                drects.append(fr);

                rects.replace(FigID,drects);
            }

        }

    // vertical glue

    k=rects.at(FigID).count();
    for (int i=0; i<k; ++i)
        if ((rects.at(FigID).at(i).TypeCollise==Collise) || (rects.at(FigID).at(i).TypeCollise==PartCollise))
        {
            GlueRect=rects.at(FigID).at(i).Figrect->rect();
            GlueList.clear();
            GlueList.append(i);
            for (int j=i+1; j<k; ++j)
                if ((rects.at(FigID).at(j).TypeCollise==Collise) || (rects.at(FigID).at(j).TypeCollise==PartCollise))
                {
                    QRectF newrct=rects.at(FigID).at(j).Figrect->rect();
                    if ((qAbs(newrct.top()-GlueRect.bottom())<0.01) &&
                        (newrct.left()==GlueRect.left()) && (newrct.width()==GlueRect.width()))
                    {
                        GlueRect.setHeight(GlueRect.height()+newrct.height());
                        GlueList.append(j);
                    }
                }

            if (GlueList.count()>1)
            {

                QList <SFigRect> drects=rects.at(FigID);

                foreach (int j, GlueList) {
                    SFigRect fr=rects.at(FigID).at(j);
                    fr.TypeCollise=isBroked;
                    delete fr.Figrect;
                    drects.replace(j,fr);
                }

                SFigRect fr;
                QGraphicsRectItem * rcti = new QGraphicsRectItem;
                rcti->setRect(GlueRect);
                fr.Figrect=rcti;
                fr.TypeCollise=Collise;
                drects.append(fr);

                rects.replace(FigID,drects);

            }

        }
}

void MainWindow::clearscene()
{
    for (int i=0; i<maxpp.count(); ++i) {
        if (maxpp.at(i)>=0)
        {
            scene->removeItem(gpiRotate.at(i).at(maxpp.at(i)));
            delete gpi.at(i).at(maxpp.at(i));
            delete gpiRotate.at(i).at(maxpp.at(i));
        }
    }

    gpi.clear();
    gpiRotate.clear();
    maxpp.clear();
    rects.clear();
    SVGFn.clear();
    gpState.clear();

    foreach (QGraphicsRectItem* litm, LList) {
       scene->removeItem(litm);
       delete litm;
    }
    LList.clear();

}

void MainWindow::GetAvailSvgFromFolder()
{
    QDir dr(SVGPath);
    QFileInfoList fil;
    QStringList slnil;
    fil=dr.entryInfoList(slnil);
    for (int i=0; i<fil.count(); ++i)
        if (fil.at(i).isFile())
            if (fil.at(i).suffix()=="svg")
            {

                SVGFn.append(SVGPath+"/"+fil.at(i).fileName());

            }
}

void MainWindow::GetMaxListSize()
{
    maxlwList=0;
    for (int i=0; i<ListSourseModel->rowCount(); ++i)
    {
        QModelIndex mi=ListSourseModel->index(i,2);
        bool ok;
        float l=ListSourseModel->data(mi).toFloat(&ok);
        if (ok)
            if (maxlwList<l) maxlwList=l;

        mi=ListSourseModel->index(i,3);
        float w=ListSourseModel->data(mi).toFloat(&ok);
        if (ok)
            if (maxlwList<w) maxlwList=w;
    }
}

void MainWindow::AnalizeFigs()
{

    clearRects();

    // Calc a square for lists
    QList <SLists> ListSqr;
    for (int i=0; i<ListSourseModel->rowCount(); ++i)
    {
        QModelIndex mi1;
        mi1=ListSourseModel->index(i,2);
        QModelIndex mi2;
        mi2=ListSourseModel->index(i,3);
        SLists sl;
        sl.nn=i;
        sl.height=ListSourseModel->data(mi1).toFloat();
        sl.width=ListSourseModel->data(mi2).toFloat();
        float f1=ListSourseModel->data(mi1).toFloat()*ListSourseModel->data(mi2).toFloat();
        sl.Lsquare=f1/1e+06;

        mi1=ListSourseModel->index(i,0);
        mi2=ListSourseModel->index(i,1);
        sl.mtr=ListSourseModel->data(mi1).toString();
        sl.clr=ListSourseModel->data(mi2).toString();

        ListSqr.append(sl);
    }

    //Sort lists

    for (int i=0; i<ListSqr.count()-1; ++i)
    {
        float MaxSqr=ListSqr.at(i).Lsquare;
        int maxpos=i;
        for (int j=i+1; j<ListSqr.count(); ++j)
            if (MaxSqr<ListSqr.at(j).Lsquare)
            {
                MaxSqr=ListSqr.at(j).Lsquare;
                maxpos=j;
            }
        if (maxpos!=i)
        {
           SLists buff=ListSqr.at(i);
           ListSqr.replace(i,ListSqr.at(maxpos));
           ListSqr.replace(maxpos,buff);
        }
    }

    if (ListSqr.count()==0)
    {
        for (int i=0; i<gpState.count(); ++i) {
            QModelIndex ci=lg->ListSourseModel->index(i,1);
            lg->ListSourseModel->setData(ci,"Не найдены листы");
        }
        return;
    }

    // Unset figures with more square than bigger list
    for (int i=0; i<gpState.count(); ++i) {

       SFigState fs=gpState.at(i);

       if (fs.Fsquare>ListSqr.at(0).Lsquare)
       {
           fs.fs=FSTooBig;
           gpState.replace(i,fs);

           QModelIndex ci=lg->ListSourseModel->index(i,1);
           lg->ListSourseModel->setData(ci,"Размеры выкройки больше размеров имеющихся листов");

       }


    }



    // sort figures
    for (int i=0; i<gpState.count(); ++i)
    {
        SFigState fs=gpState.at(i);
        fs.nn=i;
        gpState.replace(i,fs);
    }

    for (int i=0; i<gpState.count()-1; ++i)
    {
        float MaxSqr=gpState.at(i).Fsquare;
        int maxpos=i;
        for (int j=i+1; j<gpState.count(); ++j)
            if (MaxSqr<gpState.at(j).Fsquare)
            {
                MaxSqr=gpState.at(j).Fsquare;
                maxpos=j;
            }
        if (maxpos!=i)
        {
           SFigState buff=gpState.at(i);
           gpState.replace(i,gpState.at(maxpos));
           gpState.replace(maxpos,buff);
        }
    }

    //Unset fig, which not stay on any list
    for (int i=0; i<gpState.count(); ++i)
    {
        QList <int> OneFig;
        OneFig.append(i);
        bool CanStay=false;
        for (int j=0; j<ListSqr.count(); ++j)
        {
            if (TrySetupFigs(&OneFig,ListSqr.at(j),false))
            {
                CanStay=true;
                break;
            }
        }
        if (!CanStay)
        {
            SFigState fs=gpState.at(i);
            fs.fs=FSTooBig;
            gpState.replace(i,fs);

            QModelIndex ci=lg->ListSourseModel->index(fs.nn,1);
            lg->ListSourseModel->setData(ci,"Размеры выкройки больше размеров имеющихся листов");
        }
    }



    foreach (QGraphicsRectItem* litm, LList) {
       scene->removeItem(litm);
       delete litm;
    }
    LList.clear();


    // startAnalize

    float Prlevel=0.9;
    int k=MaxFigPerList;
    int ListNum=0;
    CurList=0;
    ExArr.clear();

    while (Prlevel>-0.1)
    {


        if (CheckLosts()) break;

        QList <int> Figs;

        GetRandomFigs(k,&Figs);

        bool SetupOK=false;

        if (k==Figs.count())
        {

                for (int li=ListSqr.count()-1; li>=0; --li)
                {
                    if (SetupOK || StopFlag) break;
                    bool q=true;
                    EDir GrowDirection=DNone;
                    while (q && !StopFlag)
                    {

                        float SelSqr=GetSelectSqr(&Figs);

                        float KS=SelSqr/ListSqr.at(li).Lsquare;

                        if ((KS>=Prlevel) && (KS<1))
                        {
                            if (!InException(&Figs,ListSqr.at(li)))
                            {

                                QList <QString> RememberStatus;

                                for (int jc=0; jc<lg->ListSourseModel->rowCount(); ++jc)
                                {
                                    QModelIndex ci=lg->ListSourseModel->index(jc,1);
                                    RememberStatus.append(lg->ListSourseModel->data(ci).toString());
                                }

                                for (int jc=0; jc<Figs.count(); ++jc)
                                {
                                    SFigState fs=gpState.at(Figs.at(jc));
                                    QModelIndex ci=lg->ListSourseModel->index(fs.nn,1);
                                    lg->ListSourseModel->setData(ci,"Попытка расположить выкройку");
                                }

                                QApplication::processEvents();

                                if (TrySetupFigs(&Figs,ListSqr.at(li),true))
                                {
                                    ListNum++;
                                    foreach (int i1, Figs)
                                    {
                                        SFigState fs=gpState.at(i1);
                                        fs.fs=FSStay;
                                        gpState.replace(i1,fs);

                                        QModelIndex mi1=ListSourseModel->index(ListSqr.at(li).nn,2);
                                        QModelIndex mi2=ListSourseModel->index(ListSqr.at(li).nn,3);

                                        QModelIndex ci=lg->ListSourseModel->index(fs.nn,1);
                                        lg->ListSourseModel->setData(ci,"S="+QString::number(fs.Fsquare)+"; Выкройка размещена на листе ("+QString::number(ListNum)+')'+
                                              ListSourseModel->data(mi1).toString()+'x'+ListSourseModel->data(mi2).toString());

                                    }
                                    SetupOK=true;
                                    q=false;
                                }
                                else
                                {

                                    AddToException(&Figs,ListSqr.at(li));
                                    if (RememberStatus.count()==lg->ListSourseModel->rowCount())
                                        for (int jc=0; jc<lg->ListSourseModel->rowCount(); ++jc)
                                        {
                                            QModelIndex ci=lg->ListSourseModel->index(jc,1);
                                            lg->ListSourseModel->setData(ci,RememberStatus.at(jc));
                                        }
                                    QApplication::processEvents();

                                }



                            }
                        }

                        if (q)
                        {
                            if  ((SelSqr/ListSqr.at(li).Lsquare>=1) &&
                                 ((GrowDirection==DNone) || (GrowDirection==DSmaller)))
                                {
                                    q=TryToMakeSmaller(&Figs);
                                    GrowDirection=DSmaller;
                                }
                            else
                            {
                                if ((SelSqr/ListSqr.at(li).Lsquare<Prlevel) &&
                                    ((GrowDirection==DNone) || (GrowDirection==DBigger)))
                                {
                                    q=TryToMakeBigger(&Figs);
                                    GrowDirection=DBigger;
                                }
                                else q=false;
                            }
                        }

                    }
                }




        }


        if (!SetupOK)
        {
            k--;
            if (k==0)
            {
                k=MaxFigPerList;
                Prlevel-=0.1;

            }
        }


    }
}


void MainWindow::AddToException(QList <int> * Figs, SLists lst)
{
   SExItem ei;
   ei.Listnn=lst.nn;
   for (int i=0; i<Figs->count(); i++)
       ei.FigsIDs.append(Figs->at(i));
   ExArr.append(ei);
}

float MainWindow::GetLastSqr()
{
    float res=0;
    foreach (SFigState fs, gpState)
       if (fs.fs==FSInProcess)
       {
            res+=fs.Fsquare;
       }
    return res;
}

bool MainWindow::InException(QList <int> * Figs, SLists lst)
{
    for (int i=0; i<ExArr.count(); ++i)
    {
        if (lst.nn==ExArr.at(i).Listnn)
        {
            if (Figs->count()==ExArr.at(i).FigsIDs.count())
            {
                bool q=true;
                for (int j=0; j<Figs->count(); ++j)
                    if (Figs->at(j)!=ExArr.at(i).FigsIDs.at(j))
                    {
                        q=false;
                        break;
                    }
                if (q) return true;

            }
        }
    }
    return false;
}

void MainWindow::GetRandomFigs(int k, QList <int> * Figs)
{
    QList <int> FNs;
    for (int i=0; i<gpState.count(); ++i)
    {
       SFigState fs=gpState.at(i);
       if (fs.fs==FSInProcess)
       {
            FNs.append(i);
       }
    }

    if (FNs.count()<k) return;
    if (FNs.count()==k)
    {
        foreach (int i, FNs) Figs->append(i);
        return;
    }
    float f1=FNs.count();
    float f2=k;
    int n;
    if (f1/f2>2) n=k;
    else n=FNs.count()-k;

    QList <int> RNums;

    while (RNums.count()<n)
    {
        bool q=true;
        while (q)
        {
            int rn=random(FNs.count()-1);
            q=false;
            int pi=-1;
            for (int j=0; j<RNums.count(); ++j)
            {
                if (RNums.at(j)==rn)
                {
                    q=true;
                    break;
                }
                if (pi==-1)
                    if (RNums.at(j)>rn) pi=j;
            }
            if (!q)
            {
               if (pi==-1) RNums.append(rn);
               else RNums.insert(pi,rn);
            }
        }
    }

    if (f1/f2>2)
    {
        foreach (int i, RNums) Figs->append(FNs.at(i));
    }
    else
    {
        int pp=0;
        for (int i=0; i<FNs.count(); ++i)
        {
            if (pp<RNums.count())
            {
              if (i!=RNums.at(pp))
                   Figs->append(FNs.at(i));
              else pp++;
            }
            else Figs->append(FNs.at(i));
        }
    }



}

int MainWindow::random(int nn)
{
    float kk;
    int res;
    do
    {
        kk=qrand();
        kk/=RAND_MAX;
        kk*=(nn+1);
        res=kk;
    }
    while (kk>nn);

    return res;
}

bool MainWindow::CheckLosts()
{
    return false;
}

float MainWindow::GetSelectSqr(QList <int> * Figs)
{
    float res=0;
    for (int i=0; i<Figs->count(); ++i)
    {
        res+=gpState.at(Figs->at(i)).Fsquare;
    }
    return res;
}

bool MainWindow::TryToMakeSmaller(QList <int> * Figs)
{

   QList <int> CanModifyList;

   for (int i=Figs->count()-1; i>=0; --i)
   {
       int startpos=Figs->at(i);
       int endpos;
       if (i==Figs->count()-1) endpos=gpState.count();
       else endpos=Figs->at(i+1);
       for (int j=startpos+1; j<endpos; ++j)
           if (gpState.at(j).fs==FSInProcess)
           {
              CanModifyList.append(i);
              CanModifyList.append(j);
              break;
           }
   }

   if (CanModifyList.count()==0)
   {
        return false;
   }

   int x=random(CanModifyList.count()-1);
   if (x % 2 == 1) --x;

   Figs->replace(CanModifyList.at(x),CanModifyList.at(x+1));
   return true;
}

bool MainWindow::TryToMakeBigger(QList <int> * Figs)
{

    QList <int> CanModifyList;

    for (int i=0; i<Figs->count(); ++i)
    {
        int startpos=Figs->at(i);
        int endpos;
        if (i==0) endpos=-1;
        else endpos=Figs->at(i-1);
        for (int j=startpos-1; j>endpos; --j)
            if (gpState.at(j).fs==FSInProcess)
            {
               CanModifyList.append(i);
               CanModifyList.append(j);
               break;
            }
    }



    if (CanModifyList.count()==0)
    {
         return false;
    }

    int x=random(CanModifyList.count()-1);
    if (x % 2 == 1) --x;

    Figs->replace(CanModifyList.at(x),CanModifyList.at(x+1));
    return true;
}

QPointF MainWindow::GetLeftTopPointOfFig(int FigID)
{
    QPointF res;
    res=gpiRotate.at(FigID).at(maxpp.at(FigID))->boundingRect().topLeft();
    return res;
}

bool MainWindow::isFigsCollide(int f1, int f2)
{

    return gpiRotate.at(f1).at(maxpp.at(f1))->collidesWithItem(gpiRotate.at(f2).at(maxpp.at(f2)));

    if (!gpiRotate.at(f1).at(maxpp.at(f1))->collidesWithItem(gpiRotate.at(f2).at(maxpp.at(f2)))) return false;

    bool q=false;
    for (int i=0; i<gpiRotate.at(f1).at(maxpp.at(f1))->childItems().count(); ++i)
    {
        for (int j=0; j<gpiRotate.at(f2).at(maxpp.at(f2))->childItems().count(); ++j)
            if (gpiRotate.at(f2).at(maxpp.at(f2))->childItems().at(j)->
                    collidesWithItem(gpiRotate.at(f1).at(maxpp.at(f1))->childItems().at(i)))
            {
                q=true;
                break;
            }
        if (q) break;
    }
    return q;
}

bool MainWindow::isFigInList(int Realnum, QGraphicsRectItem* litm, bool * BadWidth, bool * BadHeight, QPointF pt)
{


    QGraphicsItem * TestItem = gpiRotate.at(Realnum).at(maxpp.at(Realnum));

    QRectF litmrct;
    QRectF Testrct;
    Testrct.setLeft(TestItem->pos().x()+pt.x());
    Testrct.setTop(TestItem->pos().y()+pt.y());
    Testrct.setWidth(TestItem->boundingRect().width());
    Testrct.setHeight(TestItem->boundingRect().height());

    litmrct.setTopLeft(litm->pos());
    litmrct.setWidth(litm->boundingRect().width());
    litmrct.setHeight(litm->boundingRect().height());

    *BadWidth=Testrct.right()>litmrct.right();
    *BadHeight=Testrct.bottom()>litmrct.bottom();
    return (!*BadWidth && !*BadHeight) ;
}

bool MainWindow::TrySetupFigs(QList <int> * Figs, SLists lst, bool SetLabels = true)
{

    QGraphicsRectItem * itmLst = new QGraphicsRectItem;
    itmLst->setRect(QRectF(0,0,lst.width,lst.height));
    itmLst->setPos(XStart,YStart+CurList);
    itmLst->setBrush(QBrush(QColor(99,0,0,50)));
    itmLst->setFlags(QGraphicsItem::ItemIsSelectable | QGraphicsItem::ItemIsMovable);
    itmLst->setZValue(-1);
    scene->addItem(itmLst);

    QList <SFTrans> AFTr;
    SFTrans FTr;
    FTr.x=0;
    FTr.y=0;
    FTr.cang=0;
    FTr.ang180=false;
    FTr.ang270=false;
    for (int i=0; i<Figs->count(); ++i)
        AFTr.append(FTr);

    int i=0;
    float cx=0;
    float cy=0;
    int cang=0;
    int pang=0;
    bool cang180=false;
    bool cang270=false;
    float StepX=lst.width/KListGrid;
    float StepY=lst.height/KListGrid;
    while (i<Figs->count())
    {
        SlideUseless(Figs,i);
        bool toNextFig=false;
        bool ExitFlag=false;
        int realnum=gpState.at(Figs->at(i)).nn;
        QPointF pt=GetLeftTopPointOfFig(realnum);
        float Xodds;
        float Yodds;
        GetOdds(StepX,StepY,realnum,&Xodds,&Yodds);
        gpiRotate.at(realnum).at(maxpp.at(realnum))->setPos(XStart-pt.x()+cx,-pt.y()+YStart+CurList+cy);
        QApplication::processEvents();
        bool BadWidth;
        bool BadHeight;
        if (isFigInList(realnum,itmLst,&BadWidth,&BadHeight,pt))
        {
            bool ok=true;
            for (int df=i-1; df>=0; --df)
            {
                int realnum2=gpState.at(Figs->at(df)).nn;
                if (isFigsCollide(realnum,realnum2))
                {
                    ok=false;
                    break;
                }
            }
            if (ok)
                toNextFig=true;
            else
            {
                // HERE different step for first and others cycles
               if (cx==0) cx=Xodds;
               else cx+=StepX;
            }

        }
        else
        {

            bool q=true;
            if ((cx>0) && BadWidth)
            {
               cx=0;
               if (cy==0) cy=Yodds;
               else cy+=StepY;
               q=false;
            }


            if ((cx==0) && BadWidth && q)
            {
               cy=0;
               if (cang==0) cang180=true;
               if (cang==90) cang270=true;
               cang+=90;
               if ((cang==180) && cang180) cang+=90;
               if ((cang==270) && cang270) cang+=90;
               if (cang>270) ExitFlag=true;
               q=false;
            }

            if (BadHeight && q)
            {
               if (cy==0)
               {
                   if (cang==0) cang180=true;
                   if (cang==90) cang270=true;
               }
               cang+=90;
               if ((cang==180) && cang180) cang+=90;
               if ((cang==270) && cang270) cang+=90;
               if (cang>270) ExitFlag=true;
               q=false;
            }

        }

        if (ExitFlag || StopFlag)
        {
            i--;
            if ((i<0) || StopFlag)
            {
                scene->removeItem(itmLst);
                delete itmLst;
                return false;
            }
            FTr=AFTr.at(i);
            cx=FTr.x;
            realnum=gpState.at(Figs->at(i)).nn;
            GetOdds(StepX,StepY,realnum,&Xodds,&Yodds);
            if (cx==0) cx=Xodds;
            else cx+=StepX;
            cy=FTr.y;
            cang=FTr.cang;
            pang=cang;
            cang180=FTr.ang180;
            cang270=FTr.ang270;
        }
        else
        {
            if (pang!=cang)
            {
                RotateFigure(realnum);
                GetOdds(StepX,StepY,realnum,&Xodds,&Yodds);
                pang=cang;
            }
        }

        if (toNextFig)
        {
            FTr.x=cx;
            FTr.y=cy;
            FTr.cang=cang;
            FTr.ang180=cang180;
            FTr.ang270=cang270;
            AFTr.replace(i,FTr);
            i++;
            cx=0;
            cy=0;
            cang=0;
            pang=0;
            cang180=false;
            cang270=false;
        }
    }

    CurList+=lst.height+5000;
    LList.append(itmLst);

    if (SetLabels)
    {
        AddItemLabel(itmLst,lst.mtr+":"+lst.clr+" "+QString::number(lst.width)+"x"+QString::number(lst.height));
        for (int i=0; i<Figs->count(); ++i)
        {
            int realnum=gpState.at(Figs->at(i)).nn;
            QString fn=gpState.at(Figs->at(i)).FName;
            AddItemLabel(gpiRotate.at(realnum).at(maxpp.at(realnum)),fn,false);
        }
    }

    return true;
}

void MainWindow::RotateFigure(int FigID)
{

    QMatrix mtx;
    mtx.rotate(90);


    QPolygonF pf=gpiRotate.at(FigID).at(maxpp.at(FigID))->path().toFillPolygon(mtx);
    QPainterPath pp;
    pp.addPolygon(pf);
    gpiRotate.at(FigID).at(maxpp.at(FigID))->setPath(pp);


    for (int i=0; i<gpiRotate.at(FigID).at(maxpp.at(FigID))->childItems().count(); ++i)
    {
        QGraphicsItem * itm=gpiRotate.at(FigID).at(maxpp.at(FigID))->childItems().at(i);
        itm->setRotation(itm->rotation()+90);
    }

}

void MainWindow::StopSignal()
{
    StopFlag=true;
    lg->hide();
}

void MainWindow::SlideUseless(QList <int> * Figs,int ti)
{
    for (int i=0; i<gpState.count(); ++i)
    {
        bool q=false;
        for (int j=0; j<ti; ++j)
            if (Figs->at(j)==i)
            {
                q=true;
                break;
            }
        if (!q)
        {
           int realnum=gpState.at(i).nn;
           if ((gpState.at(i).fs==FSInProcess) || (gpState.at(i).fs==FSTooBig))
           {
                QPointF pt=GetLeftTopPointOfFig(realnum);
                gpiRotate.at(realnum).at(maxpp.at(realnum))->setPos(XStart+SlideOffset-pt.x(),-pt.y()+YStart+CurList);
           }
        }
    }
}

void MainWindow::pause(int ms)
{

    QElapsedTimer timer;
    timer.start();
    do {
        QCoreApplication::processEvents(QEventLoop::AllEvents, ms);
        QCoreApplication::sendPostedEvents(0, QEvent::DeferredDelete);
    }
    while (timer.elapsed() < ms);

}

void MainWindow::GetOdds(float StepX, float StepY, int realnum, float * Xodds, float * Yodds)
{
    float wd=gpiRotate.at(realnum).at(maxpp.at(realnum))->boundingRect().width();
    float ht=gpiRotate.at(realnum).at(maxpp.at(realnum))->boundingRect().height();
    int m=wd/StepX;

    *Xodds=(m+1)*StepX-wd;

    m=ht/StepY;
    *Yodds=(m+1)*StepY-ht;


}

void MainWindow::clearRects()
{
    for (int i=0; i<rects.count(); ++i) {
        for (int j=0; j<rects.at(i).count(); ++j)
        {
            if ((rects.at(i).at(j).TypeCollise==Collise) || (rects.at(i).at(j).TypeCollise==PartCollise))
                scene->removeItem(rects.at(i).at(j).Figrect);
        }
    }

}

void MainWindow::ShowHideListPanel()
{
    if (ui->frame->isHidden())
    {
        ui->frame->show();
        ui->pushButton_2->setText("Скрыть панель листов");
    }
    else
    {
        ui->frame->hide();
        ui->pushButton_2->setText("Показать панель листов");
    }
}

void MainWindow::RotateSelectedLeft()
{
    foreach (QGraphicsItem * sgi, scene->selectedItems())
    {
        QPointF pf=sgi->boundingRect().topLeft();
        pf.setX(pf.x()+sgi->boundingRect().width()/2);
        pf.setY(pf.y()+sgi->boundingRect().height()/2);
        sgi->setTransformOriginPoint(pf);
        sgi->setRotation(sgi->rotation()-90);

        bool isList=(sgi->type()==3);

        foreach (QGraphicsItem * sgiChild, sgi->childItems())
        {
            if (sgiChild->type()==8)
            {
                sgiChild->setRotation(sgiChild->rotation()+90);
                QPointF pos;
                int rt=sgi->rotation();
                int m=qAbs(rt/360);
                if (rt<0) rt+=(m+1)*360;
                if (rt==360) rt=0;
                if (rt>270) rt-=m*360;
                qDebug()<<"rt="<<rt;

                QPointF pt=sgi->boundingRect().topLeft();

                switch (rt) {
                case 0:
                    if (isList)
                        pos=QPointF(0,-300);
                    else
                        pos=QPointF(pt.x()+300,pt.y()+300);
                    break;
                case 90:
                    if (isList)
                        pos=QPointF(-300,sgi->boundingRect().height());
                    else
                        pos=QPointF(pt.x()+300,pt.y()+sgi->boundingRect().height());
                    break;
                case 180:
                    if (isList)
                        pos=QPointF(sgi->boundingRect().width(),sgi->boundingRect().height()+300);
                    else
                        pos=QPointF(pt.x()+sgi->boundingRect().width(),pt.y()+sgi->boundingRect().height());
                    break;
                case 270:
                    if (isList)
                        pos=QPointF(sgi->boundingRect().width()+300,0);
                    else
                        pos=QPointF(sgi->boundingRect().width()+pt.x(),pt.y()+300);
                    break;
                default:
                    if (isList)
                        pos=QPointF(0,-300);
                    else pos=QPointF(pt.x()+300,pt.y()+300);
                    break;
                }

                sgiChild->setPos(pos);
            }
        }

    }

}

void MainWindow::RotateSelectedRight()
{
    foreach (QGraphicsItem * sgi, scene->selectedItems())
    {
        QPointF pf=sgi->boundingRect().topLeft();
        pf.setX(pf.x()+sgi->boundingRect().width()/2);
        pf.setY(pf.y()+sgi->boundingRect().height()/2);
        sgi->setTransformOriginPoint(pf);
        sgi->setRotation(sgi->rotation()+90);

        bool isList=(sgi->type()==3);

        foreach (QGraphicsItem * sgiChild, sgi->childItems())
        {
            if (sgiChild->type()==8)
            {
                sgiChild->setRotation(sgiChild->rotation()-90);
                QPointF pos;
                int rt=sgi->rotation();
                int m=qAbs(rt/360);
                if (rt<0) rt+=(m+1)*360;
                if (rt==360) rt=0;
                if (rt>270) rt-=m*360;
                qDebug()<<"rt="<<rt;
                QPointF pt=sgi->boundingRect().topLeft();

                switch (rt) {
                case 0:
                    if (isList)
                        pos=QPointF(0,-300);
                    else
                        pos=QPointF(pt.x()+300,pt.y()+300);
                    break;
                case 90:
                    if (isList)
                        pos=QPointF(-300,sgi->boundingRect().height());
                    else
                        pos=QPointF(pt.x()+300,pt.y()+sgi->boundingRect().height());
                    break;
                case 180:
                    if (isList)
                        pos=QPointF(sgi->boundingRect().width(),sgi->boundingRect().height()+300);
                    else
                        pos=QPointF(pt.x()+sgi->boundingRect().width(),pt.y()+sgi->boundingRect().height());
                    break;
                case 270:
                    if (isList)
                        pos=QPointF(sgi->boundingRect().width()+300,0);
                    else
                        pos=QPointF(sgi->boundingRect().width()+pt.x(),pt.y()+300);
                    break;
                default:
                    if (isList)
                        pos=QPointF(0,-300);
                    else pos=QPointF(pt.x()+300,pt.y()+300);
                    break;
                }

                sgiChild->setPos(pos);

            }
        }

    }
}

void MainWindow::DeleteListFromScene()
{
    foreach (QGraphicsItem * sgi, scene->selectedItems())
    {
        int j=LList.count()-1;
        while (j>=0)
        {
            if (LList.at(j)==sgi)
            {
                scene->removeItem(sgi);
                LList.removeAt(j);
                delete sgi;
            }
            --j;
        }
    }
}

void MainWindow::AddListToScene()
{
    int k=ui->tableView->currentIndex().row();
    if (k>=0)
    {
        QString LabelName;
        QModelIndex mi=ListSourseModel->index(k,0);
        QModelIndex mi1=ListSourseModel->index(k,1);
        LabelName=ListSourseModel->data(mi).toString()+":"+ListSourseModel->data(mi1).toString()+" ";
        QGraphicsRectItem * itmLst = new QGraphicsRectItem;
        QRectF rct;
        rct.setTopLeft(QPointF(0,0));
        bool ok;
        mi=ListSourseModel->index(k,3);
        rct.setWidth(ListSourseModel->data(mi).toFloat(&ok));
        LabelName+=ListSourseModel->data(mi).toString()+"x";
        if (!ok) return;
        mi=ListSourseModel->index(k,2);
        rct.setHeight(ListSourseModel->data(mi).toFloat(&ok));
        LabelName+=ListSourseModel->data(mi).toString();
        if (!ok) return;

        itmLst->setRect(rct);
        QPointF pos=ui->graphicsView->mapToScene(QPoint(30,30));
        itmLst->setPos(pos);
        itmLst->setBrush(QBrush(QColor(99,0,0,50)));
        itmLst->setFlags(QGraphicsItem::ItemIsSelectable | QGraphicsItem::ItemIsMovable);
        itmLst->setZValue(-1);
        scene->addItem(itmLst);
        AddItemLabel(itmLst,LabelName);
        LList.append(itmLst);
    }
}

void MainWindow::AddItemLabel(QGraphicsItem * gi, QString LabelName, bool isList)
{
    QGraphicsTextItem * TxtItem = new QGraphicsTextItem(LabelName,gi);
    TxtItem->setFont(QFont("Arial",200));
    if (isList)
        TxtItem->setPos(0,-300);
    else
    {
        QPointF pt=gi->boundingRect().topLeft();
        TxtItem->setPos(pt.x()+300,pt.y()+300);
    }
}
