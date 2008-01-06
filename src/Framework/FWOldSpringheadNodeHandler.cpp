/*
 *  Copyright (c) 2003-2008, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */
#include <Foundation/UTLoadHandler.h>
#include <Foundation/UTLoadContext.h>
#include <Foundation/UTPath.h>
#include <FileIO/FISdk.h>
#include <FileIO/FIFileX.h>
#include <Graphics/GRFrame.h>
#include <Graphics/GRMesh.h>
#include <Graphics/GRSphere.h>
#include <Graphics/GRRender.h>
#include <Graphics/GRScene.h>
#include <Physics/PHSolid.h>
#include <Physics/PHJoint.h>
#include <Framework/FWOldSpringheadNodeHandler.h>
#include <Framework/FWObject.h>
#include <Framework/FWScene.h>
#include <Framework/FWSdk.h>
#include <Collision/CDConvexMesh.h>

#include <Framework/FWOldSpringheadNode.h>

namespace SprOldSpringhead{
using namespace Spr;

// FWScene�̌����D
static FWScene* FindFWScene(UTLoadContext* fc){
	FWScene* fs = NULL;
	for(int i=(int)fc->objects.size()-1; i>=0; --i){
		fs = DCAST(FWScene, fc->objects[i]);
		if (fs) break;
	}
	return fs;
}

// PHScene�̌����D
static PHScene* FindPHScene(UTLoadContext* fc){
	PHScene* ps = NULL;
	for(int i=(int)fc->objects.size()-1; i>=0; --i){
		ps = DCAST(PHScene, fc->objects[i]);
		if (ps) break;
		FWScene* fs = DCAST(FWScene, fc->objects[i]);
		if (fs) ps = DCAST(PHScene, fs->GetPHScene());
		if (ps) break;
	}
	return ps;
}

// GRScene�̌����D
static GRScene* FindGRScene(UTLoadContext* fc){
	GRScene* gs = NULL;
	for(int i=(int)fc->objects.size()-1; i>=0; --i){
		gs = DCAST(GRScene, fc->objects[i]);
		if (gs) break;
		FWScene* fs = DCAST(FWScene, fc->objects[i]);
		if (fs) gs = DCAST(GRScene, fs->GetGRScene());
		if (gs) break;
	}
	return gs;
}

// X�t�@�C���`���̃o�[�W�������D
class FWNodeHandlerXHeader: public UTLoadHandlerImp<Header>{
public:
	FWNodeHandlerXHeader():UTLoadHandlerImp<Desc>("Header"){}
};

// Springhead1�̃t���[���D
class FWNodeHandlerXFrame: public UTLoadHandlerImp<Frame>{
public:	
	FWNodeHandlerXFrame():UTLoadHandlerImp<Desc>("Frame"){}	
	void BeforeCreateObject(Desc& d, UTLoadedData* ld, UTLoadContext* fc){
		GRFrameDesc desc;
		ObjectIf* o = fc->CreateObject(GRFrameIf::GetIfInfoStatic(), &desc, ld->GetName());
		fc->flags.Push(o != NULL);
		if (o){
			fc->objects.Push(o);
			ld->loadedObjects.push_back(o);
		}
	}
	void AfterCreateChildren(Desc& d,  UTLoadedData* ld, UTLoadContext* fc){
		if(fc->flags.Pop()) fc->objects.Pop();
	}
};

// Springhead1��Simulator�^�X�N�D
class FWSimulatorTask: public UTLoadTask{
public:
	OBJECTDEF_NOIF(FWSimulatorTask, UTLoadTask);
	double timeStep;	// �ϕ��X�e�b�v
	double decay;		// ���̂̑��x�̌�����
	void Execute(UTLoadContext* fc){}
};
OBJECT_IMP(FWSimulatorTask, UTLoadTask);

// Springhead1��Simulator�D
class FWNodeHandlerSimulator: public UTLoadHandlerImp<Simulator>{
public:	
	FWNodeHandlerSimulator():UTLoadHandlerImp<Desc>("Simulator"){}
	void BeforeCreateObject(Desc& d, UTLoadedData* ld, UTLoadContext* fc){		
		UTRef<FWSimulatorTask> simtask = DBG_NEW FWSimulatorTask;
		simtask->timeStep = d.timeStep;
		simtask->decay    = d.decay;		
		PHScene* phScene = FindPHScene(fc);		
		if (phScene){		// phScene�������ł����ꍇ�́AtimeStep��ݒ�. 
			fc->mapObj.insert(UTPairObject(simtask->Cast(), phScene->Cast()));
			phScene->SetTimeStep(d.timeStep);
		}else{				// Scene���[�h���ɁAmap���������AtimeStep��ݒ� 			
			fc->mapObj.insert(UTPairObject(simtask->Cast(), NULL));
		}
	}
};

// DirectX��FrameTransformMatrix�D�t���[���̕ϊ��s���\���DGRFrame::transform�ɑΉ��D
class FWNodeHandlerXFrameTransformMatrix: public UTLoadHandlerImp<FrameTransformMatrix>{
public:	
	FWNodeHandlerXFrameTransformMatrix():UTLoadHandlerImp<Desc>("FrameTransformMatrix"){}
	void BeforeCreateObject(Desc& d, UTLoadedData* ld, UTLoadContext* fc){
		GRFrame* fr = DCAST(GRFrame, fc->objects.Top());
		if (fr){
			fr->transform = d.matrix;
		}else{
			fc->ErrorMessage(NULL, NULL, "FrameTransformMatrix must be inside of Frame node.");
		}
	}
};

// Springehead1�̌����DGRLight�ɑΉ��D
class FWNodeHandlerXLight8: public UTLoadHandlerImp<Light8>{
public:
	class Adapter: public UTLoadTask{
	public:
		GRLight* light;
		bool AddChildObject(ObjectIf* o){
			GRFrame* fr = DCAST(GRFrame, o);
			if (fr){
				Affinef af = fr->GetTransform();
				light->position.sub_vector(PTM::TSubVectorDim<0,3>()) = af.Ez();
				return true;
			}
			return false;
		}
		void Execute(UTLoadContext* ctx){}
	};
	FWNodeHandlerXLight8():UTLoadHandlerImp<Desc>("Light8"){}
	void BeforeCreateObject(Desc& l8, UTLoadedData* ld, UTLoadContext* fc){
		GRLightDesc grld;
		grld.ambient = l8.ambient;
		grld.attenuation0 = l8.attenuation0;
		grld.attenuation1 = l8.attenuation1;
		grld.attenuation2 = l8.attenuation2;
		grld.diffuse = l8.diffuse;
		if (l8.type == Light8::XLIGHT_DIRECTIONAL){		// ���s����
			grld.position.sub_vector(PTM::TSubVectorDim<0,3>()) = l8.direction;
			grld.position.W() = 0;
		}else{											// �_�����E�X�|�b�g���C�g����
			grld.position.sub_vector(PTM::TSubVectorDim<0,3>()) = l8.position;
			grld.position.W() = 1;
		}
		grld.range = l8.range;
		grld.specular = l8.specular;
		grld.spotFalloff = l8.falloff;
		grld.spotInner = l8.spotInner;
		grld.spotCutoff = l8.spotCutoff;
		//	����n���E��n�̕ϊ�
		grld.position.Z() *= -1;
		grld.spotDirection.Z() *= -1;
		fc->objects.Push(fc->CreateObject(GRLightIf::GetIfInfoStatic(), &grld, ld->GetName()));
		Adapter* a = DBG_NEW Adapter;
		a->light = DCAST(GRLight, fc->objects.Top());
		fc->objects.Push(a->Cast());
		fc->links.push_back(a);
	}
	void AfterCreateChildren(Desc& d, UTLoadedData* ld, UTLoadContext* fc){
		fc->objects.Pop();
		fc->objects.Pop();
	}
};

// DirectX�̃e�N�X�`���t�@�C�����^�X�N�D
class FWXTextureTask: public UTLoadTask{
public:
	OBJECTDEF_NOIF(FWXTextureTask, UTLoadTask);
	UTString filename;					// �e�N�X�`���t�@�C����
	void Execute(UTLoadContext* fc){}
};
OBJECT_IMP(FWXTextureTask, UTLoadTask);

// DirectX�̃e�N�X�`���t�@�C�����iMaterial�̓����^�O)�D
class FWNodeHandlerXTextureFilename: public UTLoadHandlerImp<TextureFilename>{
public:
	FWNodeHandlerXTextureFilename():UTLoadHandlerImp<Desc>("TextureFilename"){}
	void BeforeCreateObject(Desc& d, UTLoadedData* ld, UTLoadContext* fc){
		GRMaterial* mat = fc->objects.Top()->Cast();
		if (!mat){
			fc->ErrorMessage(NULL, NULL, "A texture filename appeared at incorrect place.");
			exit(-1);
		}
		mat->texname = d.filename;
	}
};

// DirectX�̃}�e���A���DGRMateiral�ɑΉ��D
class FWNodeHandlerXMaterial: public UTLoadHandlerImp<Material>{
public:
	FWNodeHandlerXMaterial():UTLoadHandlerImp<Desc>("Material"){}
	void BeforeCreateObject(Desc& d, UTLoadedData* ld, UTLoadContext* fc){
		GRMaterialDesc desc;
		desc.ambient = d.face;
		desc.diffuse = d.face;
		desc.specular = Vec4f(d.specular.x, d.specular.y, d.specular.z, 1.0);
		desc.emissive = Vec4f(d.emissive.x, d.emissive.y, d.emissive.z, 1.0);
		desc.power = d.power;
		fc->objects.Push(fc->CreateObject(GRMaterialIf::GetIfInfoStatic(), 
			&desc, ld->GetName() ));
		ld->loadedObjects.push_back(fc->objects.Top());
	}
	void AfterCreateChildren(Desc& d, UTLoadedData* ld, UTLoadContext* fc){
		fc->objects.Pop();		// GRMaterial
	}
};

// DirectX��Mesh�DGRMesh�ɑΉ��D
class FWNodeHandlerXMesh: public UTLoadHandlerImp<Mesh>{
public:
	FWNodeHandlerXMesh():UTLoadHandlerImp<Desc>("Mesh"){}
	void BeforeCreateObject(Desc& d, UTLoadedData* ld, UTLoadContext* fc){
		GRMeshDesc desc;
		fc->objects.Push(fc->CreateObject(GRMeshIf::GetIfInfoStatic(), &desc, ld->GetName()));	
		ld->loadedObjects.push_back(fc->objects.Top());
		GRMesh* mesh = DCAST(GRMesh, fc->objects.Top());
		
		//	Mesh�̖��O�ɁA tex3d ���܂܂��ꍇ�A�e�N�X�`����2D�łȂ�4D�ɂ���B
		if (ld->GetName().find("tex3d") != UTString::npos){
			mesh->tex3d = true;
		}
		if (mesh){
			mesh->positions = d.vertices;	// ���_���W
			for (int f=0; f < d.nFaces; ++f){		
				if ((d.faces[f].nFaceVertexIndices == 3) || (d.faces[f].nFaceVertexIndices == 4)) {
					mesh->originalFaceIds.push_back(f);
					mesh->faces.push_back( d.faces[f].faceVertexIndices[0] );
					mesh->faces.push_back( d.faces[f].faceVertexIndices[1] );
					mesh->faces.push_back( d.faces[f].faceVertexIndices[2] );
					mesh->originalFaces.push_back( d.faces[f].faceVertexIndices[0] );
					mesh->originalFaces.push_back( d.faces[f].faceVertexIndices[1] );
					mesh->originalFaces.push_back( d.faces[f].faceVertexIndices[2] );

					if (d.faces[f].nFaceVertexIndices == 4){
						mesh->originalFaceIds.push_back(f);
						// faces�ɂ́A�ʂ��l�p�`�Ȃ�O�p�`�ɕ��������C���f�b�N�X��push
						mesh->faces.push_back( d.faces[f].faceVertexIndices[0] );
						mesh->faces.push_back( d.faces[f].faceVertexIndices[2] );
						mesh->faces.push_back( d.faces[f].faceVertexIndices[3] );
						// originalFaces �ɂ́A4���_�ڂ̃C���f�b�N�X��push
						mesh->originalFaces.push_back( d.faces[f].faceVertexIndices[3] );
					}
				}else{
					fc->ErrorMessage(NULL, NULL, "Number of faces for mesh != 3 or 4.");
				}
			}
			//	�@�����
			UTLoadedData* normalData = ld->FindDescendant("MeshNormals");
			if (normalData){
				MeshNormals* normalDesc = (MeshNormals* )normalData->data;
				mesh->normals = normalDesc->normals;
				for (int f=0; f < normalDesc->nFaceNormals; ++f){
					for (int i=0; i < normalDesc->faceNormals[f].nFaceVertexIndices; ++i){
						mesh->faceNormals.push_back(	// �@���C���f�b�N�X
							normalDesc->faceNormals[f].faceVertexIndices[i] );
					}
				}
			}else{	//	�@�����̎�������
				mesh->normals.resize(mesh->positions.size());
				std::vector<int> nFace;
				nFace.resize(mesh->positions.size());
				for(unsigned i=0; i<nFace.size(); ++i) nFace[i] = 0;
				for(unsigned i=0; i<mesh->faces.size(); i+=3){
					Vec3f normal = 
						(mesh->positions[mesh->faces[i+1]]-mesh->positions[mesh->faces[i]])
						^ (mesh->positions[mesh->faces[i+2]]-mesh->positions[mesh->faces[i]]);
					normal.unitize();

					mesh->normals[mesh->faces[i  ]] += normal;
					mesh->normals[mesh->faces[i+1]] += normal;
					mesh->normals[mesh->faces[i+2]] += normal;
					nFace[mesh->faces[i  ]] ++;
					nFace[mesh->faces[i+1]] ++;
					nFace[mesh->faces[i+2]] ++;
				}
				for(unsigned i=0; i<mesh->normals.size(); ++i){
					mesh->normals[i] /= nFace[i];
				}
			}
			//	�e�N�X�`�����W
			UTLoadedData* coordsData = ld->FindDescendant("MeshTextureCoords");
			if (coordsData){
				MeshTextureCoords* coordsDesc = (MeshTextureCoords* )coordsData->data;
				mesh->texCoords = coordsDesc->textureCoords;
			}
			//	�}�e���A�����X�g
			UTLoadedData* mlData = ld->FindDescendant("MeshMaterialList");
			if (mlData){
				MeshMaterialList* mlDesc = (MeshMaterialList*) mlData->data;
				mesh->materialList = mlDesc->faceIndexes;
			}
			//	�X�L�����b�V���̃w�b�_
			UTLoadedData* skinHeader = ld->FindDescendant("XSkinMeshHeader");
			if (skinHeader){
				XSkinMeshHeader* sd = (XSkinMeshHeader*) skinHeader->data;
				//	���ɉ������Ȃ��H	
			}
			//	�X�L�����b�V���̏d�݂Â�
			for(unsigned i=0; i<ld->children.size(); ++i){
				if (ld->children[i]->type->GetTypeName().compare("SkinWeights")==0){
					SkinWeights* sw = (SkinWeights*) ld->children[i]->data;
					mesh->skinWeights.push_back(GRMesh::SkinWeight());
					for(unsigned i=0; i<sw->nWeights; ++i){
						mesh->skinWeights.back().indices.push_back(sw->vertexIndices[i]);
						mesh->skinWeights.back().weights.push_back(sw->weights[i]);
					}
					mesh->skinWeights.back().offset = sw->matrixOffset;
					fc->links.push_back( DBG_NEW UTLinkTask(mesh->Cast(), 
						sw->transformNodeName, mesh->GetNameManager()) );
				}
			}

		}else{
			fc->ErrorMessage(NULL, NULL, "cannot create Mesh node.");
		}
	}
	void AfterCreateChildren(Desc& d, UTLoadedData* ld, UTLoadContext* fc){
		fc->objects.Pop();
	}
};

inline void SetPHMaterial(PHMaterial& mat, const PhysicalMaterial& oldMat){
	mat.e = (1-oldMat.nd) / oldMat.ns;
	if (mat.e < 0) mat.e = 0;
	mat.mu = oldMat.d;
	mat.mu0 = oldMat.s;
}
// DirectX��Mesh�DPHSolid�̎q���̏ꍇ�CCDConvexMesh�����[�h�D
class FWNodeHandlerSolidXMesh: public UTLoadHandlerImp<PHSolidDesc>{
public:
	FWNodeHandlerSolidXMesh():UTLoadHandlerImp<Desc>("PHSolidDesc"){}
	void AfterCreateObject(Desc& d, UTLoadedData* ld, UTLoadContext* fc){
		std::vector<UTLoadedDatas> meshes;
		ld->EnumLinkDescendant(meshes, "Mesh");
		if (!meshes.size()) return;

		for(unsigned i=0; i<meshes.size(); ++i){
			Mesh& mesh = *(Mesh*)meshes[i][0]->data;
			CDConvexMeshDesc cmd;
			for(unsigned j=0; j< mesh.vertices.size(); ++j){
				cmd.vertices.push_back(mesh.vertices[j]);
			}
			UTLoadedData* ldMat = meshes[i][0]->FindDescendant("PhysicalMaterial");
			if (ldMat){	//	�����}�e���A���̃��[�h
				SetPHMaterial(cmd.material, *(PhysicalMaterial*)ldMat->data);
			}
			Affinef afShape;
			for(unsigned j=1; j<meshes[i].size();++j){
				if (meshes[i][j]->type->GetTypeName().compare("Frame")==0){
					for(unsigned k=0; k<meshes[i][j]->children.size(); ++k){
						UTString tn = meshes[i][j]->children[k]->type->GetTypeName();
						if (tn.compare("FrameTransformMatrix") == 0) {
							FrameTransformMatrix* ftm = 
								(FrameTransformMatrix*) meshes[i][j]->children[k]->data;
							afShape = ftm->matrix * afShape;
						}
					}
				}
			}
			for (unsigned j=0; j<cmd.vertices.size(); ++j){
				cmd.vertices[j] = afShape * cmd.vertices[j];
			}
			ObjectIf* obj = fc->CreateObject(CDConvexMeshIf::GetIfInfoStatic(), &cmd, meshes[i][0]->GetName())->Cast();
			ld->loadedObjects.push_back(obj);
		}
	}
};

// DirectX��AnimationSet�D
class FWNodeHandlerXAnimationSet: public UTLoadHandlerImp<AnimationSet>{
public:
	FWNodeHandlerXAnimationSet():UTLoadHandlerImp<Desc>("AnimationSet"){}
	void BeforeCreateObject(Desc& d, UTLoadedData* ld, UTLoadContext* fc){
		GRAnimationSetDesc ad;
		fc->objects.Push(fc->CreateObject(
			GRAnimationSetIf::GetIfInfoStatic(), &ad, ld->GetName()));	
		ld->loadedObjects.push_back(fc->objects.Top());
	}
	void AfterCreateChildren(Desc& d, UTLoadedData* ld, UTLoadContext* fc){
		fc->objects.Pop();
	}
};

// DirectX��Animation�D
class FWNodeHandlerXAnimation: public UTLoadHandlerImp<Animation>{
	class LoadInitalPose: public UTLoadTask{
		GRAnimation* anim;
	public:
		LoadInitalPose(GRAnimation* a):anim(a){}
		void Execute(UTLoadContext* ctx){
			anim->LoadInitialPose();
		}
	};
public:
	FWNodeHandlerXAnimation():UTLoadHandlerImp<Desc>("Animation"){}
	void BeforeCreateObject(Desc& d, UTLoadedData* ld, UTLoadContext* fc){
		GRAnimationDesc ad;
		fc->objects.Push(fc->CreateObject(
			GRAnimationIf::GetIfInfoStatic(), &ad, ld->GetName()));	
		ld->loadedObjects.push_back(fc->objects.Top());
		fc->postTasks.push_back(new LoadInitalPose(fc->objects.Top()->Cast()));
	}
	void AfterCreateChildren(Desc& d, UTLoadedData* ld, UTLoadContext* fc){
		fc->objects.Pop();
	}
	
};

// DirectX��AnimationKey�D
class FWNodeHandlerXAnimationKey: public UTLoadHandlerImp<AnimationKey>{
public:
	FWNodeHandlerXAnimationKey():UTLoadHandlerImp<Desc>("AnimationKey"){}
	void BeforeCreateObject(Desc& d, UTLoadedData* ld, UTLoadContext* fc){
		GRAnimation* anim = fc->objects.Top()->Cast();
		assert(anim);
		anim->keys.push_back(GRAnimationKey());
		GRAnimationKey& key = anim->keys.back();
		key.keyType = (GRAnimationDesc::KeyType)d.keyType;
		for(unsigned i=0; i<d.keys.size(); ++i){
			key.keys.push_back(GRKey());
			key.keys.back().time = d.keys[i].time;
			key.keys.back().values = d.keys[i].tfkeys.values;
		}
	}
};

// Spirnghead1��Sphere
class FWNodeHandlerSolidSphere: public UTLoadHandlerImp<PHSolidDesc>{
public:
	FWNodeHandlerSolidSphere():UTLoadHandlerImp<Desc>("PHSolidDesc"){}
	void AfterCreateObject(Desc& d, UTLoadedData* ld, UTLoadContext* fc){
		std::vector<UTLoadedDatas> spheres;
		ld->EnumLinkDescendant(spheres, "Sphere");
		if (!spheres.size()) return;

		for(unsigned i=0; i<spheres.size(); ++i){
			Sphere& sphere = *(Sphere*)spheres[i][0]->data;
			CDSphereDesc csd;
			csd.radius = sphere.radius;
			UTLoadedData* ldMat = spheres[i][0]->FindDescendant("PhysicalMaterial");
			if (ldMat){	//	�����}�e���A���̃��[�h
				SetPHMaterial(csd.material, *(PhysicalMaterial*)ldMat->data);
			}
			Affinef afShape;
			for(unsigned j=1; j<spheres[i].size();++j){
				if (spheres[i][j]->type->GetTypeName().compare("Frame")==0){
					for(unsigned k=0; k<spheres[i][j]->children.size(); ++k){
						UTString tn = spheres[i][j]->children[k]->type->GetTypeName();
						if (tn.compare("FrameTransformMatrix") == 0) {
							FrameTransformMatrix* ftm = 
								(FrameTransformMatrix*) spheres[i][j]->children[k]->data;
							afShape = ftm->matrix * afShape;
						}
					}
				}
			}
			ObjectIf* obj = fc->CreateObject(
				CDSphereIf::GetIfInfoStatic(), &csd, spheres[i][0]->GetName())->Cast();
			ld->loadedObjects.push_back(obj);

			PHSolid* solid = fc->objects.Top()->Cast();
			if (solid){
				int pos = (int)(solid->NChildObject()-1);
				Quaterniond pose;
				pose.FromMatrix(afShape);
				solid->SetShapePose(pos, pose);
			}
		}
	}
};

class FWNodeHandlerSphere: public UTLoadHandlerImp<Sphere>{
public:
	FWNodeHandlerSphere():UTLoadHandlerImp<Desc>("Sphere"){}
	void BeforeCreateObject(Desc& d, UTLoadedData* ld, UTLoadContext* fc){
	GRSphereDesc desc; 
	fc->objects.Push(fc->CreateObject(GRSphereIf::GetIfInfoStatic(), &desc, ld->GetName()));
	ld->loadedObjects.push_back(fc->objects.Top());
	GRSphere* sphere = DCAST(GRSphere, fc->objects.Top());
		sphere->radius = d.radius;
		sphere->slices = d.slices;
		sphere->stacks = d.stacks;
	}
	void AfterCreateChildren(Desc& d, UTLoadedData* ld, UTLoadContext* fc){
		assert(DCAST(GRSphere, fc->objects.Top()));
		fc->objects.Pop();
	}
};	

// Springhead1��ContactInactive�̃^�X�N�D
class FWContactInactiveTask: public UTLoadTask{
public:
	OBJECTDEF_NOIF(FWContactInactiveTask, UTLoadTask);
	int nSolids;
	std::vector<int> solidIndexes;
	void Execute(UTLoadContext* fc){}
};
OBJECT_IMP(FWContactInactiveTask, UTLoadTask);

// Springhead1��ContactInactive.
class FWNodeHandlerContactInactive: public UTLoadHandlerImp<ContactInactive>{
public:
	FWNodeHandlerContactInactive():UTLoadHandlerImp<Desc>("ContactInactive"){}
	void BeforeCreateObject(Desc& d, UTLoadedData* ld, UTLoadContext* fc){
		UTRef<FWContactInactiveTask> task = DBG_NEW FWContactInactiveTask;
		task->nSolids = d.nSolids;
		task->solidIndexes = d.solidIndexes;
		fc->objects.Top()->AddChildObject(task->Cast());
		fc->links.push_back(task);
	}
};

// �Â������}�e���A���D���[�h���̋L�^�p�D
class FWPHMaterialTask: public UTLoadTask{
public:
	OBJECTDEF_NOIF(FWPHMaterialTask, UTLoadTask);
	FWPHMaterialTask(): mu(0.2f), mu0(0.2f), e(0.2f){}
	float mu;		///<	�����C���C�W��
	float mu0;		///<	�Î~���C�W��
	float e;		///<	���˕Ԃ�W��	
	void Execute(UTLoadContext* fc){}
};
OBJECT_IMP(FWPHMaterialTask, UTLoadTask);

// �Â������}�e���A���D
class FWNodeHandlerPhysicalMaterial: public UTLoadHandlerImp<PhysicalMaterial>{
public:	
	FWNodeHandlerPhysicalMaterial():UTLoadHandlerImp<Desc>("PhysicalMaterial"){}
	void BeforeCreateObject(Desc& d, UTLoadedData* ld, UTLoadContext* fc){
		UTRef<FWPHMaterialTask> phmtask = DBG_NEW FWPHMaterialTask;
		phmtask->mu = d.d;		// �ő�Î~���C�W��
		phmtask->mu0 = d.s;		// �����C�W��		
		if (fc->datas.Top()->GetName().length()){
			GRScene* gs = FindGRScene(fc);
			phmtask->SetNameManager(gs->Cast());
			phmtask->SetName(fc->datas.Top()->GetName().c_str());
		}		
		fc->links.push_back(phmtask);
		fc->mapObj.insert(UTPairObject(fc->objects.Top(), phmtask->Cast()));
	}
	void AfterCreateChildren(Desc& d, UTLoadedData* ld, UTLoadContext* fc){	
	}
};


// Springhead1��ContactEngine�D
class FWNodeHandlerContactEngine: public UTLoadHandlerImp<ContactEngine>{
public:	
	class Adapter: public UTLoadTask{
	public:
		PHSceneIf* phScene;
		std::vector< UTRef<PHSolidIf> > solids;	// ContactEngine�m�[�h�ɒǉ�����Ă�������solids�ɓo�^�D
												// ContactInactive�m�[�h�̃��[�h�ŗ��p�D
		std::vector< UTRef<FWContactInactiveTask> > inactiveTask;	// ContactEngine�^�X�N
		UTLoadContext* fc;
		Adapter():phScene(NULL){}
		void AddFrameToSolid(PHSolid* solid, GRFrame* fr, Affinef af){
			af = af * fr->GetTransform();
			for(unsigned i=0; i<fr->NChildObject(); ++i){
				GRVisual* v = DCAST(GRVisual, fr->GetChildObject(i));
				GRFrame* f = DCAST(GRFrame, v);
				if (f) AddFrameToSolid(solid, f, af);

				//	Mesh�̏ꍇ�CCDConvexMesh��ǉ�
				GRMesh* m = DCAST(GRMesh, v);
				if (m){
					CDConvexMeshDesc mdesc;
					for(unsigned i=0; i<m->positions.size(); ++i){
						mdesc.vertices.push_back(m->positions[i]);
					}

					// �����C���C�W��mu�A�Î~���C�W��mu0 
					UTMapObject::iterator itr = fc->mapObj.find(m->Cast());
					if (itr != fc->mapObj.end()){
						FWPHMaterialTask* maptask = DCAST(FWPHMaterialTask, itr->second);	
						mdesc.material.mu = maptask->mu;
						mdesc.material.mu0 = maptask->mu0;
					}
					solid->CreateShape(mdesc.GetIfInfo(), mdesc);
					Posed pose;
					pose.FromAffine(af);
					solid->SetShapePose(solid->NShape()-1, pose);
				}

				//	Sphere�̏ꍇ�CCDSphere��ǉ�
				GRSphere* s = DCAST(GRSphere, v);
				if (s){
					CDSphereDesc sdesc;
					sdesc.radius = s->radius;

					// �����C���C�W��mu�A�Î~���C�W��mu0 
					UTMapObject::iterator itr = fc->mapObj.find(s->Cast());
					if (itr != fc->mapObj.end()){
							FWPHMaterialTask* maptask = DCAST(FWPHMaterialTask, itr->second);
							sdesc.material.mu = maptask->mu;
							sdesc.material.mu0 = maptask->mu0;
					}
					solid->CreateShape(sdesc.GetIfInfo(), sdesc);
					Posed pose;
					pose.FromAffine(af);
					solid->SetShapePose(solid->NShape()-1, pose);
				}
			}
		}
		virtual bool AddChildObject(ObjectIf* o){
			GRFrame* fr = DCAST(GRFrame, o);
			if (fr){	//	Solid�ɕϊ����Ēǉ�
				PHSolidDesc sd;
				PHSolid* solid = DCAST(PHSolid, phScene->CreateSolid(PHSolidDesc()));
				UTString name("so");
				name.append(fr->GetName());
				solid->SetName(name.c_str());
				solid->SetDynamical(false);
				Posed pose;
				pose.FromAffine(fr->GetTransform());
				solid->SetPose(pose);
				AddFrameToSolid(solid, fr, fr->GetTransform().inv());
				solids.push_back(solid->Cast());
				return true;
			}
			PHSolid* so = DCAST(PHSolid, o);
			if (so){
				solids.push_back(so->Cast());
				return true;
			}
			FWContactInactiveTask* task = DCAST(FWContactInactiveTask, o);
			if (task){
				inactiveTask.push_back(task);
			}
			return false;
		}
		void Execute(UTLoadContext* fc){
			phScene->SetContactMode(PHSceneDesc::MODE_NONE);
			for(unsigned i=0; i<solids.size(); ++i){
				phScene->SetContactMode(solids[i], PHSceneDesc::MODE_LCP);
			}
			// ContactInactive�m�[�h
			int iindex=0, jindex=0;
			for (unsigned int t=0; t<inactiveTask.size(); ++t){		
				for (unsigned int i=0; i<inactiveTask[t]->solidIndexes.size(); ++i){
					for (unsigned int j=i+1; j<inactiveTask[t]->solidIndexes.size(); ++j){
						iindex = inactiveTask[t]->solidIndexes[i];
						jindex = inactiveTask[t]->solidIndexes[j];
						phScene->SetContactMode(solids[iindex], solids[jindex], 
							PHSceneDesc::MODE_NONE);
					}
				}
			}
		}
	};
	FWNodeHandlerContactEngine():UTLoadHandlerImp<Desc>("ContactEngine"){}
	void BeforeCreateObject(Desc& d, UTLoadedData* ld, UTLoadContext* fc){
		PHScene* ps = FindPHScene(fc);
		Adapter* task = DBG_NEW Adapter;
		task->phScene = ps->Cast();
		task->fc = fc;
		fc->objects.Push(task->Cast());
		ld->loadedObjects.push_back(fc->objects.Top());
	}
	void AfterCreateChildren(Desc& d, UTLoadedData* ld, UTLoadContext* fc){
		Adapter* task = (Adapter*)DCAST(UTLoadTask, fc->objects.Top());
		fc->links.push_back(task);
		fc->objects.Pop();	//	task
	}
};


// Springhead1��Solid�D
class FWNodeHandlerSolid: public UTLoadHandlerImp<Solid>{
public:
	class Adapter: public FWNodeHandlerContactEngine::Adapter{
	public:
		PHSolid* solid;
		GRFrame* frame;
		FWScene* fwScene;
		Adapter():solid(NULL), frame(NULL), fwScene(NULL){}
		virtual bool AddChildObject(ObjectIf* o){
			frame = DCAST(GRFrame, o);
			if (frame){	//	fr�ȉ��̑SMesh��Solid�ɒǉ�
				AddFrameToSolid(solid, frame, frame->GetTransform().inv());
				return true;
			}
			return false;
		}
		virtual void Execute(UTLoadContext* ctx){
			FWObject* obj = DCAST(FWObject, fwScene->CreateObject(FWObjectIf::GetIfInfoStatic(), NULL));
			assert(frame);
			Posed pose;
			pose.FromAffine(frame->GetTransform());
			solid->SetPose(pose);
			obj->AddChildObject(frame->Cast());
			obj->AddChildObject(solid->Cast());
			fwScene->AddChildObject(obj->Cast());
		}
	};

