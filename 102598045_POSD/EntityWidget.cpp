#include "EntityWidget.h"
#include <QGraphicsSceneMouseEvent>
#include <QPainter>

EntityWidget::EntityWidget(QRectF rect,QGraphicsItem* parent) : ComponentWidget(rect,parent){
}

EntityWidget::~EntityWidget(){
}

void EntityWidget::paint(QPainter* painter,const QStyleOptionGraphicsItem* option,QWidget* widget){
	painter->drawRect(this->boundingRect());
	painter->drawText(this->boundingRect(), Qt::AlignCenter,QString("test"));
}