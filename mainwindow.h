#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFile>
#include <QGraphicsScene>
#include <QGraphicsRectItem>
#include <QStandardItemModel>
#include <QItemSelectionModel>
#include "logger.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void closeEvent(QCloseEvent * e);


enum TCollise
{
    NoCollise,
    Collise,
    PartCollise,
    Undefine,
    isBroked
};

enum EDir
{
    DNone,
    DBigger,
    DSmaller
};

struct SFigRect
{
    QGraphicsRectItem * Figrect;
    TCollise TypeCollise;
};

enum FigState{
    FSTooBig,
    FSInProcess,
    FSStay
};

struct SFigState
{
    FigState fs;
    float Fsquare;
    int nn;
    QString FName;
};

struct SLists{
    float Lsquare;
    int nn;
    float height;
    float width;
    QString clr;
    QString mtr;
};

struct SFTrans
{
    float x;
    float y;
    int cang;
    bool ang180;
    bool ang270;
};

struct SExItem
{
    int Listnn;
    QList <int> FigsIDs;
};

private:
    Ui::MainWindow *ui;
    void Inits();
    void RotateToNormal(QLineF);
    QList <QString> SVGFn;
    QGraphicsScene * scene;
    QGraphicsRectItem * ListGI;
    QList <QList<SFigRect>> rects;
    QList <QList <QGraphicsPathItem *>> gpi;
    QList <QList <QGraphicsPathItem *>> gpiRotate;
    QList <SFigState> gpState;
    QList <QGraphicsRectItem*> LList;
    QList <SExItem> ExArr;
    QStandardItemModel * ListSourseModel;
    QItemSelectionModel * ListSourseModelsm;
    QString SVGPath;
    bool FindNext;
    bool HasYet(int);
    void CheckInt();
    void RotatePaths(float);
    void OpenFile();
    void GetMaxMinPos();
    void SetFigRect();
    bool isPointinPath(QPointF);
    void GetTypeCollide(int, SFigRect *);
    void SetColorForRect(int);
    void BrokeRect(int);
    void GlueRects();
    void clearscene();
    void GetAvailSvgFromFolder();
    void AnalizeFigs();
    float GetLastSqr();
    bool InException(QList <int> * Figs, SLists lst);
    bool TrySetupFigs(QList <int> *, SLists, bool SetLabels);
    void AddToException(QList <int> *, SLists);
    QList <int> maxpp;
    float hip,lowp,leftp,rightp;
    static int const FirstKdiv=10;
    static int const NQuality=4;
    static int const MaxFigPerList=4;
    static int const KListGrid=10;
    static int const XStart=0;
    static int const YStart=-20000;
    static int const SlideOffset=-10000;
    int FigID;
    Logger * lg;
    float maxlwList;
    void GetMaxListSize();
    void GetRandomFigs(int,QList <int> *);
    int random(int nn);
    bool CheckLosts();
    float GetSelectSqr(QList <int> *);
    bool TryToMakeSmaller(QList <int> *);
    bool TryToMakeBigger(QList <int> *);
    QPointF GetLeftTopPointOfFig(int);
    bool isFigsCollide(int f1, int f2);
    bool isFigInList(int Realnum, QGraphicsRectItem* litm, bool *BadWidth, bool *BadHeight, QPointF pt);
    float CurList;
    void RotateFigure(int FigID);
    bool StopFlag;
    void SlideUseless(QList <int> *, int);
    void pause(int ms);
    void GetOdds(float, float, int, float *, float*);
    void clearRects();
    void AddItemLabel(QGraphicsItem *, QString, bool isList = true);

public slots:
    void exit();
    void RunSlot();
    void SetListSize();
  //  void ListVisible(bool);
  //  void gpiVisible(bool);
  //  void gpiRotateVisible(bool);
    void IndexChange(QModelIndex);
    void FindStr();
    void FindNextDown();
    void MakeChange();
    void NewList();
    void DelItem();
    void FolderChoose();
    void StopSignal();
    void ShowHideListPanel();
    void RotateSelectedRight();
    void RotateSelectedLeft();
    void DeleteListFromScene();
    void AddListToScene();
};


#endif // MAINWINDOW_H
