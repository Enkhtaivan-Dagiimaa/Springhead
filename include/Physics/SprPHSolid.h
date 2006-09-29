/*
 *  Copyright (c) 2003-2006, Shoichi Hasegawa and Springhead development team 
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
///	���̂̃X�e�[�g
struct PHSolidState{
	Vec3d		velocity;		///<	���x			(World�n)
	Vec3d		oldVel;		///<	�P�X�e�b�v�O�̑��x
	Vec3d		angVelocity;	///<	�p���x			(World�n)
	Vec3d		oldAngVel;		///<	�P�X�e�b�v�O�̊p���x
	Vec3d		acceleration;	///<	�����x			(World�n)
	Vec3d		angAcceleration;///<	�p�����x		(World�n)
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
	bool		gravity;		///<	�d�͂̉e�����󂯂邩

	PHSolidDesc(){ Init(); }
	void Init(){
		mass = 1.0f;
		inertia=Matrix3d::Unit();
		gravity=true;
	}
};

struct CDShapeIf; 
///	���̂̃C���^�t�F�[�X
struct PHSolidIf : public SceneObjectIf{
	IF_DEF(PHSolid);

	/** @brief �͂����ʒ��S�ɉ�����
		@param f ��
	 */
	virtual void AddForce(Vec3d f)=0;
	/** @brief �g���N��������
		@param t �g���N
	 */
	virtual void AddTorque(Vec3d t)=0;
	/**	@brief �͂��w�肵����p�_�ɉ�����
		@param f ��
		@param r �V�[�����W�n�ŕ\�킵����p�_�̈ʒu
	 */
	virtual void AddForce(Vec3d f, Vec3d r)=0;

	/**	@brief ���ʂ��擾����
		@return ����
	 */
	virtual double		GetMass()=0;
	/** @brief ���ʂ̋t�����擾����
		@return ���ʂ̋t��
	 */
	virtual double		GetMassInv()=0;
	/** @brief ���ʂ�ݒ肷��
		@param m ����
	 */
	virtual void		SetMass(double m)=0;
	/** @brief ���ʒ��S���擾����
		@return ���ʒ��S�̈ʒu
	 */
	virtual Vec3d		GetCenterOfMass()=0;
	/** @brief ���ʒ��S��ݒ肷��
		@param center ���ʒ��S�̈ʒu
	 */
	virtual void		SetCenterOfMass(const Vec3d& center)=0;
	/** @brief �����e���\�����擾����
		@return �����e���\��
	 */
	virtual Matrix3d	GetInertia()=0;
	/** @brief �����e���\���̋t�����擾����
		@return �����e���\���̋t��
	 */
	virtual Matrix3d	GetInertiaInv()=0;
	/** @brief �����e���\����ݒ肷��
		@param I �����e���\��
	 */
	virtual void		SetInertia(const Matrix3d& I)=0;
	/** @brief ���̂̈ʒu���擾����
		@return �V�[���ɑ΂��鍄�̃t���[�����_�̈ʒu
	 */
	virtual Vec3d		GetFramePosition() const =0;
	/** @brief ���̂̈ʒu��ݒ肷��
		@param p �V�[���ɑ΂��鍄�̃t���[�����_�̈ʒu
	 */
	virtual void		SetFramePosition(const Vec3d& p) =0;
	/** @brief ���̂̎��ʒ��S�̈ʒu���擾����
		@return �V�[���ɑ΂��鍄�̂̎��ʒ��S�̈ʒu
	 */
	virtual Vec3d		GetCenterPosition() const  =0;
	/** @brief ���̂̈ʒu��ݒ肷��
		@param p �V�[���ɑ΂��鍄�̂̎��ʒ��S�̈ʒu
	 */
	virtual void		SetCenterPosition(const Vec3d& p) =0;

	/** @brief ���̂̌X�����擾����
		@return �V�[���ɑ΂��鍄�̂̌X��
	 */
	virtual Quaterniond GetOrientation() const =0;
	/** @brief ���̂̌X����ݒ肷��
		@param q �V�[���ɑ΂��鍄�̂̌X��
	 */
	virtual void		SetOrientation(const Quaterniond& q) =0;

	/** @brief ���̂̈ʒu�ƌX�����擾����
		@return �V�[���ɑ΂��鍄�̂̈ʒu�ƌX��
	 */
	virtual Posed		GetPose() const = 0;
	/** @brief ���̂̈ʒu�ƌX����ݒ肷��
		@param p �V�[���ɑ΂��鍄�̂̈ʒu�ƌX��
	 */
	virtual void		SetPose(const Posed& p) = 0;

	/** @brief ���̂̑��x���擾����
		@return �V�[���ɑ΂��鍄�̂̎��ʒ��S�̑��x
	 */
	virtual Vec3d		GetVelocity() const  =0;

	/** @brief ���̂̂P�X�e�b�v�O�̑��x���擾����
		@return �V�[���ɑ΂��鍄�̂̂P�X�e�b�v�O�̑��x
	 */
	virtual Vec3d		GetOldVelocity() const = 0;
	/** @brief ���̂̑��x��ݒ肷��
		@param v �V�[���ɑ΂��鍄�̂̎��ʒ��S�̑��x
	 */
	virtual void		SetVelocity(const Vec3d& v) =0;
	/**	@brief ���̂̉����x���擾����
		@return ���̂̑��x���狁�܂�����x
	*/
	virtual Vec3d		GetAcceleration() const = 0;
	/**	@brief ���̂̉����x��ݒ肷��
		@param accel �V�[���ɑ΂��鍄�̂̉����x
	*/
	virtual void		SetAcceleration(const Vec3d& accel) = 0;
	/** @brief ���̂̊p���x���擾����
		@return �V�[���ɑ΂��鍄�̂̊p���x
	 */
	virtual Vec3d		GetAngularVelocity() const =0;
	
	/** @brief ���̂̂P�X�e�b�v�O�̊p���x���擾����
		@return �V�[���ɑ΂��鍄�̂̂P�X�e�b�v�O�̊p���x
	 */
	virtual Vec3d		GetOldAngularVelocity() const = 0;

    /** @brief ���̂̊p���x��ݒ肷��
		@param av �V�[���ɑ΂��鍄�̂̊p���x
	 */
	virtual void		SetAngularVelocity(const Vec3d& av)=0;
    /** @brief �Ō�̃X�e�b�v�ō��̂ɉ�������͂��擾����B
		@return ���̂ɉ�����ꂽ��(World�n)
	*/
	virtual Vec3d GetForce() const=0;
    /** @brief �Ō�̃X�e�b�v�ō��̂ɉ�������g���N���擾����B
		@return ���̂ɉ�����ꂽ�g���N(World�n�A���̂̏d�S����)
	*/
	virtual Vec3d GetTorque() const=0;
	/**	@brief ���̂̊p�����x���擾����
		@return ���̂̊p���x���狁�܂�p�����x
	*/
	virtual Vec3d		GetAngularAcceleration() const = 0;
	/**	@brief ���̂̊p�����x��ݒ肷��
		@param angAccel �V�[���ɑ΂��鍄�̂̊p�����x
	*/
	virtual void		SetAngularAcceleration(const Vec3d& angAccel) = 0;
	/** @brief ���̂Ɍ`���o�^����
		@param shape �`��ւ̃|�C���^
	 */
	virtual void		AddShape(CDShapeIf* shape)=0;
	/** @brief �o�^����Ă���`��̌����擾����
		@return �`��̌�
	 */
	virtual int			NShape()=0;
	/**	@brief �o�^����Ă���`����擾����
		@param index �`��̃C���f�b�N�X
		@return �`��ւ̃|�C���^
		�ŏ��ɓo�^���ꂽ�`��̃C���f�b�N�X��0�C���̎��ɓo�^���ꂽ�`��̃C���f�b�N�X��1�C
		�ȉ����l�D
	 */
	virtual CDShapeIf*	GetShape(int index)=0;
	/** @brief �`��̈ʒu�ƌX�����擾����
		@param index �ΏۂƂ���`��̃C���f�b�N�X
		@return ���̂ɑ΂���`��̈ʒu�ƌX��
		�C���f�b�N�X�ɂ��Ă�GetShape���Q�ƁD
	 */
	virtual Posed		GetShapePose(int index)=0;
	/** @brief �`��̈ʒu�ƌX����ݒ肷��
		@param index �ΏۂƂ���`��̃C���f�b�N�X
		@param pose ���̂ɑ΂���`��̈ʒu�ƌX��
		�C���f�b�N�X�ɂ��Ă�GetShape���Q�ƁD
	 */
	virtual void		SetShapePose(int index, const Posed& pose)=0;
	
	/** @brief �d�͂�L��/����������
		@param bOn true�Ȃ�΍��̂ɏd�͂������Dfalse�Ȃ�Ή����Ȃ��D
	 */
	virtual void		SetGravity(bool bOn)=0;
	/** @brief �����@���ɏ]�����ǂ�����L��/����������
		@param bOn true�Ȃ�΍��͕̂����@���ɂ��������ĉ^������D
		�����@���ɏ]��Ȃ����̂��CSetPose�CSetVelocity�Ȃǂ̊֐��ɂ��ʒu�⑬�x�̍X�V��
		�\�ł��D
	 */
	virtual void		SetDynamical(bool bOn)=0;
	/** @brief �����@���ɏ]�����ǂ������擾����
		@return true�Ȃ�΍��͕̂����@���ɂ��������ĉ^������D
	 */
	virtual bool		IsDynamical()=0;
};

//@}

}	//	namespace Spr
#endif