	FWNodeHandlerSolid():UTLoadHandlerImp<Desc>("Solid"){}
	void BeforeCreateObject(Desc& d, UTLoadedData* ld, UTLoadContext* fc){
		PHSolidDesc desc; 
		fc->objects.Push(fc->CreateObject(PHSolidIf::GetIfInfoStatic(), &desc, ld->GetName()));
		ld->loadedObjects.push_back(fc->objects.Top());
		PHSolid* solid = DCAST(PHSolid, fc->objects.Top());
		solid->center = d.center;
		solid->velocity = d.velocity;
		solid->angVelocity = d.angularVelocity;
		solid->inertia = d.inertia;
		solid->mass = d.mass;
		
		Adapter* task = DBG_NEW Adapter;
		task->solid = solid;
		task->fwScene = FindFWScene(fc);
		task->fc = fc;
		fc->objects.Push(task->Cast());
		ld->loadedObjects.push_back(fc->objects.Top());
	}
	void AfterCreateChildren(Desc& d, UTLoadedData* ld, UTLoadContext* fc){
		Adapter* task = (Adapter*)DCAST(UTLoadTask, fc->objects.Top());
		fc->links.push_back(task);
		fc->objects.Pop();	//	task

		assert(DCAST(PHSolid, fc->objects.Top()));
		fc->objects.Pop();	//	solid
	}
};

// Springhead1��Camera�D
class FWNodeHandlerCamera: public UTLoadHandlerImp<Camera>{
public:	
	FWNodeHandlerCamera():UTLoadHandlerImp<Desc>("Camera"){}
	void BeforeCreateObject(Desc& d, UTLoadedData* ld, UTLoadContext* fc){
		d.view.ExZ() *= -1;
		d.view.EyZ() *= -1;
		d.view.EzX() *= -1;
		d.view.EzY() *= -1;
		d.view.PosZ() *= -1;

		GRCameraDesc cd;
		cd.back = d.back;
		cd.front = d.front;
		cd.center = Vec2f(d.offsetX, d.offsetY);
		cd.size = Vec2f(d.width, d.height);
		fc->objects.Push(fc->CreateObject(GRCameraIf::GetIfInfoStatic(), &cd, ld->GetName()));
		ld->loadedObjects.push_back(fc->objects.Top());
		GRCamera* cam = DCAST(GRCamera, fc->objects.Top());
		GRFrameDesc fd;
		fd.transform = d.view;
		GRFrameIf* frame = DCAST(GRFrameIf, cam->GetNameManager()->CreateObject(GRFrameIf::GetIfInfoStatic(), &fd));
		cam->AddChildObject(frame);
	}
	void AfterCreateChildren(Desc& d, UTLoadedData* ld, UTLoadContext* fc){
		fc->objects.Pop();
	}
};

// Springhead1��GravityEngine�D
class FWNodeHandlerGravityEngine: public UTLoadHandlerImp<GravityEngine>{
public:	
	FWNodeHandlerGravityEngine():UTLoadHandlerImp<Desc>("GravityEngine"){}
	int linkPos;
	void BeforeCreateObject(Desc& d, UTLoadedData* ld, UTLoadContext* fc){
		PHScene* s = FindPHScene(fc);
		if (s) s->SetGravity(d.gravity);
		linkPos = (int)fc->links.size();
	}
	void AfterCreateChildren(Desc& d, UTLoadedData* ld, UTLoadContext* fc){
		//	GravityEngine�̎q�I�u�W�F�N�g�͂Ƃ肠���������B�{����Gravity��On/Off�Ɏg���B
		fc->links.resize(linkPos);
	}
};

// Springhead1��Scene�D
class FWNodeHandlerScene: public UTLoadHandlerImp<Scene>{
public:	
	FWNodeHandlerScene():UTLoadHandlerImp<Desc>("Scene"){}
	void BeforeCreateObject(Desc& d, UTLoadedData* ld, UTLoadContext* fc){
		//	Framework�����D
		FWSceneDesc fwsd;
		fc->objects.Push(fc->CreateObject(FWSceneIf::GetIfInfoStatic(), &fwsd, ld->GetName()));
		ld->loadedObjects.push_back(fc->objects.Top());
		FWScene* fws = DCAST(FWScene, fc->objects.Top());
		FWSdk* sdk = NULL;
		for(int i=(int)fc->objects.size()-1; i>=0; --i){
			sdk = fc->objects[i]->Cast();
			if (sdk) break;
		}
		assert(sdk);	//<	FWSdk���X�^�b�N�ɐς�ł����K�v����D

		//	GRScene�����D
		GRSceneIf* grScene = sdk->GetGRSdk()->CreateScene(GRSceneDesc());
		//	PHScene�����D
		PHSceneDesc psd;
		PHSceneIf* phScene = sdk->GetPHSdk()->CreateScene(psd);

		//	Framework�ɃV�[����o�^�D
		fws->AddChildObject(phScene);
		fws->AddChildObject(grScene);

		// timeStep��ݒ�D
		UTMapObject::iterator itr;
		for (itr=fc->mapObj.begin(); itr!=fc->mapObj.end(); ++itr){
			FWSimulatorTask* simtask = DCAST(FWSimulatorTask, itr->first);
			if (simtask){
				phScene->SetTimeStep(simtask->timeStep);
				itr->second = phScene;
			}
		}				
	}
	void AfterCreateChildren(Desc& d, UTLoadedData* ld, UTLoadContext* fc){
		fc->objects.Pop();
	}
};

// Springhead1��SolidContainer�D
class FWNodeHandlerSolidContainer: public UTLoadHandlerImp<SolidContainer>{
public:	
	FWNodeHandlerSolidContainer():UTLoadHandlerImp<Desc>("SolidContainer"){}
	void BeforeCreateObject(Desc& d, UTLoadedData* ld, UTLoadContext* fc){
	}
	void AfterCreateChildren(Desc& d, UTLoadedData* ld, UTLoadContext* fc){
	}
};

// Springhead1��JointEngine�D
class FWNodeHandlerJointEngine: public UTLoadHandlerImp<JointEngine>{
public:	
	class JointCreator: public UTLoadTask{
	public:
		PHScene* phScene;
		JointCreator* parent;
		PHJoint1DDesc desc;
		int nType;
		PHSolid* solid;
		UTString name;
		JointCreator(): phScene(NULL), parent(NULL), solid(NULL){}
		bool AddChildObject(ObjectIf* o){
			PHSolid* s = DCAST(PHSolid, o);
			if (s){
				solid = s;
				return true;
			}
			return false;
		}
		void Execute(UTLoadContext* fc){
			const float tinyMass = 0.1f;
			if (!solid){
				PHSolidDesc sd;
				sd.mass *= tinyMass;
				sd.inertia *= tinyMass;
				solid = DCAST(PHSolid, phScene->CreateSolid(sd));
			}
			if (parent && !parent->solid){
				PHSolidDesc sd;
				sd.mass *= tinyMass;
				sd.inertia *= tinyMass;
				parent->solid = DCAST(PHSolid, phScene->CreateSolid(sd));
			}
			if (parent){
				PHSceneIf* phSceneIf = phScene->Cast();
				PHJointIf* j = (nType ? 
					phSceneIf->CreateJoint(solid->Cast(), parent->solid->Cast(), (const PHSliderJointDesc&)desc) : 
					phSceneIf->CreateJoint(solid->Cast(), parent->solid->Cast(), (const PHHingeJointDesc&)desc));
				j->SetName(name.c_str());
			}
		}
	};

