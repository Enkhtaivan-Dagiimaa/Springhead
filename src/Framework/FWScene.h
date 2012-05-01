/*
 *  Copyright (c) 2003-2008, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */
#ifndef FWSCENE_H
#define FWSCENE_H

#include <Framework/SprFWScene.h>
#include <Framework/SprFWObject.h>
#include <Framework/SprFWBone.h>
#include <Framework/SprFWHapticPointer.h>
#include <Foundation/Scene.h>
#include <HumanInterface/HIBase.h>

namespace Spr{;

struct CDConvexMeshIf;
struct CDFaceIf;
struct PHSceneIf;
struct GRSceneIf;

class FWSdk;

///	FWScene�̃t�@�N�g��
class FWSceneFactory : public FactoryBase {
public:
	const IfInfo* GetIfInfo() const {
		return FWSceneIf::GetIfInfoStatic();
	}
	ObjectIf* Create(const void* desc, const ObjectIf*){
		return CreateFWScene();
	}
};

///	Graphics��Physics��Scene���܂Ƃ߂��V�[���O���t�S��
class FWScene : public Scene, public FWSceneDesc {
public:
	SPR_OBJECTDEF(FWScene);
	ACCESS_DESC(FWScene);

	typedef std::vector< UTRef<FWObjectIf> >		FWObjects;
	typedef std::vector< UTRef<FWBoneIf> >			FWBones;
	typedef std::vector< UTRef<FWStructureIf> >		FWStructures;
	typedef std::vector< UTRef<HIHaptic> >			FWHumanInterfaces;
	typedef std::vector< UTRef<FWHapticPointerIf> > FWHapticPointers;
	
	FWSdk*				sdk;						///<	�eSDK�ւ̎Q��
	FWObjects			fwObjects;					///<	�����ƃO���t�B�b�N�X�̃I�u�W�F�N�g�̃����N
	FWBones				fwBones;					///<	�����ƃO���t�B�b�N�X��Bone�̃����N
	FWStructures		fwStructures;				///<	BoneObject�̏W���̂ł���FWStructure�ւ̃����N
	FWHapticPointers	fwHapticPointers;			///<	�o�^����Ă���͊o�|�C���^�ւ̃����N
	PHSceneIf*			phScene;					///<	�����V�~�����[�V�����p�̃V�[��
	GRSceneIf*			grScene;					///<	�O���t�B�b�N�X�p�̃V�[��

	/// �`�搧��t���O
	std::map<ObjectIf*, bool>	renderObject;
	bool		renderPHScene;
	bool		renderSolid, renderWire;
	bool		renderAxisWorld, renderAxisSolid, renderAxisConst;
	bool		renderForceSolid, renderForceConst;
	bool		renderContact;
	bool		renderGridX, renderGridY, renderGridZ;
	bool		renderLimit;
	bool		renderIK;
	bool		renderHaptic;

	/// �`��X�P�[��
	float		scaleAxisWorld, scaleAxisSolid, scaleAxisConst;
	float		scaleForce, scaleMoment;
	float		scaleIK;
	/// �`��}�e���A��
	std::map<PHSolidIf*, int>	matSolid, matWire;
	Vec3i		matAxis;
	int			matForce, matMoment;
	int			matContact;
	Vec3i		matGrid;
	int			matIK;
	/// ���W���ݒ�
	int			axisStyle;
	/// �O���b�h�ݒ�
	Vec3f		gridOffset;
	Vec3f		gridSize;
	Vec3i		gridSlice;

	//	hase TBW
	//	FWHumanInterfaces humanInterfaces;			///<	���[�U�C���^�t�F�[�X�D[0]���J�����DHIForceDevice6D�̃N���X���͕ύX����\��D
	enum HumanInterfacePurposeId{
		HI_CAMERACONTROLLER,
		HI_OBJECTMANIPULATOR0,
		HI_OBJECTMANIPULATOR1,
		HI_OBJECTMANIPULATOR2,
		HI_OBJECTMANIPULATOR3,
	};

public:

	/// �R���X�g���N�^
	FWScene(const FWSceneDesc& d=FWSceneDesc());
	
	/** ��{�@�\
	 */
	PHSceneIf*		GetPHScene(){ return phScene; }
	void			SetPHScene(PHSceneIf* s){ phScene = s; }
	GRSceneIf*		GetGRScene(){ return grScene; }
	void			SetGRScene(GRSceneIf* s){ grScene = s; }
	FWObjectIf*		CreateFWObject();
	int				NObject()const{ return (int)fwObjects.size(); }
	FWObjectIf**	GetObjects(){ return (FWObjectIf**)&*fwObjects.begin(); }

	void		Sync(bool ph_to_gr = true);
	void		Step();

