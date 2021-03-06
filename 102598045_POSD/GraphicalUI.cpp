#include "GraphicalUI.h"
#include <QKeyEvent>
#include <QFileDialog>
#include <QSignalMapper>
#include <QInputDialog>
#include <QHBoxLayout>
#include <QMenuBar>
#include <QMessageBox>
#include <QSplitter>
#include "GUIScene.h"
#include "GUITableScene.h"
#include "GUITableView.h"
#include "GUIComponentTableView.h"
#include "FileMenuItem.h"
#include "AddMenuItem.h"
#include "EditMenuItem.h"
#include "HelpMenuItem.h"
#include "FileToolBar.h"
#include "EditToolBar.h"
#include "AddDrawableToolBar.h"
#include "BrowseToolBar.h"
#include "GraphicalPresentation.h"
#include "ApplicationSetting.h"
#include "ActionData.h"
#include "ControllerEvent.h"
#include "StateID.h"
#include "DialogSetting.h"

GraphicalUI::GraphicalUI(GraphicalPresentation* graphicalPresentation): graphicalPresentation(graphicalPresentation),QMainWindow(){
    this->setTitle(this->graphicalPresentation->getTitle());
    this->resize(ApplicationSetting::DefaultWidth,ApplicationSetting::DefaultHeight);
    this->initialGraphicView();
    this->initialAllAction();
    this->initialMenuBar();
    this->initialToolBar();
    this->initialSyncMap();
    QMetaObject::connectSlotsByName(this);
    qRegisterMetaType<string>("string");
    connect(this,SIGNAL(onSyncEvent(string)),this,SLOT(executeSync(string)));
    this->switchState(StateID::PointerState);
    this->graphicalPresentation->registerSynchronizer(this);
    this->graphicalPresentation->notifyModel();
    this->graphicalPresentation->notify();
}

GraphicalUI::~GraphicalUI(){
    this->graphicalPresentation->unregisterSynchronizer(this);
}

GraphicalPresentation* GraphicalUI::getGraphicalPresentation(){
    return this->graphicalPresentation;
}

void GraphicalUI::sync(string syncEventType){
    this->onSyncEvent(syncEventType);
}

void GraphicalUI::closeEvent(QCloseEvent* closeEvent){
    this->close();
}

void GraphicalUI::keyPressEvent(QKeyEvent* keyEvent){
    this->setKeyCtrlPressed(keyEvent);
}

void GraphicalUI::keyReleaseEvent(QKeyEvent* keyEvent){
    this->setKeyCtrlPressed(keyEvent);
}

void GraphicalUI::setTitle(const char* title,string iconPath){
    this->setWindowTitle(title);
    this->setWindowIcon(QIcon(iconPath.c_str()));
}

void GraphicalUI::initialGraphicView(){
    QHBoxLayout* horizontalBoxLayout = new QHBoxLayout();
    QWidget* centralWidget = new QWidget(this);
    centralWidget->setLayout(horizontalBoxLayout);
    this->setCentralWidget(centralWidget);

    QGraphicsScene* scene = new GUIScene(this);
    QGraphicsView* view = new GUIScrollView(scene,this);
    view->setMouseTracking(true);
    QTableWidget* componentTableView = new GUIComponentTableView(this->graphicalPresentation);
    QGraphicsScene* tableScene = new GUITableScene(this);
    QGraphicsView* tableView = new GUITableView(this->graphicalPresentation,tableScene,this);
    QSplitter* viewSplitterWidget = new QSplitter(Qt::Horizontal);
    QSplitter* leftViewSplitterWidget = new QSplitter(Qt::Vertical);

    QVBoxLayout* verticalBoxLayout = new QVBoxLayout();
    QWidget* viewWidget = new QWidget(centralWidget);
    viewWidget->setLayout(verticalBoxLayout);

    horizontalBoxLayout->addWidget(viewSplitterWidget);
    verticalBoxLayout->addWidget(leftViewSplitterWidget);
    leftViewSplitterWidget->addWidget(view);
    leftViewSplitterWidget->addWidget(tableView);
    leftViewSplitterWidget->setStretchFactor(leftViewSplitterWidget->indexOf(view),COMPONENT_VIEW_STRETCH);
    leftViewSplitterWidget->setStretchFactor(leftViewSplitterWidget->indexOf(tableView),TABLE_VIEW_STRETCH);

    viewSplitterWidget->addWidget(leftViewSplitterWidget);
    viewSplitterWidget->addWidget(componentTableView);
    viewSplitterWidget->setStretchFactor(viewSplitterWidget->indexOf(leftViewSplitterWidget),COMPONENT_VIEW_STRETCH);
    horizontalBoxLayout->setStretchFactor(viewWidget,COMPONENT_VIEW_STRETCH);
    horizontalBoxLayout->setStretchFactor(componentTableView,TABLE_VIEW_STRETCH);
}

