/*
 *  Copyright (c) 2003-2008, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */
#ifndef GRFrame_H
#define GRFrame_H

#include <SprGraphics.h>

namespace Spr{;

/**	@class	GRVisual
    @brief	 */
class GRVisual: public SceneObject{
public:
	SPR_OBJECTDEF_ABST(GRVisual);
	virtual void Render(GRRenderIf* render){}
	virtual void Rendered(GRRenderIf* render){}
};

/**	@class	GRFrame
    @brief	�O���t�B�b�N�X�V�[���O���t�̃c���[�̃m�[�h ���W�n��\�� */
class GRFrame: public GRVisual, public GRFrameDesc{
public:
	SPR_OBJECTDEF(GRFrame);
	ACCESS_DESC(GRFrame);
	GRFrame* parent;
	typedef std::vector< UTRef<GRVisualIf> > GRVisualIfs;
	GRVisualIfs children;

	GRFrame(const GRFrameDesc& desc=GRFrameDesc());

	GRSceneIf* GetScene(){return DCAST(GRSceneIf, GRVisual::GetScene());}
	
	virtual SceneObjectIf* CloneObject(); 
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
	virtual void Clear(){children.clear();}
	void Print(std::ostream& os) const ;

	// Keyframe Blending based on Radial Basis Function 
	std::vector< PTM::VVector<float> > kfPositions;
	PTM::VVector<float> kfAffines[4][4], kfCoeffs[4][4];
	void AddRBFKeyFrame(PTM::VVector<float> pos);
	void BlendRBF(PTM::VVector<float> pos);
};

/**	@class	GRDummyFrame
    @brief	�\�����Ȃ��R���e�i�DVisual�����܂��Ă����āC��Ńv���O��������g�����߂Ɏg���@*/
class GRDummyFrame: public GRVisual, public GRDummyFrameDesc{
public:
	SPR_OBJECTDEF(GRDummyFrame);
	ACCESS_DESC(GRDummyFrame);
	typedef std::vector< UTRef<GRVisualIf> > GRVisualIfs;
	GRVisualIfs children;
	GRDummyFrame(const GRDummyFrameDesc& desc=GRDummyFrameDesc());
	GRSceneIf* GetScene(){return DCAST(GRSceneIf, GRVisual::GetScene());}
	
	virtual bool AddChildObject(ObjectIf* v);
	virtual bool DelChildObject(ObjectIf* v);
	virtual size_t NChildObject() const;
	virtual ObjectIf* GetChildObject(size_t pos);
};

class GRAnimation: public SceneObject, public GRAnimationDesc{
public:
	SPR_OBJECTDEF(GRAnimation);
	GRAnimation(const GRAnimationDesc& d = GRAnimationDesc()){}
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
	virtual void BlendPose(float time, float weight, bool add);
	///
	virtual void ResetPose();
	///
	virtual void LoadInitialPose();
	///	
	virtual bool AddChildObject(ObjectIf* v);
};

class GRAnimationSet: public SceneObject{
	typedef std::vector< UTRef<GRAnimation> > Animations;
	Animations animations;
	std::vector<GRFrame*> roots;

public:
	SPR_OBJECTDEF(GRAnimationSet);
	GRAnimationSet(const GRAnimationSetDesc& d = GRAnimationSetDesc()){}
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
	///	�w��̎����̕ϊ��ɏd�݂������āA�{�[��������킷�^�[�Q�b�g�̃t���[���ɓK�p����B
	virtual void BlendPose(float time, float weight, bool add);
	///	�{�[��������킷�^�[�Q�b�g�̃t���[���̍s��������l�ɖ߂��D
	virtual void ResetPose();
	///
	virtual void LoadInitialPose();
};

class GRAnimationController: public SceneObject{
public:
	typedef std::map<UTString, UTRef<GRAnimationSet>, UTStringLess> Sets;
	Sets sets;
	float buffer[16];
	SPR_OBJECTDEF(GRAnimationController);
	GRAnimationController(const GRAnimationControllerDesc& d = GRAnimationControllerDesc()){}
	///	�w��̎����̕ϊ��ɏd�݂������āA�{�[��������킷�^�[�Q�b�g�̃t���[���ɓK�p����B
	virtual void BlendPose(UTString name, float time, float weight);
	///	�w��̎����̕ϊ��ɏd�݂������āA�{�[��������킷�^�[�Q�b�g�̃t���[���ɓK�p����B
	virtual void BlendPose(UTString name, float time, float weight, bool add);
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
