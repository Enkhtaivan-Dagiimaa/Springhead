#ifndef PHSHAPE_H
#define PHSHAPE_H

#include <SprCollision.h>
#include <Foundation/Object.h>

namespace Spr{;

/// �`��
/// �`�󎩑͈̂ʒu���������Ȃ��D�ʒu���͌`����Q�Ƃ���e���̂����D
class CDShape : public InheritNamedObject<CDShapeIf, NamedObject>{
protected:
	//Posef pose;
public:
	PHMaterial material;
	static double epsilon;

	OBJECTDEFABST(CDShape);
	//Posef GetPose() const { return pose; }
	//void SetPose(const Posef& p){ pose = p; }
	
	virtual void CalcBBox(Vec3f& bbmin, Vec3f& bbmax, const Posed& pose)=0;
};

/// �`��ւ̎Q�Ƃƍ��̂ɑ΂��鑊�Έʒu
struct CDShapeRefWithPose{
	CDShape* shape;
	Posed pose;
};

template <class intf, class base>
struct InheritCDShape:public InheritNamedObject<intf, base>{
	//void SetPose(const Posef& p){ base::SetPose(p); }
	//Posef GetPose() const { return base::GetPose(); }
};

}	//	namespace Spr
#endif
