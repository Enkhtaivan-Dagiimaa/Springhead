/**	\addtogroup	gpGraphics	*/
//@{

#ifndef SPR_GRMesh_H
#define SPR_GRMesh_H

#include <Graphics/SprGRFrame.h>
#include <Springhead.h>

namespace Spr{;

///	@brief �\���p��Mesh(GRMesh)�̃f�X�N���v�^�D
struct GRMeshDesc{
	std::vector<Vec3f> vertices;		///<	���_�̍��W
	std::vector<Vec3f> normals;			///<	���_�̖@��
	std::vector<int> faces;				///<	�ʂ��\�����钸�_�̔ԍ��D
};


/**	@brief	�O���t�B�b�N�X�Ŏg���\���p��Mesh */
struct GRMeshIf: public GRVisualIf{
	IF_DEF(GRMesh);
};


//@}
}
#endif