void GraphicalUI::initialAllAction(){
    this->actionMap = new QActionMap(this);
    QAction* openFileAction = this->actionMap->getQAction(ActionData::OpenFile);
    connect(openFileAction,SIGNAL(triggered()),this,SLOT(openFile()));
    QAction* saveFileAction = this->actionMap->getQAction(ActionData::SaveFile);
    connect(saveFileAction,SIGNAL(triggered()),this,SLOT(saveFile()));
    QAction* saveXmlFileAction = this->actionMap->getQAction(ActionData::SaveXmlFile);
    connect(saveXmlFileAction,SIGNAL(triggered()),this,SLOT(saveXmlFile()));
    QAction* exitAction = this->actionMap->getQAction(ActionData::Exit);
    connect(exitAction,SIGNAL(triggered()),this,SLOT(close()));
    QAction* undoAction = this->actionMap->getQAction(ActionData::Undo);
    connect(undoAction,SIGNAL(triggered()),this,SLOT(undo()));
    QAction* redoAction = this->actionMap->getQAction(ActionData::Redo);
    connect(redoAction,SIGNAL(triggered()),this,SLOT(redo()));
    QAction* deleteComponentAction = this->actionMap->getQAction(ActionData::Delete);
    connect(deleteComponentAction,SIGNAL(triggered()),this,SLOT(deleteComponent()));

    //Set State Action : use signal mapper to pass state argument
    QSignalMapper* signalMapper = new QSignalMapper(this);
    int stateID = StateID::PointerState;
    for(unsigned int index = ActionData::PointerState;index <= ActionData::SetPrimaryKeyState;index++){
        QAction* stateAction = this->actionMap->getQAction(index);
        connect(stateAction,SIGNAL(triggered()),signalMapper,SLOT(map()));
        if(stateID >= StateID::AddAttributeState && stateID <= StateID::AddRelationShipState)
            connect(stateAction,SIGNAL(triggered()),this,SLOT(displayEditTextDialog()));
        signalMapper->setMapping(stateAction,stateID);
        stateID++;
    }
    connect (signalMapper,SIGNAL(mapped(int)),this,SLOT(switchState(int)));

    QAction* cutAction = this->actionMap->getQAction(ActionData::Cut);
    connect(cutAction,SIGNAL(triggered()),this,SLOT(cutComponents()));
    QAction* copyAction = this->actionMap->getQAction(ActionData::Copy);
    connect(copyAction,SIGNAL(triggered()),this,SLOT(copyComponents()));
    QAction* pasteAction = this->actionMap->getQAction(ActionData::Paste);
    connect(pasteAction,SIGNAL(triggered()),this,SLOT(pasteComponents()));

    QAction* browseDBAction = this->actionMap->getQAction(ActionData::BrowseDB);
    connect(browseDBAction,SIGNAL(triggered()),this,SLOT(displayDBTable()));
    QAction* aboutAction = this->actionMap->getQAction(ActionData::About);
    connect(aboutAction,SIGNAL(triggered()),this,SLOT(displayAbout()));
}

void GraphicalUI::initialMenuBar(){
    QMenuBar* menuBar = new QMenuBar(this);
    menuBar->addMenu(new FileMenuItem(this->actionMap,this));
    menuBar->addMenu(new AddMenuItem(this->actionMap,this));
    menuBar->addMenu(new EditMenuItem(this->actionMap,this));
    menuBar->addMenu(new HelpMenuItem(this->actionMap,this));
    this->setMenuBar(menuBar);
}

void GraphicalUI::initialToolBar(){
    this->addToolBar(new FileToolBar(this,this->actionMap));
    this->addToolBar(new EditToolBar(this,this->actionMap));
    this->addToolBar(new AddDrawableToolBar(this,this->actionMap));
    this->addToolBar(new BrowseToolBar(this,this->actionMap));
}

void GraphicalUI::initialSyncMap(){
    this->syncMap.put(ControllerEvent::OpenFile,&GraphicalUI::refreshAllWidgets);
    this->syncMap.put(ControllerEvent::AddNode,&GraphicalUI::refreshAllWidgets);
    this->syncMap.put(ControllerEvent::ConnectTwoNodes,&GraphicalUI::refreshAllWidgets);
    this->syncMap.put(ControllerEvent::DisplayDiagram,&GraphicalUI::refreshAllWidgets);
    this->syncMap.put(ControllerEvent::SetPrimaryKey,&GraphicalUI::refreshAllWidgets);
    this->syncMap.put(ControllerEvent::DisplayTable,&GraphicalUI::refreshAllWidgets);
    this->syncMap.put(ControllerEvent::DeleteComponent,&GraphicalUI::refreshAllWidgets);
    this->syncMap.put(ControllerEvent::Undo,&GraphicalUI::refreshAllWidgets);
    this->syncMap.put(ControllerEvent::Redo,&GraphicalUI::refreshAllWidgets);
    this->syncMap.put(ControllerEvent::Close,&GraphicalUI::close);
}