	FWNodeHandlerJointEngine():UTLoadHandlerImp<Desc>("JointEngine"){}
	void BeforeCreateObject(Desc& d, UTLoadedData* ld, UTLoadContext* fc){
		JointCreator* j = DBG_NEW JointCreator;
		j->phScene = FindPHScene(fc);
		fc->objects.Push(j->Cast());
	}
	void AfterCreateChildren(Desc& d, UTLoadedData* ld, UTLoadContext* fc){
		JointCreator* j = (JointCreator*) DCAST(UTLoadTask, fc->objects.Top());
		fc->links.push_back(j);
		fc->objects.Pop();
	}
};

// Springhead1��Joint�D
class FWNodeHandlerJoint: public UTLoadHandlerImp<Joint>{
public:	
	typedef FWNodeHandlerJointEngine::JointCreator JointCreator;
	FWNodeHandlerJoint():UTLoadHandlerImp<Desc>("Joint"){}
	void BeforeCreateObject(Desc& d, UTLoadedData* ld, UTLoadContext* fc){
		JointCreator* j = DBG_NEW JointCreator;
		j->name = ld->GetName();
		j->nType = d.nType;
		j->desc.posePlug.Pos() = d.crj;
		j->desc.posePlug.Ori().FromMatrix(d.cRj);
		j->desc.poseSocket.Pos() = d.prj;
		j->desc.poseSocket.Ori().FromMatrix(d.pRj);
		j->desc.spring = d.fPValue;
		j->desc.damper = d.fDValue;
		j->desc.origin = d.fInput;
		j->desc.lower = d.fMinPosition;
		j->desc.upper = d.fMaxPosition;
		
		j->parent = (JointCreator*)DCAST(UTLoadTask, fc->objects.Top());
		j->phScene = FindPHScene(fc);
		fc->objects.Push(j->Cast());
	}
	void AfterCreateChildren(Desc& d, UTLoadedData* ld, UTLoadContext* fc){
		JointCreator* j = (JointCreator*)DCAST(UTLoadTask, fc->objects.Top());
		fc->links.push_back(j);
		fc->objects.Pop();
	}
};

// Springhead1��Import�D
class FWNodeHandlerImport: public UTLoadHandlerImp<Import>{
public:	
	FWNodeHandlerImport():UTLoadHandlerImp<Desc>("Import"){}
	void AfterLoadData(Desc& d, UTLoadedData* ld, UTLoadContext* fc){
		UTString filename = d.file;

		UTPath xFilePath;
		xFilePath.Path(fc->fileMaps.back()->name);
		xFilePath.Path(xFilePath.FullPath());
		UTPath imPath;
		imPath.Path(filename);
        UTString oldCwd = imPath.GetCwd();
		imPath.SetCwd(xFilePath.Drive() + xFilePath.Dir());
		UTString full = imPath.FullPath();		

		UTRef<FISdkIf> sdk = FISdkIf::CreateSdk();
		fc->PushFileMap(full);
		if (fc->IsGood()){
			UTRef<FIFileX> fileX = DCAST(FIFileX, sdk->CreateFileX());
			fileX->LoadImp((FILoadContext*)fc);
		}
		fc->fileMaps.Pop();

		imPath.SetCwd(oldCwd);
	}
};

}


