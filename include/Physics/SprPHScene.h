
/** \addtogroup gpPhysics 	*/
//@{

/**
 *	@file SprPHScene.h
 *	@brief �V�[��
*/
#ifndef SPR_PHSCENEIF_H
#define SPR_PHSCENEIF_H

namespace Spr{;

struct PHSolidIf;
struct PHSolidDesc;

struct CDShapeIf;
struct CDShapeDesc;

struct PHJointIf;
struct PHJointDesc;

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
/// �����G���W���̃V�[���̃f�X�N���v�^
struct PHSceneDesc: public PHSceneState{
	///	�ڐG�E�S�������G���W���̎��
	enum SolverType{
		SOLVER_PENALTY,
		SOLVER_CONSTRAINT
	} contactSolver;
	PHSceneDesc(){Init();}
	void Init(){
		//contactSolver = SOLVER_PENALTY;
		contactSolver = SOLVER_CONSTRAINT;
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

	/** @brief �����Solid�̑g�ɂ��ĐڐG��L����/����������
		@param bEnable true�Ȃ�ΗL��������Dfalse�Ȃ�Ζ���������D

		Solid lhs��Solid rhs�Ƃ̐ڐG��L����/����������D
		���������ꂽ�ꍇ�C�Փ˔����ڐG�͂̌v�Z�͍s���Ȃ��D
	  */
	virtual void EnableContact(PHSolidIf* lhs,	PHSolidIf* rhs, bool bEnable = true)=0;

	/** @brief �֐߂��쐬����
		@param desc �֐߂̃f�B�X�N���v�^
		@return �֐߂̃C���^�t�F�[�X
		Solid lhs��Solid rhs��A������֐߂��쐬����D
	 */
	virtual PHJointIf* CreateJoint(PHSolidIf* lhs, PHSolidIf* rhs, const PHJointDesc& desc)=0;

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
