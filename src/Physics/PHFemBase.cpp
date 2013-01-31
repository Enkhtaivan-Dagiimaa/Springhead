#include <Physics/PHFemBase.h>
//#include <Physics/PHFemMeshNew.h>

namespace Spr{;

///////////////////////////////////////////////////////////////////
/* 計算モジュールの共通部分 */
void PHFem::SetPHFemMesh(PHFemMeshNew* m){ phFemMesh = m; }

PHFemMeshNew* PHFem::GetPHFemMesh(){ return phFemMesh; }

int PHFem::NVertices(){ return GetPHFemMesh()->NVertices(); }

}