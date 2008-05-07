#ifndef CRMANIPULATOR_H
#define CRMANIPULATOR_H

#include <Springhead.h>
#include <Foundation/Object.h>

#include <vector>

#include "CRBody.h"

namespace Spr{;

//@{
class CRManipulator : public CRBody, public CRManipulatorDesc {

private:
	PHSceneIf*									phScene;		// ���u���V�[���ւ̃|�C���^
	std::vector< PHSolidIf* >					soFingersTop;	// �w��̍��̂ւ̃|�C���^�̔z��
	std::vector< std::vector< PHSolidIf* > >	soFingers;		// �e�w�̍��̂ւ̃|�C���^��5�{�w�ɑ΂��ēK�p��������
	std::vector< std::vector< PHJointIf* > >	joFingers;		// �e�w�̊֐߂ւ̃|�C���^��5�{�w�ɑ΂��ēK�p��������
	std::vector< PHBallJointIf* >				joFingerTips;	// �e�w�̕t�����֐߂ւ̃|�C���^��5�{�w�ɑ΂��ēK�p��������

public:
	SPR_OBJECTDEF(CRManipulator);
	ACCESS_DESC(CRManipulator);

	CRManipulator(){}
	CRManipulator(const CRManipulatorDesc& desc, CRCreatureIf* c = NULL)
		: CRManipulatorDesc(desc),
		CRBody((const CRManipulatorDesc&)desc, c)
	{
		solids.resize(CRManipulatorDesc::SO_NSOLIDS);
		joints.resize(CRManipulatorDesc::JO_NJOINTS);

		CreateHand();

		Init();

	}

	int	 NSolids();
	int	 NJoints();
	int  NBallJoints();
	int  NHingeJoints();

	bool CreateHand();					// ����쐬����
	bool CreateThumb();					// �e�w�n���쐬����(�w�ԍ� 0 )
	bool CreateIndexFinger();			// �l�����w�n���쐬����(�w�ԍ� 1 )
	bool CreateLongFinger();			// ���w�n���쐬����(�w�ԍ� 2 )
	bool CreateAnnularFinger();			// ��w�n���쐬����(�w�ԍ� 3 )
	bool CreateDigitusFinger();			// �q�w�n���쐬����(�w�ԍ� 4 )

	void Grip();						// ���铮���������
	void Release();						// �E�͂����|�[�Y��������i�����Ă�����̂�����j
	void Rock();						// �O�[�̃|�[�Y��������
	void Paper();						// �p�[�̃|�[�Y��������
	void Scissors();					// �`���L�̃|�[�Y��������

};

}

#endif