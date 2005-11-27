#ifndef SPR_PHSCENEIF_H
#define SPR_PHSCENEIF_H

namespace Spr{;

struct PHSolidIf;
struct PHSolidDesc;

struct CDShapeIf;
struct CDShapeDesc;

///	�V�[��
struct PHSceneIf : public ObjectIf{
public:

	///	Solid�쐬
	virtual PHSolidIf* CreateSolid()=0;
	///	Solid�쐬
	virtual PHSolidIf* CreateSolid(const PHSolidDesc& desc)=0;
	///	Solid�̐�
	virtual int GetNSolids()=0;
	///	Solid�̎擾
	virtual PHSolidIf** GetSolids()=0;


	///	Shape�쐬
	virtual CDShapeIf* CreateShape(const CDShapeDesc& desc)=0;
	///	Shape�̐�
	virtual int GetNShape()=0;
	///	Shape�̎擾
	virtual CDShapeIf** GetShapes()=0;


	/// �ϕ��X�e�b�v��Ԃ�
	virtual double GetTimeStep()const=0;
	/// �ϕ��X�e�b�v��ݒ肷��
	virtual void SetTimeStep(double dt)=0;
	/// �J�E���g����Ԃ�
	virtual unsigned GetCount()const=0;
	/// �J�E���g����ݒ肷��
	virtual void SetCount(unsigned c)=0;
	///	�V�[���̎�����i�߂� ClearForce(); GenerateForce(); Integrate(); �Ɠ���
	virtual void Step()=0;
	///	�V�[���̎�����i�߂�i�͂Ƒ��x��ϕ����āC���x�ƈʒu���X�V�j
	virtual void Integrate()=0;
	///	�V�[������ɂ���D
	virtual void Clear()=0;

	///	�d�͂̐ݒ�(�����Ɏ�������ׂ����v���������CNovodex�͂���)
	virtual void SetGravity(Vec3f accel)=0;
	virtual Vec3f GetGravity()=0;

};

}	//	namespace Spr
#endif