// �n���h����`
namespace Spr{
using namespace SprOldSpringhead;
void SPR_CDECL FWRegisterOldSpringheadNode(){
	UTLoadHandlerDb* handlers = UTLoadHandlerDbPool::Get("OldSpringhead");
	handlers->insert(DBG_NEW FWNodeHandlerXHeader);
	handlers->insert(DBG_NEW FWNodeHandlerXFrame);
	handlers->insert(DBG_NEW FWNodeHandlerXFrameTransformMatrix);
	handlers->insert(DBG_NEW FWNodeHandlerXLight8);
	handlers->insert(DBG_NEW FWNodeHandlerXTextureFilename);
	handlers->insert(DBG_NEW FWNodeHandlerXMaterial);		
	handlers->insert(DBG_NEW FWNodeHandlerXMesh);
	handlers->insert(DBG_NEW FWNodeHandlerXAnimation);
	handlers->insert(DBG_NEW FWNodeHandlerXAnimationKey);
	handlers->insert(DBG_NEW FWNodeHandlerXAnimationSet);
	handlers->insert(DBG_NEW FWNodeHandlerSolidXMesh);
	handlers->insert(DBG_NEW FWNodeHandlerSolidSphere);
	handlers->insert(DBG_NEW FWNodeHandlerSolid);
	handlers->insert(DBG_NEW FWNodeHandlerScene);
	handlers->insert(DBG_NEW FWNodeHandlerSimulator);
	handlers->insert(DBG_NEW FWNodeHandlerCamera);
	handlers->insert(DBG_NEW FWNodeHandlerSolidContainer);
	handlers->insert(DBG_NEW FWNodeHandlerGravityEngine);
	handlers->insert(DBG_NEW FWNodeHandlerContactInactive);
	handlers->insert(DBG_NEW FWNodeHandlerContactEngine);
	handlers->insert(DBG_NEW FWNodeHandlerJointEngine);
	handlers->insert(DBG_NEW FWNodeHandlerJoint);
	handlers->insert(DBG_NEW FWNodeHandlerImport);
}
}
