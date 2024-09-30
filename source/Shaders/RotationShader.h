#pragma once

#include <QSGMaterialShader>

//----------------------------------------------------------------------------------------------------------------------------------//

class RotationShader : public QSGMaterialShader
{
public:
	RotationShader();

	bool updateUniformData(RenderState& state, QSGMaterial* newMaterial, QSGMaterial* oldMaterial) override;
};

//----------------------------------------------------------------------------------------------------------------------------------//
