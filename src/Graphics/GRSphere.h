/*
 *  Copyright (c) 2003-2008, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */
#ifndef GRSphere_H
#define GRSphere_H

#include <Graphics/SprGRSphere.h>
#include <Graphics/GRFrame.h>

namespace Spr{;

class GRMaterial;
/**	@class	GRSphere
    @brief	�O���t�B�b�N�X�V�[���O���t�ł̋��I�u�W�F�N�g��\���D */
class GRSphere: public GRVisual, public GRSphereDesc{
	unsigned int list;				///< �f�B�X�v���C���X�g�̎��ʎq
	GRRenderIf* render;
	
	/// �f�B�X�v���C���X�g���쐬�D
	void CreateList(GRRenderIf* r);
public:
	SPR_OBJECTDEF(GRSphere);
	ACCESS_DESC(GRSphere);
	
	UTRef<GRMaterial> material;		///< �}�e���A��
	
	GRSphere(const GRSphereDesc& desc=GRSphereDesc());
	~GRSphere();
	void Render(GRRenderIf* r);
	void Rendered(GRRenderIf* r);
	virtual bool AddChildObject(ObjectIf* o);

	void SetRender(GRRenderIf* r) {render = r;}
};
}
#endif
