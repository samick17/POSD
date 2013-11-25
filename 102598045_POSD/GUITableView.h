#pragma once

#include <QTableWidget>
#include <QLabel>
#include <string>
#include "IObserver.h"

class GraphicalPresentation;

using namespace std;

class GUITableView : public QTableWidget,public IObserver{
public:
    GUITableView(GraphicalPresentation* graphicalPresentation);
    ~GUITableView();

    void notify(ISubject* subject);
protected:
    void keyPressEvent(QKeyEvent* keyEvent);
private:
    static const string TableTitle;
    static const int TableSize;
    static const string TableColumnTypeName;
    static const string TableColumnTextName;
    GraphicalPresentation* graphicalPresentation;
};