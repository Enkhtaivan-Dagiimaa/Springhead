#include <Physics/PHFemBase.h>
//#include <Physics/PHFemMeshNew.h>

namespace Spr{;

///////////////////////////////////////////////////////////////////
/* �v�Z���W���[���̋��ʕ��� */
void PHFem::SetPHFemMesh(PHFemMeshNew* m){ phFemMesh = m; }

PHFemMeshNew* PHFem::GetPHFemMesh(){ return phFemMesh; }

int PHFem::NVertices(){ return GetPHFemMesh()->NVertices(); }

}