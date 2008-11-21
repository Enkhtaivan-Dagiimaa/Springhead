/*
 *  Copyright (c) 2003-2008, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */
/**
 *	@file SprPHSolid.h
 *	@brief ����
*/
#ifndef SPR_PHSolidIf_H
#define SPR_PHSolidIf_H
#include <Foundation/SprObject.h>

/**	\addtogroup gpPhysics	*/
//@{
namespace Spr{;

///	�`�ƈʒu���w�肷�邽�߂̏Փ˔���p�t���[��
struct PHFrameDesc{
	Posed pose;
};
struct PHFrameIf: public SceneObjectIf{
	SPR_IFDEF(PHFrame);
	Posed GetPose();
	void SetPose(Posed p);
};



///	���̂̃X�e�[�g
struct PHSolidState{
	Vec3d		velocity;		///<	���x			(World�n)
	Vec3d		angVelocity;	///<	�p���x			(World�n)
	Posed		pose;			///<	�ʒu�ƌ���		(World�n)
	Vec3d		force;			///<	�O�̐ϕ��ł��̍��̂ɉ��������(World�n)
	Vec3d		torque;			///<	�O�̐ϕ��ł��̍��̂ɉ�������g���N(World�n)
	Vec3d		nextForce;		///<	���̐ϕ��ł��̍��̂ɉ�����(World�n)
	Vec3d		nextTorque;		///<	���̐ϕ��ł��̍��̂ɉ����g���N(World�n)
};
///	���̂̃f�B�X�N���v�^
struct PHSolidDesc: public PHSolidState{
	double		mass;			///<	����
	Matrix3d	inertia;		///<	�����e���\��	(Local�n)
	Vec3d		center;			///<	���ʒ��S�̈ʒu	(Local�n)
	bool		dynamical;		///<	�����@���ɏ]����

	PHSolidDesc(){ Init(); }
	void Init(){
		mass = 1.0f;
		inertia = Matrix3d::Unit();
		dynamical = true;
	}
};

struct CDShapeIf; 
///	���̂̃C���^�t�F�[�X
struct PHSolidIf : public SceneObjectIf{
	SPR_IFDEF(PHSolid);

	/** @brief �͂����ʒ��S�ɉ�����
		@param f ��
	 */
	void AddForce(Vec3d f);
	
	/** @brief �g���N��������
		@param t �g���N
	 */
	void AddTorque(Vec3d t);
	
	/**	@brief �͂��w�肵����p�_�ɉ�����
		@param f ��
		@param r �V�[�����W�n�ŕ\�킵����p�_�̈ʒu
	 */
	void AddForce(Vec3d f, Vec3d r);

	/**	@brief ���ʂ��擾����
		@return ����
	 */
	double		GetMass();
	
	/** @brief ���ʂ̋t�����擾����
		@return ���ʂ̋t��
	 */
	double		GetMassInv();
	
	/** @brief ���ʂ�ݒ肷��
		@param m ����
	 */
	void		SetMass(double m);
	
	/** @brief ���ʒ��S���擾����
		@return ���ʒ��S�̈ʒu
	 */
	Vec3d		GetCenterOfMass();
	
	/** @brief ���ʒ��S��ݒ肷��
		@param center ���ʒ��S�̈ʒu
	 */
	void		SetCenterOfMass(const Vec3d& center);
	
	/** @brief �����e���\�����擾����
		@return �����e���\��
	 */
	Matrix3d	GetInertia();
	
	/** @brief �����e���\���̋t�����擾����
		@return �����e���\���̋t��
	 */
	Matrix3d	GetInertiaInv();
	
	/** @brief �����e���\����ݒ肷��
		@param I �����e���\��
	 */
	void		SetInertia(const Matrix3d& I);

	/** @brief �`��Ɩ��x����d�S�C���ʁC�����e���\���������I�ɐݒ肷��
	 */
	void		CompInertia();
	
	/** @brief ���̂̈ʒu���擾����
		@return �V�[���ɑ΂��鍄�̃t���[�����_�̈ʒu
	 */
	Vec3d		GetFramePosition() const;
	
