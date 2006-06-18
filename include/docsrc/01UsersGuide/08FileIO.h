/** \page pageFileIO �t�@�C�����o��SDK

\ref pageFileIO �́C\ref sec_SceneGraph �̃t�@�C�����o�͂��s���܂��D
����SDK��API(\ref pagePhysics \ref pageGraphics)��p���āC
�t�@�C�����烍�[�h�����f�[�^����ɃV�[���O���t�̐���������C
�V�[���O���t�̃f�[�^��ǂݏo���āC�t�@�C���Ƀf�[�^��ۑ����܂��D

\image html FileScene.png

��̐}�́C�V�[���O���t�ƃt�@�C�����o�͂̃C���[�W�ł��D
�t�@�C����ł̃m�[�h�̐e�q�֌W�ƃV�[���O���t�̐e�q�֌W�͕K���Ή�����킯�ł͂���܂���D

\section secXFile X�t�@�C��
Springhead2��FileIO�́C���̂Ƃ���DirectX��X�t�@�C�������[�h���邱�Ƃ��ł��܂��D
�i����VRML�Ȃǂ����[�h�ł���悤�Ɋg������\��ł��D�j
X�t�@�C���Ȃǂ̃t�@�C���̏����́C���@�ƃm�[�h�̒�`�ɕ�����Ē�߂��Ă��܂��D
�Ⴆ��X�t�@�C���Ȃ�΁C
<pre>
xof 0302txt 0064	#�ŏ��̍s�͂��ꂩ��n�܂�
#	#��//�̌�̓R�����g
#�m�[�h�́C�^���C�m�[�h�� { �t�B�[���h�̌J��Ԃ�   �q�m�[�h }����Ȃ�D
PHScene scene1{
	0.01;0;;			#	�t�B�[���h �� �l; �̌J��Ԃ�
	1;0;-9.8;0;;		#	�l�� ���l�C������܂��̓t�B�[���h
	PHSolid soFloor{	#	�q�m�[�h�́C�m�[�h�Ɠ���
		(�ȗ�)
	}
}
</pre>
�̂悤�ɁC���@�����߂��Ă��܂��D
�m�[�h�̒�`�i�m�[�h�̌^���Ƃ��̃m�[�h�^�ɂǂ̂悤�ȃt�B�[���h�����邩�j
�ɂ��ẮC���@�ł͌��߂��Ă��܂���D

\section secNodeDef �m�[�h�̒�`
Springhead2�̒ʏ�̃m�[�h�́C�I�u�W�F�N�g�̃f�B�X�N���v�^(\ref secStateDesc ���Q��)��1��1�Ή����܂��D
���[�h���ɂ́C�f�B�X�N���v�^�ɑΉ�����I�u�W�F�N�g����������C�V�[���O���t�ɒǉ�����܂��D
�Z�[�u���ɂ́C�I�u�W�F�N�g����f�B�X�N���v�^��ǂݏo���C�m�[�h�̌`���Ńt�@�C���ɕۑ�����܂��D

�I�u�W�F�N�g�̃f�B�X�N���v�^�ɂ́C�K���Ή�����m�[�h������܂��D
�Ⴆ�΁CSprPHScene.h �ɂ́C
<pre>
struct PHSceneState{
	double timeStep;	///< �ϕ��X�e�b�v
	unsigned count;		///< �ϕ�������
};
struct PHSolidDesc:PHSceneState{
	///	�ڐG�E�S�������G���W���̎��
	enum SolverType{ SOLVER_PENALTY, SOLVER_CONSTRAINT} contactSolver;
	Vec3f gravity;	/// �d�͉����x�x�N�g���D�f�t�H���g�l��(0.0f, -9.8f, 0.0f)�D
};
</pre>
�̂悤�ɁC�X�e�[�g�ƃf�B�X�N���v�^���錾����Ă��܂��D����PHSceneDesc�ɑΉ�����X�t�@�C���̃m�[�h�́C
<pre>
PHScene scene1{
	0.01;			#	PHSceneState::timeStep
	0;;				#	PHSceneState::count			�Ō��;��PHSceneState���̏I���������D
	1;				#	PHSceneDesc::solverType
	0;-9.8;0;;		#	PHSceneDesc::gravity		Vec3f�̂��ƂɁCgravity�̏I��������;������D
}
</pre>
�̂悤�ɂȂ�܂��D�N���X�̃����o�ϐ������̂܂܃t�B�[���h�ɂȂ�܂��D
�܂��C��{�N���X�́C�擪�Ƀt�B�[���h���ǉ����ꂽ�`�ɂȂ�܂��D

�ʏ�m�[�h�̈ꗗ�� \ref pageNodeDefList ���Q�Ƃ��������D

\section secSpecialNode ����m�[�h
����m�[�h�́C�ߋ��̃t�@�C���t�H�[�}�b�g�Ƃ̌݊����̂��߂̃m�[�h�ł��D
���Ƃ��΁CDirectX��X�t�@�C���ɂ͂��Ƃ��ƁC
<pre>
Frame{
	FrameTransfromMatrix{ 1,0,0,0, 0,1,0,0, 0,0,1,0, 0,0,0,1; }
}
</pre>
�̂悤�ȃt���[���̃m�[�h�^������܂����CSprinhead2�ɂ͑Ή�����f�B�X�N���v�^��I�u�W�F�N�g������܂���D
�����ŁC�����́CGRFrame�ɕϊ�����ă��[�h����܂��D
���̂悤�ȁC�Ή�����I�u�W�F�N�g���Ȃ��m�[�h������m�[�h�ł��D
����m�[�h�̈ꗗ�́C\ref pageNodeDefList ���Q�Ƃ��������D

\section secLoadNode �m�[�h�̃��[�h
�f�B�X�N���v�^�ɑΉ�����m�[�h�͎��̎菇�Ő�������܂��D
<ol>
 <li> �m�[�h�̌^�ɑΉ�����f�B�X�N���v�^�𐶐�����D
 <li> �f�B�X�N���v�^�Ƀm�[�h�̃t�B�[���h�̓��e���L������D
 <li> ��c�m�[�h�� CreateObject() API��p���āC�I�u�W�F�N�g�𐶐�����D
 <li> ���������I�u�W�F�N�g�ɖ��O��ݒ肷��D
 <li> ���������I�u�W�F�N�g��e�I�u�W�F�N�g�ɒǉ�����D
 <li> �q�m�[�h�����[�h����D
</ol>

\section secFileIOLoadSave �t�@�C���̃��[�h�ƃZ�[�u

�t�@�C���̃��[�h��Z�[�u���s���ɂ́C�܂�FISdk���쐬���CFISdk����C
FIFileX���쐬���܂��D

�ȉ��́C�t�@�C�����[�h�ƃZ�[�u�̃R�[�h�̃T���v���ł��D
<pre>
	FISdkIf* fiSdk = CreateFISdk();				//	FISdk�̍쐬
	FIFileXIf* fileX = fiSdk->CreateFileX();	//	FIFileX�̍쐬
	/*	�I�u�W�F�N�g�̔z��D
		�����Ƀt�@�C�����烍�[�h�����I�u�W�F�N�g���ǉ������D*/
	ObjectIfs objs;	
	if (! fileX->Load(objs, "test.x") ) {	//	���[�h
		DSTR << "Error: Cannot open load file. " << std::endl;
		exit(EXIT_FAILURE);
	}
	PHSdk* phSdk = NULL;
	for(unsigned  i=0; i<objs.size(); ++i){
		phSdk = DCAST(PHSdkIf, objs[i]);	//	���[�h����PHSdk�����o���D
		if(phSdk) break;
	}
	objs.clear();
	objs.push_back(phSdk);
	fileX->Save(objs, "save.x");
</pre>

*/
