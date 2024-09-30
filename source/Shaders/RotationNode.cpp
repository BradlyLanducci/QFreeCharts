#include "RotationNode.h"

#include "RotationMaterial.h"

//----------------------------------------------------------------------------------------------------------------------------------//

RotationNode::RotationNode()
{
	auto* m = new RotationMaterial();
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