	/** @brief ���̂̈ʒu��ݒ肷��
		@param p �V�[���ɑ΂��鍄�̃t���[�����_�̈ʒu
	 */
	void		SetFramePosition(const Vec3d& p);
	
	/** @brief ���̂̎��ʒ��S�̈ʒu���擾����
		@return �V�[���ɑ΂��鍄�̂̎��ʒ��S�̈ʒu
	 */
	Vec3d		GetCenterPosition() const ;
	
	/** @brief ���̂̈ʒu��ݒ肷��
		@param p �V�[���ɑ΂��鍄�̂̎��ʒ��S�̈ʒu
	 */
	void		SetCenterPosition(const Vec3d& p);
	
	/** @brief ���̂̑O�̃X�e�b�v����̕��i�ړ��ʂ�Ԃ�
		@return �O�̃X�e�b�v����̕��i�ړ���
	 */
	Vec3d GetDeltaPosition(const Vec3d& p) const;

	/** @brief ���̂̑O�̃X�e�b�v����̕��i�ړ��ʂ�Ԃ��D
		@param pos �d�S����̑��Έʒu�D���̈ʒu�̕��i�ړ��ʂ�Ԃ��D
		@return �O�̃X�e�b�v����̕��i�ړ���
	*/
	Vec3d GetDeltaPosition() const;

	/** @brief ���̂̌������擾����
		@return �V�[���ɑ΂��鍄�̂̌���
	 */
	Quaterniond GetOrientation() const;
	
	/** @brief ���̂̌�����ݒ肷��
		@param q �V�[���ɑ΂��鍄�̂̌���
	 */
	void		SetOrientation(const Quaterniond& q);

	/** @brief ���̂̈ʒu�ƌ������擾����
		@return �V�[���ɑ΂��鍄�̂̈ʒu�ƌ���
	 */
	Posed		GetPose() const;
	
	/** @brief ���̂̈ʒu���擾����
		@return �V�[���ɑ΂��鍄�̂̈ʒu
	*/
	Vec3d		GetPoseR() const;

	/** @brief	���̂̌������擾����
	*/
	Quaterniond GetPoseQ() const;

	/** @brief ���̂̈ʒu�ƌ�����ݒ肷��
		@param p �V�[���ɑ΂��鍄�̂̈ʒu�ƌ���
	 */
	void		SetPose(const Posed& p);

	/** @brief ���̂̑��x���擾����
		@return �V�[���ɑ΂��鍄�̂̎��ʒ��S�̑��x
	 */
	Vec3d		GetVelocity() const ;

	/** @brief ���̂̑��x��ݒ肷��
		@param v �V�[���ɑ΂��鍄�̂̎��ʒ��S�̑��x
	 */
	void		SetVelocity(const Vec3d& v);
	/** @brief ���̂̊p���x���擾����
		@return �V�[���ɑ΂��鍄�̂̊p���x
	 */
	Vec3d		GetAngularVelocity() const;
	
    /** @brief ���̂̊p���x��ݒ肷��
		@param av �V�[���ɑ΂��鍄�̂̊p���x
	 */
	void		SetAngularVelocity(const Vec3d& av);
    /** @brief �Ō�̃X�e�b�v�ō��̂ɉ�������͂��擾����B
		@return ���̂ɉ�����ꂽ��(World�n)
	*/
	Vec3d GetForce() const;
    /** @brief �Ō�̃X�e�b�v�ō��̂ɉ�������g���N���擾����B
		@return ���̂ɉ�����ꂽ�g���N(World�n�A���̂̏d�S����)
	*/
	Vec3d GetTorque() const;
	/** @brief ���̂Ɍ`���o�^����
		@param shape �`��ւ̃|�C���^
	 */
	void		AddShape(CDShapeIf* shape);
	/** @brief �o�^����Ă���`��̌����擾����
		@return �`��̌�
	 */
	int			NShape();
	/**	@brief �o�^����Ă���`����擾����
		@param index �`��̃C���f�b�N�X
		@return �`��ւ̃|�C���^
		�ŏ��ɓo�^���ꂽ�`��̃C���f�b�N�X��0�C���̎��ɓo�^���ꂽ�`��̃C���f�b�N�X��1�C
		�ȉ����l�D
	 */
	CDShapeIf*	GetShape(int index);
	/** @brief �`��̈ʒu�ƌ������擾����
		@param index �ΏۂƂ���`��̃C���f�b�N�X
		@return ���̂ɑ΂���`��̈ʒu�ƌ���
		�C���f�b�N�X�ɂ��Ă�GetShape���Q�ƁD
	 */
	Posed		GetShapePose(int index);
	/** @brief �`��̈ʒu�ƌ�����ݒ肷��
		@param index �ΏۂƂ���`��̃C���f�b�N�X
		@param pose ���̂ɑ΂���`��̈ʒu�ƌ���
		�C���f�b�N�X�ɂ��Ă�GetShape���Q�ƁD
	 */
	void		SetShapePose(int index, const Posed& pose);
	
