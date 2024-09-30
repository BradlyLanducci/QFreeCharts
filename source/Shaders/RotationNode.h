#pragma once

#include <QSGGeometryNode>

//----------------------------------------------------------------------------------------------------------------------------------//

class RotationNode : public QSGGeometryNode
{
public:
	RotationNode();

	void setRect(const QRectF& bounds);
	void setGeometry(QSGGeometry* geometry);
};

//----------------------------------------------------------------------------------------------------------------------------------//
