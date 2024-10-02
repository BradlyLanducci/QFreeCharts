#include "RotationNode.h"

//----------------------------------------------------------------------------------------------------------------------------------//

RotationNode::RotationNode(RGBA color)
{
	auto* m = new RotationMaterial();
	m->setColor(color);
	setMaterial(m);
	setFlag(OwnsMaterial, true);

	QSGGeometry* g = new QSGGeometry(QSGGeometry::defaultAttributes_TexturedPoint2D(), 4);
	QSGGeometry::updateTexturedRectGeometry(g, QRect(), QRect());
	g->setLineWidth(1); // Graphics API enforces line width of 1
	g->setDrawingMode(QSGGeometry::DrawTriangles);
	setGeometry(g);
	setFlag(OwnsGeometry, true);
}

//----------------------------------------------------------------------------------------------------------------------------------//

void RotationNode::setRect(const QRectF& bounds)
{
	QSGGeometry::updateTexturedRectGeometry(geometry(), bounds, QRectF(0, 0, 1, 1));
	markDirty(QSGNode::DirtyGeometry);
}

//----------------------------------------------------------------------------------------------------------------------------------//

void RotationNode::setGeometry(QSGGeometry* g)
{
	QSGGeometryNode::setGeometry(g);

	markDirty(QSGNode::DirtyGeometry);
}

//----------------------------------------------------------------------------------------------------------------------------------//

void RotationNode::setColor(const QColor color)
{
	RGBA colorVector4d = QVector4D(color.redF(), color.greenF(), color.blueF(), color.alphaF());
	RotationMaterial* m = static_cast<RotationMaterial*>(material());
	m->setColor(colorVector4d);
}

//----------------------------------------------------------------------------------------------------------------------------------//
