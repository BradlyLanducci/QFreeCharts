#pragma once

#include "RotationMaterial.h"

#include <QSGGeometryNode>

//----------------------------------------------------------------------------------------------------------------------------------//

class RotationNode : public QSGGeometryNode
{
public:
	RotationNode(RGBA color);

	void setRect(const QRectF& bounds);
	void setGeometry(QSGGeometry* geometry);
	void setColor(const QColor color);
};

//----------------------------------------------------------------------------------------------------------------------------------//