	/** @brief �d�͂�L��/����������
		@param bOn true�Ȃ�΍��̂ɏd�͂������Dfalse�Ȃ�Ή����Ȃ��D
	 */
	void		SetGravity(bool bOn);
	/** @brief �����@���ɏ]�����ǂ�����L��/����������
		@param bOn true�Ȃ�΍��͕̂����@���ɂ��������ĉ^������D
		�����@���ɏ]��Ȃ����̂��CSetPose�CSetVelocity�Ȃǂ̊֐��ɂ��ʒu�⑬�x�̍X�V��
		�\�ł��D
	 */
	void		SetDynamical(bool bOn);
	/** @brief �����@���ɏ]�����ǂ������擾����
		@return true�Ȃ�΍��͕̂����@���ɂ��������ĉ^������D
	 */
	bool		IsDynamical();
	/** @brief �ʒu�C���x���X�V���邩�ǂ�����L��/����������
		@param bOn true�Ȃ�Έʒu�C���x�̍X�V�𖳌�������D
	 */
	void		SetFrozen(bool bOn);
	/** @brief �ʒu�C���x���X�V���邩�ǂ������擾����
		@param bOn true�Ȃ�Έʒu�C���x�̍X�V�𖳌�������D
	*/	
	bool		IsFrozen();
};

struct PHShapePairForLCPIf : public ObjectIf{
	SPR_IFDEF(PHShapePairForLCP);

	/** @brief section�̐����擾����
	*/
	int NSectionVertexes();

	/** @brief i�Ԗڂ̌����f�ʂ̒��_���W���擾����
		@param i:���_���W�̔ԍ��D
	*/
	Vec3d GetSectionVertex(int i);
};

struct PHSolidPairForLCPIf : public ObjectIf{
	SPR_IFDEF(PHSolidPairForLCP);
	
	PHSolidIf* GetSolid(int i);

	/** @brief ���̓��m�̐ڐG��Ԃ��擾����
		@param i,j:Shape�̔ԍ��D
		������FindBySolidPair�ƃZ�b�g�Ŏg������.
	*/
	int	GetContactState(int i, int j);

	/** @brief ���̓��m�̐ڐG���W�i���_�j���擾����
		@param i,j:Shape�̔ԍ��D
		������FindBySolidPair�ƃZ�b�g�Ŏg������.
	*/
	Vec3d GetCommonPoint(int i, int j);

	/** @brief �Ō�ɐڐG�����Ƃ��̃J�E���g���擾����
		@param i,j:Shape�̔ԍ��D
		������scene.count�Ɣ�r����ۂɈ��̃Y��������̂Œ���.
	*/
	unsigned GetLastContactCount(int i, int j);

	/** @brief �ڐG�����Ƃ��̂߂荞�ݗʂ��擾����
		@param i,j:Shape�̔ԍ��D
	*/
	double GetContactDepth(int i, int j);

	/** @brief �ڐG�����Ƃ��̌`��y�A���擾����
		@param i,j:Shape�̔ԍ��D
	*/
	PHShapePairForLCPIf* GetShapePair(int i, int j);
};


//@}

}	//	namespace Spr
#endif
