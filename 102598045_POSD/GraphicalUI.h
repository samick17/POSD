#pragma once

#include <QMainWindow>
#include <QGraphicsView>
#include <QMenuBar>
#include <QToolBar>
#include "GUIScene.h"
#include "FileMenuItem.h"
#include "AddMenuItem.h"
#include "FileToolBar.h"
#include "AddDrawableToolBar.h"
#include "QActionMap.h"
#include "ISynchronizer.h"
#include "Component.h"

class GraphicalPresentation;
class SelectedFrameWidget;

using namespace Qt;

class GraphicalUI : public QMainWindow,public ISynchronizer{
    Q_OBJECT
signals:
    void onSyncEvent(string syncEventType);
public:
    GraphicalUI(GraphicalPresentation* graphicalPresentation);
    ~GraphicalUI();

    GraphicalPresentation* getGraphicalPresentation();
    void sync(string syncEventType);
    void setTitle(string title,string iconPath);
protected:
    void closeEvent(QCloseEvent* closeEvent);
    void keyPressEvent(QKeyEvent* keyEvent);
    void keyReleaseEvent(QKeyEvent* keyEvent);
private:
    GraphicalPresentation* graphicalPresentation;
    FileMenuItem* fileMenuItem;
    AddMenuItem* addMenuItem;
    FileToolBar* fileToolBar;
    AddDrawableToolBar* addDrawableToolBar;
    QMenuBar* menuBar;
    QGraphicsView* view;
    GUIScene* scene;
    QActionMap* actionMap;
    typedef void (GraphicalUI::*ViewSyncFunction)();
    HashMap<string,ViewSyncFunction> syncMap;

    void initialGraphicView();
    void initialAllAction();
    void initialMenuBar();
    void initialToolBar();
    void initialSyncMap();
    void refreshAllWidgets();
    void setKeyCtrlPressed(QKeyEvent* keyEvent);
private slots:
    void openFile();
    void close();
    void switchState(int stateID);
    void executeSync(string syncEventType);
};