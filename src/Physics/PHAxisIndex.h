/*
 *  Copyright (c) 2003-2012, Shoichi Hasegawa and Springhead development team 
 *  All rights reserved.
 *  This software is free software. You can freely use, distribute and modify this 
 *  software. Please deal with this software under one of the following licenses: 
 *  This license itself, Boost Software License, The MIT License, The BSD License.   
 */
#ifndef PH_AXIS_INDEX_H
#define PH_AXIS_INDEX_H

namespace Spr{;

/// �S�����Ǘ��N���X�D�S���̗L����ێ����C�S�������X�g���쐬����
template<int N>
class AxisIndex {

	/// ���̗L���E������ێ�����t���O
	bool   bCurr[N], bPrev[N];

	/// �L�����ԍ����X�g
	int    idx[N];

	/// idx�̗v�f��
	size_t sz;

public:

	/// �ω���Ԃ�����킷enum
	enum Transition { IL_CONTINUE_ENABLED=0, IL_NEWLY_ENABLED, IL_DISABLED };
  
	/// �R���X�g���N�^
	AxisIndex() {
		for (int i=0; i<N; ++i) { bPrev[i] = false; }
		Clear();
	}

	/// ���ԍ�i���S������
	void Enable(int i)  { bCurr[i] = true; }

	/// ���ԍ�i�̍S������������
	void Disable(int i) { bCurr[i] = false; }

	/// ���ԍ�i���S������Ă��邩�ǂ������擾����
	bool IsEnabled(int i) { return bCurr[i]; }

	/// �S�����ԍ����X�g�̃T�C�Y���擾����
	int  size() { return sz; }

	/// �S�����ԍ����X�g��n�Ԗڂ̎��ԍ����擾����
	int& operator[](int n){ return idx[n]; }

	/// ���ԍ�i�̍S�����p�����Ă��邩�ǂ�����Ԃ�
	bool IsContinued(int i) {
		return(bCurr[i] && bPrev[i]);
	}

	/// �S���t���O����S�����ԍ����X�g���쐬����
	void CreateList() {
		for (int i=0; i<N; ++i) { if (bCurr[i]) { idx[sz++] = i; } }
	}

	/// �S�����ԍ����X�g���N���A
	void Clear() {
		sz = 0;
		for (int i=0; i<N; ++i) {
			bPrev[i] = bCurr[i];
			bCurr[i] = false;
		}
	}
};

}

#endif//PH_AXIS_INDEX_H