	/** �`��@�\
	 */
	void	Draw			(GRRenderIf* render, bool ph_or_gr);
	void	DrawPHScene		(GRRenderIf* render);
	void	DrawSolid		(GRRenderIf* render, PHSolidIf* solid, bool solid_or_wire);
	void	DrawShape		(GRRenderIf* render, CDShapeIf* shape, bool solid_or_wire);
	void	DrawConstraint	(GRRenderIf* render, PHConstraintIf* con);
	void	DrawContact		(GRRenderIf* render, PHContactPointIf* con);
	void	DrawIK			(GRRenderIf* render, PHIKEngineIf* ikEngine);
	void	DrawLimit		(GRRenderIf* render, PHConstraintIf* con);
	void	DrawHaptic		(GRRenderIf* render, PHHapticEngineIf* hapticEngine);
	
	/// �`�搧��
	void	SetRenderMode		(bool solid, bool wire);
	void	EnableRender		(ObjectIf* obj, bool enable);
	void	SetSolidMaterial	(int mat, PHSolidIf* solid);
	void	SetWireMaterial		(int mat, PHSolidIf* solid);

	void	EnableRenderAxis	(bool world, bool solid, bool con);
	void	SetAxisMaterial		(int matX, int matY, int matZ);
	void	SetAxisScale		(float scaleWorld, float scaleSolid, float scaleConst);
	void	SetAxisStyle		(int style);

	void	EnableRenderForce	(bool solid, bool con);
	void	SetForceMaterial	(int matf, int matm);
	void	SetForceScale		(float scalef, float scalem);

	void	EnableRenderContact	(bool enable);
	void	SetContactMaterial	(int mat);

	void	EnableRenderGrid	(bool x, bool y, bool z);
	void	SetGridOption		(char axis, float offset, float size, int slice);
	void	SetGridMaterial		(int matX, int matY, int matZ);

	void	EnableRenderIK		(bool enable);
	void	SetIKMaterial		(int mat);
	void	SetIKScale			(float scale);

	void	EnableRenderLimit	(bool enable);

	void	EnableRenderHaptic	(bool enable);

	//void	EnableRender		(int item, bool enable = true, ObjectIf* obj = 0);
	//void	SetSolidMaterial	(int item, int mat, ObjectIf* obj = 0);
	//void	SetWireMaterial		(int item, int mat, ObjectIf* obj = 0);
	//void	SetRenderScale		(int item, float scale, ObjectIf* obj = 0);
	//void	SetGridOption		(char axis, float offset, float size, int slice);
	/// ���`�搧��API
	//void	EnableRenderWorldAxis	(bool enable, float scale);
	//void	EnableRenderAxis		(bool enable, float scale);
	//void	EnableRenderForce		(bool enable, float scale);
	//void	EnableRenderContact		(bool enable);
	//void	EnableRenderGrid		(bool enable, float y, float span);
	//void	EnableRenderIK			(bool enable, float scale);

	/// �����p
	void	DrawCoordinateAxis	(GRRenderIf* render, float scale, bool solid_or_wire);
	void	DrawMesh			(GRRenderIf* render, CDConvexMeshIf* mesh, bool solid_or_wire);
	void	DrawFaceSolid		(GRRenderIf* render, CDFaceIf* face, Vec3f * base);
	void	DrawFaceWire		(GRRenderIf* render, CDFaceIf* face, Vec3f * base);
	void	DrawForce			(GRRenderIf* render, const Vec3d& f, const Vec3d& t);
	bool	IsRenderEnabled		(ObjectIf* obj);
	int		GetSolidMaterial	(PHSolidIf* solid);
	int		GetWireMaterial		(PHSolidIf* solid);
	
	/** HumanInterface�n�֐�
	 */
	HIForceDevice6D*	GetHumanInterface(size_t pos);
	void				AddHumanInterface(HIForceDevice6D* d);
	void				SetFWBones(FWBoneIf* b);
	std::vector< UTRef<FWBoneIf> > GetFWBones();
	void				CreateFWStructure();
	void				AddFWStructure(FWStructureIf* o);
	FWStructureIf*		GetFWStructure();
	FWStructureIf*		GetFWStructure(int n);
	size_t				NFWStructure();
	FWHapticPointerIf*	CreateHapticPointer();
	FWHapticPointerIf*	GetHapticPointer(int i);
	int					NHapticPointers();
	void				UpdateHapticPointers();

	/** Scene�֐��̌p��
	 */
	virtual ObjectIf* CreateObject(const IfInfo* info, const void* desc);
	virtual bool AddChildObject(ObjectIf* o);
	virtual bool DelChildObject(ObjectIf* o);
	virtual size_t NChildObject() const;
	virtual ObjectIf* GetChildObject(size_t pos);
	virtual NamedObjectIf* FindObject(UTString name, UTString cls);
	
};


}

#endif
