#include "RotationMaterial.h"

//----------------------------------------------------------------------------------------------------------------------------------//

RotationMaterial::RotationMaterial()
{
}

//----------------------------------------------------------------------------------------------------------------------------------//

QSGMaterialType* RotationMaterial::type() const
{
	static QSGMaterialType type;
	return &type;
}

//----------------------------------------------------------------------------------------------------------------------------------//

int RotationMaterial::compare(const QSGMaterial* o) const
{
	Q_ASSERT(o && type() == o->type());
	const auto* other = static_cast<const RotationMaterial*>(o);
	return other == this ? 0 : 1; // ### TODO: compare state???
}

//----------------------------------------------------------------------------------------------------------------------------------//
