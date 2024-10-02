#include "RotationShader.h"
#include "RotationMaterial.h"

//----------------------------------------------------------------------------------------------------------------------------------//

RotationShader::RotationShader()
{
	setShaderFileName(VertexStage, QLatin1String(":/Shaders/Rotation.vert.qsb"));
	setShaderFileName(FragmentStage, QLatin1String(":/Shaders/Rotation.frag.qsb"));
}

//----------------------------------------------------------------------------------------------------------------------------------//

bool RotationShader::updateUniformData(RenderState& state, QSGMaterial* newMaterial, QSGMaterial* oldMaterial)
{
	bool changed = false;
	QByteArray* buf = state.uniformData();
	Q_ASSERT(buf->size() >= 64);

	if (state.isMatrixDirty())
	{
		const QMatrix4x4 m = state.combinedMatrix();

		memcpy(buf->data(), m.constData(), 64);
		changed = true;
	}

	RotationMaterial* rotationMaterial = static_cast<RotationMaterial*>(newMaterial);
	if (oldMaterial != newMaterial || rotationMaterial->uniforms.dirty)
	{
		memcpy(buf->data() + 64, &rotationMaterial->uniforms.color, 16);
		rotationMaterial->uniforms.dirty = false;
		changed = true;
	}

	return changed;
}

//----------------------------------------------------------------------------------------------------------------------------------//
