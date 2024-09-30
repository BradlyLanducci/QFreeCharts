#pragma once

#include "RotationShader.h"

#include <QSGMaterial>

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

	struct
	{
		bool dirty;
	} uniforms;
};

//----------------------------------------------------------------------------------------------------------------------------------//