void GraphicalUI::openFile(){
    QString openFileCaption = QString(ActionData::ActionName[ActionData::OpenFile].c_str());
    QString defaultFileDir = QString(ApplicationSetting::FilePath.c_str());
    QString openFileFilter = QString(ApplicationSetting::FullERDFileExtension.c_str());
    QString openFilePath = QFileDialog::getOpenFileName(this,openFileCaption,defaultFileDir,openFileFilter);
    if(!openFilePath.isEmpty()){
        this->graphicalPresentation->openFile(openFilePath.toStdString());
    }
}

void GraphicalUI::saveFile(){
    QString saveFileCaption = QString(ActionData::ActionName[ActionData::SaveFile].c_str());
    QString defaultFileDir = QString(ApplicationSetting::FilePath.c_str());
    QString saveFileFilter = QString((ApplicationSetting::FullERDFileExtension+";;"+ApplicationSetting::FullXmlFileExtension).c_str());
    QString saveFilePath = QFileDialog::getSaveFileName(this,saveFileCaption,defaultFileDir,saveFileFilter);
    if(!saveFilePath.isEmpty()){
        this->graphicalPresentation->saveFile(saveFilePath.toStdString());
    }
    this->graphicalPresentation->notifyModel();
}

void GraphicalUI::saveXmlFile(){
    QString saveFilePath = QFileDialog::getSaveFileName(this,QString(ActionData::ActionName[ActionData::SaveFile].c_str()),QString(ApplicationSetting::FilePath.c_str()),QString(ApplicationSetting::FullXmlFileExtension.c_str()));
    if(!saveFilePath.isEmpty()){
        this->graphicalPresentation->saveFile(saveFilePath.toStdString());
    }
    this->graphicalPresentation->notifyModel();
}

void GraphicalUI::close(){
    if(this->graphicalPresentation->isNeedToSave()){
        QMessageBox::StandardButton reply;
        reply = QMessageBox::question(this, ApplicationSetting::Title.c_str(), "Do you want to save file?",QMessageBox::Yes|QMessageBox::No);
        if (reply == QMessageBox::Yes)
            this->saveFile();
    }
    this->graphicalPresentation->close();
}

void GraphicalUI::undo(){
    this->graphicalPresentation->undo();
}

void GraphicalUI::redo(){
    this->graphicalPresentation->redo();
}

void GraphicalUI::deleteComponent(){
    this->graphicalPresentation->deleteComponent();
}

void GraphicalUI::switchState(int stateID){
    this->graphicalPresentation->switchState(stateID);
}

//execute sunchronization event that are mapped.
void GraphicalUI::executeSync(string syncEventType){
    if(this->syncMap.containsKey(syncEventType)){
        ViewSyncFunction syncFunction = this->syncMap.get(syncEventType);
        (this->*syncFunction)();
    }
}

void GraphicalUI::refreshAllWidgets(){
    this->graphicalPresentation->notifyModel();
}

void GraphicalUI::setKeyCtrlPressed(QKeyEvent* keyEvent){
    bool isCtrlKey = (keyEvent->key() == Key_Control);
    bool isKeyPressed = keyEvent->type() == QEvent::KeyPress;

    bool isCtrlKeyPressed = isCtrlKey && isKeyPressed;
    this->graphicalPresentation->setKeyCtrlState(isCtrlKeyPressed);
}

void GraphicalUI::displayEditTextDialog(){
    bool isOKClicked;
    QString text = QInputDialog::getText(this,QString(DialogSetting::Title.c_str()),QString(DialogSetting::Text.c_str()),QLineEdit::Normal,StringSymbol::Empty.c_str(),&isOKClicked);
    if(isOKClicked){
        ComponentData* componentData = this->graphicalPresentation->getComponentDataForPreview();
        componentData->setName(text.toStdString());
        this->graphicalPresentation->notifyModel();
    }
    else{
        this->graphicalPresentation->switchState(StateID::PointerState);
    }
}

void GraphicalUI::cutComponents(){
    this->graphicalPresentation->cutComponents();
}

void GraphicalUI::copyComponents(){
    this->graphicalPresentation->copyComponents();
}

void GraphicalUI::pasteComponents(){
    this->graphicalPresentation->pasteComponents();
}

void GraphicalUI::displayDBTable(){
    this->graphicalPresentation->switchDisplayDBTable();
    this->graphicalPresentation->notify();
}

void GraphicalUI::displayAbout(){
    QMessageBox::about(this,ApplicationSetting::AboutTitle.c_str(),ApplicationSetting::AboutText.c_str());
}