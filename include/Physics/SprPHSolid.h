#ifndef SPR_PHSolidIf_H
#define SPR_PHSolidIf_H
#include <Foundation/SprObject.h>

/**	\addtogroup gpPhysics	*/
//@{
namespace Spr{;
///	���̂̃X�e�[�g
struct PHSolidState{
	Vec3d		velocity;		///<	���x			(World�n)
	Vec3d		angVelocity;	///<	�p���x			(World�n)
	Vec3d		center;			///<	���ʒ��S�̈ʒu	(Local�n)
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
	bool		gravity;		///<	�d�͂̉e�����󂯂邩

	PHSolidDesc(){ Init(); }
	void Init(){
		mass = 1.0f;
		inertia=Matrix3d::Unit();
		gravity=true;
	}
};

struct CDShapeIf; 
///	����
struct PHSolidIf : public SceneObjectIf{
	IF_DEF(PHSolid);
	virtual void AddForce(Vec3d f)=0;					///< �͂����ʒ��S�ɉ�����
	virtual void AddTorque(Vec3d t)=0;					///< �g���N��������
	virtual void AddForce(Vec3d f, Vec3d r)=0;			///< �͂� �ʒur(World�n) �ɉ�����

	virtual double		GetMass()=0;					///< ����
	virtual double		GetMassInv()=0;					///< ���ʂ̋t��
	virtual void		SetMass(double m)=0;			///< ���ʂ̐ݒ�
	virtual Matrix3d	GetInertia()=0;					///< �����e���\��
	virtual void		SetInertia(const Matrix3d& I)=0;

	virtual Vec3d		GetFramePosition() const =0;
	virtual void		SetFramePosition(const Vec3d& p) =0;
	virtual Vec3d		GetCenterPosition() const  =0;
	virtual void		SetCenterPosition(const Vec3d& p) =0;

	///	�����̎擾
	virtual Quaterniond GetOrientation() const =0;
	///	�����̐ݒ�
	virtual void		SetOrientation(const Quaterniond& q) =0;

	/// �p���̎擾
	virtual Posed		GetPose() const = 0;
	/// �p���̐ݒ�
	virtual void		SetPose(const Posed& p) = 0;

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

	/// �`��̈ʒu�̎擾
	virtual Posed		GetShapePose(int pos)=0;

	/// �`��̈ʒu�̐ݒ�
	virtual void		SetShapePose(int pos, const Posed& pose)=0;

	///	�d�͂�L��/����������	�����ł悢���^��
	virtual void		SetGravity(bool bOn)=0;
	/// ���͊w�@���ɏ]�����ǂ�����L��/����������
	virtual void		SetDynamical(bool bOn)=0;
	virtual bool		IsDynamical()=0;

	/// �o�^����Ă���`��̌����擾
	virtual int			NShape()=0;

	///	�`����擾
	virtual CDShapeIf*	GetShape(int i)=0;

};

//@}

}	//	namespace Spr
#endif
