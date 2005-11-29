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
		Vec3d point;		/// �ڐG�_�̈ʒu
		Matrix3d J[2][2];	/// LCP���\�����郄�R�r�s��

	};
	typedef std::vector<PHContactPoint> PHContactPoints;

	/// Solid���m�̌���
	struct PHContactVolume{
		int solids[2];				/// �ڐG���Ă��鍄��
		int shapes[2];				/// �ڐG���Ă���`��
		CDShape* intersection;		/// �����`��
		Vec3d normal;				/// �@��
		Vec3d center;				/// �����`��̏d�S
		PHContactPoints	points;		/// normal�ɒ������镽�ʂ֎ˉe�����ڐG���ʑ̂̒��_�z��
	};
	typedef std::vector<PHContactVolume> PHContactVolumes;
	

protected:
	PHSolids			solids;		//�S���͌v�Z�̑ΏۂƂȂ鍄��
	PHContactVolumes	contacts;	//���̓��m�̐ڐG���

	//UTCombination<UTRef<PHSolidPair> > solidPairs;
public:
	void Add(PHSolid* s);
	void Init();
	///
	int GetPriority() const {return 0/*SGBP_CONSTRAINTENGINE*/;}
	///	���x���ʒu�A�����x�����x�̐ϕ�
	virtual void Step();
	virtual void Clear(PHScene* s){ solids.clear(); }
};

}	//	namespace Spr
#endif
