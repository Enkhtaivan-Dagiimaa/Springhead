#ifndef PHCONSTRAINTENGINE_H
#define PHCONSTRAINTENGINE_H

#include <Springhead.h>
#include <Foundation/Object.h>
#include <Physics/PHScene.h>
#include <Physics/PHEngine.h>
#include <Base/Combination.h>
#include <Collision/CDDetectorImp.h>

namespace Spr{;

class PHConstraintEngine: public PHEngine{
	OBJECTDEF(PHConstraintEngine);

	/// 1�̐ڐG�_
	struct PHContactPoint{
		Vec3d pos;				/// �ڐG�_�̈ʒu
		Matrix3d Jlin[2], Jang[2];	/// LCP���\�����郄�R�r�s��

	};
	typedef std::vector<PHContactPoint> PHContactPoints;

	/// Solid���m�̐ڐG
	struct PHContact{
		bool	bValid;				/// true => �L��, false => ����
		bool	bNew;				/// true => �V�K, false => �p��
		int solids[2];				/// �ڐG���Ă��鍄��
		int shapes[2];				/// �ڐG���Ă���`��
		CDShape* intersection;		/// �����`��
		Vec3d normal;				/// �@��
		Vec3d center;				/// �����`��̏d�S
		PHContactPoints	points;		/// normal�ɒ������镽�ʂ֎ˉe�����ڐG���ʑ̂̒��_�z��
	};

	/// �S�Ă̐ڐG��ێ�����R���e�i
	/** �E�V�����ڐG���������ꍇ�C�擪����X�L��������bValid == false�̗v�f��
		�@�V���ȐڐG���㏑�������D���̂Ƃ�bNew = true�ƂȂ�
		�E�����ڐG��2�X�e�b�v�ȏ�p�������ꍇbNew = false�ƂȂ�
		�E�ڐG���������ꂽ�ꍇ�C�Y������v�f��bValid == false�ƂȂ薳���������
	  */
	class PHContacts : public std::vector<PHContact>{
	public:
		bool IsInContact(){}
	};
	

protected:
	PHSolids	solids;		/// �S���͌v�Z�̑ΏۂƂȂ鍄��
	PHContacts	contacts;	/// ���̓��m�̐ڐG���

	std::vector<double>		minv;	/// �e���̂̎��ʂ̋t��
	std::vector<Matrix3d>	Iinv;	/// �e���̂̊����s��̋t�s��

	void Init();	/// ������

public:
	void AddSolid(PHSolid* s);
	void RemoveSolid(PHSolid* s);
	///
	int GetPriority() const {return 0/*SGBP_CONSTRAINTENGINE*/;}
	///	���x���ʒu�A�����x�����x�̐ϕ�
	virtual void Step();
	virtual void Clear(PHScene* s){ solids.clear(); }
};

}	//	namespace Spr
#endif
