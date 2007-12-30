/*
 *  Copyright (c) 2003-2006, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */
#ifndef GRFrame_H
#define GRFrame_H

#include <SprGraphics.h>
#include "IfStubGraphics.h"

namespace Spr{;

/**	@class	GRVisual
    @brief	 */
class GRVisual: public SceneObject, public GRVisualIfInit{
public:
	OBJECTDEF_ABST(GRVisual, SceneObject);
	virtual void Render(GRRenderIf* render){}
	virtual void Rendered(GRRenderIf* render){}
};

/**	@class	GRFrame
    @brief	�O���t�B�b�N�X�V�[���O���t�̃c���[�̃m�[�h ���W�n��\�� */
class GRFrame: public GRVisual, public GRFrameIfInit, public GRFrameDesc{
public:
	OBJECTDEF(GRFrame, GRVisual);
	ACCESS_DESC(GRFrame);
	GRFrame* parent;
	typedef std::vector< UTRef<GRVisualIf> > GRVisualIfs;
	GRVisualIfs children;
	GRFrame(const GRFrameDesc& desc=GRFrameDesc());

	GRSceneIf* GetScene(){return DCAST(GRSceneIf, GRVisual::GetScene());}
	
	virtual GRFrameIf* GetParent(){ return parent->Cast(); }
	virtual void SetParent(GRFrameIf* fr);
	virtual int NChildren(){ return (int)children.size(); }
	virtual GRVisualIf** GetChildren(){ return children.empty() ? NULL : (GRVisualIf**)&*children.begin(); }
	virtual bool AddChildObject(ObjectIf* v);
	virtual bool DelChildObject(ObjectIf* v);

	virtual void Render(GRRenderIf* r);
	virtual void Rendered(GRRenderIf* r);
	virtual size_t NChildObject() const;
	virtual ObjectIf* GetChildObject(size_t pos);
	virtual Affinef GetWorldTransform(){ if (parent) return parent->GetWorldTransform() * transform; return transform; }
	virtual Affinef GetTransform(){ return transform; }
	virtual void SetTransform(const Affinef& af){ transform = af; }
	void Print(std::ostream& os) const ;
};

class GRAnimation: public SceneObject, public GRAnimationIfInit, public GRAnimationDesc{
public:
	OBJECTDEF(GRAnimation, SceneObject);
	GRAnimation(GRAnimationDesc& d = GRAnimationDesc()){}
	///
	struct Target{
		GRFrameIf* target;
		Affinef initalTransform;
	};
	typedef std::vector<Target> Targets;
	///	�ϊ��Ώۃt���[��
	Targets targets;
	///	
	virtual void BlendPose(float time, float weight);
	///
	virtual void ResetPose();
	///
	virtual void LoadInitialPose();
	///	
	virtual bool AddChildObject(ObjectIf* v);
};

class GRAnimationSet: public SceneObject, public GRAnimationSetIfInit{
	typedef std::vector< UTRef<GRAnimation> > Animations;
	Animations animations;
	std::vector<GRFrame*> roots;

public:
	OBJECTDEF(GRAnimationSet, SceneObject);
	GRAnimationSet(GRAnimationSetDesc& d = GRAnimationSetDesc()){}
	///	�q�I�u�W�F�N�g(animations)��Ԃ�
	ObjectIf* GetChildObject(size_t p);
	///	GRAnimation�̒ǉ�
	virtual bool AddChildObject(ObjectIf* o);
	///	GRAnimation�̍폜
	virtual bool DelChildObject(ObjectIf* o);
	///	GRAnimation�̐�
	virtual int NChildObject();

	///	�w��̎����̕ϊ��ɏd�݂������āA�{�[��������킷�^�[�Q�b�g�̃t���[���ɓK�p����B
	virtual void BlendPose(float time, float weight);
	///	�{�[��������킷�^�[�Q�b�g�̃t���[���̍s��������l�ɖ߂��D
	virtual void ResetPose();
	///
	virtual void LoadInitialPose();
};

class GRAnimationController: public SceneObject, GRAnimationControllerIfInit{
public:
	typedef std::map<UTString, UTRef<GRAnimationSet>, UTStringLess> Sets;
	Sets sets;
	OBJECTDEF(GRAnimationController, SceneObject);
	GRAnimationController(const GRAnimationControllerDesc& d = GRAnimationControllerDesc()){}
	///	�w��̎����̕ϊ��ɏd�݂������āA�{�[��������킷�^�[�Q�b�g�̃t���[���ɓK�p����B
	virtual void BlendPose(UTString name, float time, float weight);
	///	�t���[���̕ϊ��s��������l�ɖ߂��D
	virtual void ResetPose();
	///	�t���[���̕ϊ��s��������l�ɖ߂��D
	virtual void LoadInitialPose();

	///	GRAnimation�̒ǉ�
	virtual bool AddChildObject(ObjectIf* o);
	///	GRAnimation�̍폜
	virtual bool DelChildObject(ObjectIf* o);
	///	GRAnimation�̐�
	virtual int NChildObject();
	///	GRAnimation�̎擾
	ObjectIf* GetChildObject(size_t p);
};

}//	namespace Spr
#endif
