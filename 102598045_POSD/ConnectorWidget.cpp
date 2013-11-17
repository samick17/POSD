#include "ConnectorWidget.h"
#include <QPainter>
#include "Connector.h"
#include <iostream>

using namespace Qt;

ConnectorWidget::ConnectorWidget(Component* component,GraphicalPresentation* graphicalPresentation) : ComponentWidget(component,graphicalPresentation){
    Connector* connector = static_cast<Connector*>(this->component);
    connector->updateRect();
    this->updateWidget();
}

ConnectorWidget::~ConnectorWidget(){
}

QPainterPath ConnectorWidget::shape() const{
    QPainterPath path;
    path.moveTo(this->sourcePoint);
    path.addEllipse(this->sourcePoint,WidgetDefaultSetting::WidgetLineWidth,WidgetDefaultSetting::WidgetLineWidth);
    path.lineTo(this->targetPoint);
    path.addEllipse(this->targetPoint,WidgetDefaultSetting::WidgetLineWidth,WidgetDefaultSetting::WidgetLineWidth);
    QPainterPathStroker painterPathStroker;
    painterPathStroker.setWidth(WidgetDefaultSetting::ConnectorWidth);
    path = painterPathStroker.createStroke(path);
    return path;
}

void ConnectorWidget::doUpdateWidget(){
    Connector* connector = static_cast<Connector*>(this->component);
    Component* firstComponent = connector->getFirstConnectedNode();
    Component* secondComponent = connector->getSecondConnectedNode();
    if(!firstComponent && !secondComponent)
        return;
    Rect sourceRect = firstComponent->getRect();
    Rect targetRect = secondComponent->getRect();
    pair<Point,Point> minDistancePointPair = sourceRect.getMinDistanceToRectPoint(targetRect);
    Point sourcePoint = minDistancePointPair.first;
    Point targetPoint = minDistancePointPair.second;

    this->sourcePoint = QPointF(sourcePoint.getX(),sourcePoint.getY());
    this->targetPoint = QPointF(targetPoint.getX(),targetPoint.getY());
}