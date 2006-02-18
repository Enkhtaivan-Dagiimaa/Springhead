#ifndef SPR_PHSCENEIF_H
#define SPR_PHSCENEIF_H

namespace Spr{;

struct PHSolidIf;
struct PHSolidDesc;

struct CDShapeIf;
struct CDShapeDesc;

/// �����G���W���̃V�[���̏��
struct PHSceneState{
	/// �ϕ��X�e�b�v
	double timeStep;
	/// �ϕ�������
	unsigned int count;
	PHSceneState(){Init();}
	void Init(){
		timeStep = 0.005;
		count = 0;
	}
};
/// �����G���W���̃V�[���̃f�X�N���v�^
struct PHSceneDesc: public PHSceneState{
	///	�ڐG�E�S�������G���W���̎��
	enum SolverType{
		SOLVER_PENALTY,
		SOLVER_CONSTRAINT
	} contactSolver;
	PHSceneDesc(){Init();}
	void Init(){
		contactSolver = SOLVER_PENALTY;
		//contactSolver = SOLVER_CONSTRAINT;
	}
};

///	�V�[��
struct PHSceneIf : public SceneIf{
public:
	IF_DEF(PHScene);

	///	����Scene������SDK��Ԃ�
	virtual PHSdkIf* GetSdk()=0;

	///	Shape�̍쐬
	virtual CDShapeIf* CreateShape(const CDShapeDesc& desc)=0;
	///	Solid�쐬
	virtual PHSolidIf* CreateSolid()=0;
	///	Solid�쐬
	virtual PHSolidIf* CreateSolid(const PHSolidDesc& desc)=0;
	///	Solid�̐�
	virtual int NSolids()=0;
	///	Solid�̎擾
	virtual PHSolidIf** GetSolids()=0;

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
	///	�d�͂̎擾
	virtual Vec3f GetGravity()=0;

};

}	//	namespace Spr
#endif
