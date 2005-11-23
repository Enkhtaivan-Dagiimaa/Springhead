#ifndef PHSCENE_H
#define PHSCENE_H
#include <Foundation/Scene.h>
#include <Physics/PHEngine.h>

namespace Spr {;

class PHShape;

/**	�V�[���O���t�̃g�b�v�m�[�h�D�����E���_�����D
	�����_���ƃV�[���O���t�̊֌W���[�����߁C
	�����_�����Ⴄ�ƃV�[���O���t�͂��̂܂܎g�p�ł��Ȃ��D
	�V�[���O���t�́C���Ƃ��΃����_����OpenGL�Ȃ�CdisplayList
	�������Ă��邵�CD3D�Ȃ�� ID3DXMesh�������Ă���D
	OpenGL�̃V�[���O���t��D3D�ɕϊ����邽�߂ɂ́C��xDocument��
	�Z�[�u���āCD3D�`���Ń��[�h���Ȃ���΂Ȃ�Ȃ��D	*/
class SPR_DLL PHScene:public PHSceneIf, public Scene{
	OBJECTDEF(PHScene);
	BASEIMP_OBJECT(Scene);
public:
	PHEngines engines;
protected:
	/// �ϕ��X�e�b�v
	double timeStep;
	/// �ϕ�������
	unsigned int count;
	///	����SDK
	PHSdkIf* sdk;
	///	PHShape
	std::vector< UTRef<PHShape> > shapes;
public:
	///	�R���X�g���N�^
	PHScene();
	PHScene(PHSdkIf* s);
	void Init();
	///	�f�X�g���N�^
	~PHScene(){Clear();}

	PHSolidIf* CreateSolid();
	PHSolidIf* CreateSolid(const PHSolidDesc& desc);
	PHShapeIf* CreateShape(const PHShapeDesc& desc);

	void SetGravity(const Vec3d& g);

	/// �ϕ��X�e�b�v��Ԃ�
	double GetTimeStep()const{return timeStep;}
	/// �ϕ��X�e�b�v��ݒ肷��
	void SetTimeStep(double dt);
	/// �J�E���g����Ԃ�
	unsigned GetCount()const{return count;}
	/// �J�E���g����ݒ肷��
	void SetCount(unsigned c){count = c;}
	///	�V�[���̎�����i�߂� ClearForce(); GenerateForce(); Integrate(); �Ɠ���
	void Step();
	///	�V�[���̎�����i�߂�i�͂̃N���A�j
	void ClearForce();
	///	�V�[���̎�����i�߂�i�͂̐����j
	void GenerateForce();
	///	�V�[���̎�����i�߂�i�͂Ƒ��x��ϕ����āC���x�ƈʒu���X�V�j
	void Integrate();
	///	�V�[������ɂ���D
	void Clear();
	///	���O�\����C�Q�Ƃ���Ă��Ȃ��I�u�W�F�N�g���폜����D
	void ClearName();
	///	�f�o�b�O�p
	void Print(std::ostream& os) const { names.Print(os); }

protected:
	friend class Object;
};

}
#endif
