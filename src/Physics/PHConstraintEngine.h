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

	/// �ڐG�Ɋ֗^���鍄�̂̏��
	struct PHSolidAux{
		double		minv;				/// �e���̂̎��ʂ̋t��
		Matrix3d	Iinv;				/// �e���̂̊����s��̋t�s��
		Vec3d		dVlin_nc, dVang_nc;
	};
	typedef std::vector<PHSolidAux> PHSolidAuxArray;

	/// 1�̐ڐG�_
	struct PHContactPoint{
		Vec3d pos;					/// �ڐG�_�̈ʒu
		Matrix3d Jlin[2], Jang[2];	/// J�s��̃u���b�N
		Matrix3d Tlin[2], Tang[2];	/// T�s��̃u���b�N
		Vec3d b;					/// b�x�N�g���̃u���b�N

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

		������̕����͗\��ł��B��߂邩���B����
	  */
	class PHContacts : public std::vector<PHContact>{
	public:
		int	ncon;	/// �ڐG�_�̑���
	};

	/// LCP��^����s��
	class PHLCPMatrix : public std::vector<Matrix3d>{
		int ncon;	/// �ڐG�̐�
	public:
		typedef std::vector<Matrix3d> base_type;
		void resize(size_t K){
			base_type::resize(K * K);
			ncon = K;
		}
		Matrix3d& item(int i, int j){return (*this)[i * ncon + j];}
	};
	
protected:
	PHSolids		solids;		/// �S���͌v�Z�̑ΏۂƂȂ鍄��
	PHContacts		contacts;	/// ���̓��m�̐ڐG���
	PHSolidAuxArray	solidauxs;
	PHLCPMatrix		A;

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
