#pragma once

#include <QMainWindow>
#include <QGraphicsView>
#include <QMenuBar>
#include <QToolBar>
#include "GUIScene.h"
#include "FileMenuItem.h"
#include "FileToolBar.h"
#include "AddDrawableToolBar.h"
#include "QActionMap.h"
#include "IObserver.h"

class GraphicalPresentation;
class SelectedFrameWidget;

using namespace Qt;

class GraphicalUI : public QMainWindow,IObserver{
    Q_OBJECT
signals:
    void notifyEvent(int notifiedEventType);
public:
    GraphicalUI(GraphicalPresentation* graphicalPresentation);
    ~GraphicalUI();

    GraphicalPresentation* getGraphicalPresentation();
    void notify(int notifiedEventType);
    void setTitle(string title);

    void mousePress();
    void mouseMove();
    void mouseRelease();
protected:
    void closeEvent(QCloseEvent* closeEvent);
    void keyPressEvent(QKeyEvent* keyEvent);
    void keyReleaseEvent(QKeyEvent* keyEvent);
private:
    GraphicalPresentation* graphicalPresentation;
    FileMenuItem* fileMenuItem;
    FileToolBar* fileToolBar;
    AddDrawableToolBar* addDrawableToolBar;
    QMenuBar* menuBar;
    QGraphicsView* view;
    GUIScene* scene;
    QActionMap* actionMap;
    typedef void (GraphicalUI::*ViewNotifyFunction)();
    HashMap<int,ViewNotifyFunction> notifyMap;

    void initialGraphicView();
    void initialAllAction();
    void initialMenuBar();
    void initialToolBar();
    void initialNotifyMap();
    void refresh();
    void displayDiagram();
private slots:
    void openFile();
    void close();
    void switchState(int stateID);
    void executeNotify(int notifiedEventType);
};