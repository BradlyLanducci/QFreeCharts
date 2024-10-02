#pragma once

#include "RotationShader.h"

#include <QSGMaterial>

//----------------------------------------------------------------------------------------------------------------------------------//

using RGBA = QVector4D;

//----------------------------------------------------------------------------------------------------------------------------------//

class RotationMaterial : public QSGMaterial
{
public:
	RotationMaterial();

	QSGMaterialType* type() const override;
	int compare(const QSGMaterial* other) const override;

	RotationShader* createShader(QSGRendererInterface::RenderMode) const override
	{
		return new RotationShader();
	}

	void setColor(RGBA rgba);

	struct
	{
		QVector4D color;
		bool dirty;
	} uniforms;
};

//----------------------------------------------------------------------------------------------------------------------------------//
