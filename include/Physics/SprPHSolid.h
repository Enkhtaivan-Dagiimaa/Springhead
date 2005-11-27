#ifndef SPR_PHSolidIf_H
#define SPR_PHSolidIf_H
#include <Foundation/SprObject.h>

namespace Spr{;

struct PHSolidDesc{
	double		mass;			///<	����
	Matrix3d	inertia;		///<	�����e���\��
	Vec3d		velocity;		///<	���x			(World)
	Vec3d		angVelocity;	///<	�p���x			(World)
	Vec3d		center;			///<	���ʒ��S�̈ʒu	(Local..frame��posture�n)
	Posed		pose;			///<	�ʒu�ƌ���		(World)

	PHSolidDesc(){ Init(); }
	void Init(){
		mass = 1.0f;
		inertia=Matrix3d::Unit();
	}
};

struct CDShapeIf; 
///	����
struct PHSolidIf : public ObjectIf{
	virtual void AddForce(Vec3d f)=0;					///< �͂����ʒ��S�ɉ�����
	virtual void AddTorque(Vec3d t)=0;					///< �g���N��������
	virtual void AddForce(Vec3d f, Vec3d r)=0;			///< �͂� �ʒur(World�n) �ɉ�����

	virtual double		GetMass()=0;							///< ����
	virtual double		GetMassInv()=0;						///< ���ʂ̋t��
	virtual void		SetMass(double m)=0;						///< ���ʂ̐ݒ�
	virtual Matrix3d	GetInertia()=0;					///< �����e���\��

	virtual Vec3d		GetFramePosition() const =0;
	virtual void		SetFramePosition(const Vec3d& p) =0;
	virtual Vec3d		GetCenterPosition() const  =0;
	virtual void		SetCenterPosition(const Vec3d& p) =0;

	///	�����̎擾
	virtual Quaterniond GetOrientation() const =0;
	///	�����̐ݒ�
	virtual void		SetOrientation(const Quaterniond& q) =0;

	///	���ʒ��S�̑��x�̎擾
	virtual Vec3d		GetVelocity() const  =0;
	///	���ʒ��S�̑��x�̐ݒ�
	virtual void		SetVelocity(const Vec3d& v) =0;

	///	�p���x�̎擾
	virtual Vec3d		GetAngularVelocity() const =0;
	///	�p���x�̐ݒ�
	virtual void		SetAngularVelocity(const Vec3d& av)=0;

	///	�`��̒ǉ�
	virtual void		AddShape(CDShapeIf* shape)=0;

	///	�d�͂�L��/����������	�����ł悢���^��
	virtual void		SetGravity(bool bOn)=0;

	virtual int			GetNShapes()=0;
	///	�`����擾
	virtual CDShapeIf**	GetShapes()=0;
};

}	//	namespace Spr
#endif
