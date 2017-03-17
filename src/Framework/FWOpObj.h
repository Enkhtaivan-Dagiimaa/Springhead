#ifndef FWOPOBJ_H
#define FWOPOBJ_H


#include <Framework\SprFWOpObj.h>
#include <Framework/FWObject.h>
#include "FrameworkDecl.hpp"
#include <Graphics\GRMesh.h>
#include <Physics/PHOpObj.h>
namespace Spr{ ;

class FWOpObj: public FWObject{
	SPR_OBJECTDEF(FWOpObj);
public :
	UTRef<GRMesh> grMesh;
	UTRef<PHOpObj> opObj;
	float fwPSize;

	FWOpObj(const FWOpObjDesc& d = FWOpObjDesc());

	//GRMesh����AOp�I�u�W�F�N�g�����
	void CreateOpObj();
	//���_��Graphics�ɔ��f����
	void Blend();
	void Sync();
	void CreateOpObjWithRadius(float r);
	///	�q�I�u�W�F�N�g�̐�
	virtual size_t NChildObject() const;
	///	�q�I�u�W�F�N�g�̎擾
	virtual ObjectIf* GetChildObject(size_t pos);
	///	�q�I�u�W�F�N�g�̒ǉ�
	virtual bool AddChildObject(ObjectIf* o);
	ObjectIf* GetOpObj();
	ObjectIf* GetGRMesh();
};

}
#endif