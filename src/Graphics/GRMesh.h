#ifndef GRMesh_H
#define GRMesh_H

#include <SprGraphics.h>
#include "GRFrame.h"

namespace Spr{;

/**	@class	GRMesh
    @brief	�O���t�B�b�N�X�V�[���O���t�ł̍��W�n��\���D */
class GRMesh: public InheritGRVisual<GRMeshIf, GRVisual>, public GRMeshDesc{
public:
	OBJECT_DEF(GRMesh);
	void Render(GRRenderIf* r);
	void Rendered(GRRenderIf* r);
};
}
#endif
