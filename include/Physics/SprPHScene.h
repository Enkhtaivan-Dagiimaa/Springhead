/*
 *  Copyright (c) 2003-2006, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */

/** \addtogroup gpPhysics 	*/
//@{

/**
 *	@file SprPHScene.h
 *	@brief �V�[��
*/
#ifndef SPR_PHSCENEIF_H
#define SPR_PHSCENEIF_H

namespace Spr{;

struct PHSdkIf;

struct PHSolidIf;
struct PHSolidDesc;

struct CDShapeIf;
struct CDShapeDesc;

struct PHJointIf;
struct PHConstraintDesc;
typedef PHConstraintDesc PHJointDesc;

struct PHPathIf;
struct PHPathDesc;

/// �����G���W���̃V�[���̏��
struct PHSceneState{
	/// �ϕ��X�e�b�v
	double timeStep;
	/// �ϕ�������
	unsigned count;
	PHSceneState(){Init();}
	void Init(){
		timeStep = 0.005;
		count = 0;
	}
};
/// \ingroup gpPhysicsNode
/// �����G���W���̃V�[���̃f�X�N���v�^
struct PHSceneDesc: public PHSceneState{
	///	�ڐG�̃��[�h
	enum ContactMode{
		MODE_NONE,		///< ��������
		MODE_PENALTY,	///< �y�i���e�B�@�ŉ���
		MODE_LCP		///< LCP�ŉ���
	};
	
	/// �d�͉����x�x�N�g���D�f�t�H���g�l��(0.0f, -9.8f, 0.0f)�D
	Vec3f gravity;

	PHSceneDesc(){Init();}
	void Init(){
		gravity=Vec3f(0,-9.8f,0);
	}
};

///	�V�[��
struct PHSceneIf : public SceneIf{
public:
	IF_DEF(PHScene);

	/** @brief ����Scene������SDK��Ԃ�
		@return SDK�̃C���^�t�F�[�X
	 */
	virtual PHSdkIf* GetSdk()=0;

	/** @brief Shape���쐬����
		@param desc �쐬����Shape�̃f�B�X�N���v�^
		@return Shape�̃C���^�t�F�[�X
	 */
	virtual CDShapeIf* CreateShape(const CDShapeDesc& desc)=0;
	
	/** @brief Solid���쐬����
		@return Solid�̃C���^�t�F�[�X

		PHSolidDesc�̃f�t�H���g�l��p����Solid���쐬����D
	 */
	virtual PHSolidIf* CreateSolid()=0;
	
	/** @brief Solid���쐬����
		@param desc �쐬����Solid�̃f�B�X�N���v�^
		@return Solid�̃C���^�t�F�[�X
	*/
	virtual PHSolidIf* CreateSolid(const PHSolidDesc& desc)=0;
	
	/** @brief Solid�̐����擾����
		@return Solid�̐�
	 */
	virtual int NSolids()=0;

	/** @brief Solid���擾����
		@return Solid�̃C���^�t�F�[�X�̔z��ւ̃|�C���^
	 */
	virtual PHSolidIf** GetSolids()=0;

	/** @brief �w�肵�����̓��m�̐ڐG�̃��[�h��ݒ肷��
		@param lhs �g�̕Е��̍��̂ւ̃|�C���^
		@param rhs �g�̂����Е��̍��̂ւ̃|�C���^
		@param mode �ڐG�̃��[�h

		����lhs�ƍ���rhs�Ƃ̐ڐG�̃��[�h��mode�ɐݒ肷��D
	  */
	virtual void SetContactMode(PHSolidIf* lhs,	PHSolidIf* rhs, PHSceneDesc::ContactMode mode = PHSceneDesc::MODE_LCP)=0;

	/** @brief �w�肵���W���Ɋ܂܂��S�Ă̍��̓��m�̐ڐG�̃��[�h��ݒ肷��
		@param group ���̂ւ̃|�C���^�z��̐擪�A�h���X
		@param size �|�C���^�z��̒���
		@param mode �ڐG�̃��[�h

		group[0]����group[size-1]�܂ł̑S�Ă̍��̂̑g�̐ڐG�̃��[�h��mode�ɐݒ肷��D
	 */
	virtual void SetContactMode(PHSolidIf** group, size_t length, PHSceneDesc::ContactMode mode = PHSceneDesc::MODE_LCP)=0;

	/** @brief �w�肵�����̂Ƒ��̍��̂Ƃ̑S�Ă̐ڐG�̃��[�h��ݒ肷��
		@param mode �ڐG�̃��[�h
	 */
	virtual void SetContactMode(PHSolidIf* solid, PHSceneDesc::ContactMode mode = PHSceneDesc::MODE_LCP)=0;

	/** @brief �V�[�����ۗL����S�Ă̍��̓��m�̐ڐG�̃��[�h��ݒ肷��
		@param mode �ڐG�̃��[�h
	 */
	virtual void SetContactMode(PHSceneDesc::ContactMode mode = PHSceneDesc::MODE_LCP)=0;

	/** @brief �֐߂��쐬����
		@param desc �֐߂̃f�B�X�N���v�^
		@return �֐߂̃C���^�t�F�[�X
		Solid lhs��Solid rhs��A������֐߂��쐬����D
	 */
	virtual PHJointIf* CreateJoint(PHSolidIf* lhs, PHSolidIf* rhs, const PHJointDesc& desc)=0;

	/** @brief �p�X���쐬����
		@param desc �p�X�̃f�B�X�N���v�^
	 */
	virtual PHPathIf* CreatePath(const PHPathDesc& desc)=0;

	/** @brief �ϕ��X�e�b�v���擾����
		@return �ϕ��X�e�b�v
	 */
	virtual double GetTimeStep()const=0;
	
	/** @brief �ϕ��X�e�b�v��ݒ肷��
		@param dt �ϕ��X�e�b�v
	 */
	virtual void SetTimeStep(double dt)=0;
	
	/** @brief �J�E���g�����擾����
		@return �J�E���g��
		�V�[�����쐬����Ă��猻�݂܂ł�Step()�����s�����񐔂�Ԃ��D
	 */
	virtual unsigned GetCount()const=0;

	/** @brief �J�E���g����ݒ肷��
		@param count �J�E���g��
	 */
	virtual void SetCount(unsigned count)=0;
	
	/** @brief �V�[���̎�����i�߂�
	 */
	virtual void Step()=0;

	/** @brief �V�[������ɂ���
	 */
	virtual void Clear()=0;

	/** @brief �d�͂�ݒ肷��
		@param accel �d�͉����x�x�N�g��
	 */
	//�����Ɏ�������ׂ����v���������CNovodex�͂��� 
	virtual void SetGravity(Vec3f accel)=0;
	
	/** @brief �d�͂��擾����
		@return �d�͉����x�x�N�g��
	 */
	virtual Vec3f GetGravity()=0;

};

//@}

}	//	namespace Spr
#endif
