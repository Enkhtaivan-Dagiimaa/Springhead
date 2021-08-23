#!/usr/local/bin/python
# -*- coding: utf-8 -*-
# ==============================================================================
#  SYNOPSIS:
#	python make_projs_file out-file [env-name [--cmake]]
#	    out-file	�o�͂���(�o�b�`)�t�@�C����
#	    env-name	�ݒ肷��(��)�ϐ���
#
#  DESCRIPTION:
#	������2�ȏ�̂Ƃ�
#	    �v���W�F�N�g���ꗗ���󔒋�؂�̕�����Ƃ��Ċ��ϐ� env-name ��
#	    �ݒ肷��o�b�`�t�@�C���𐶐�����.
#	    �v���W�F�N�g���ꗗ���擾����ɂ� call out-file �Ƃ���.
#	    ���� --cmake ���w�肳�ꂽ�Ƃ���, �o�b�`�t�@�C���̑���� cmake
#	    �� include �\�� cmake text �t�@�C���𐶐�����.
#	������1�̂Ƃ�
#	    �v���W�F�N�g�Ƃ��̈ˑ��֌W��\�����t�@�C��(�� do_swigall.projs ��
#	    �����`��)�̃e�L�X�g�t�@�C���𐶐�����.
#
#	�v���W�F�N�g����, "../../RunSwig/project_dependencies.py" �ɒ�`���ꂽ
#	�v���W�F�N�g�ˑ��֌W��`����擾�������̂� Base �����������̂ł���.
#
# ==============================================================================
#  VERSION:
#     Ver 1.0	 2021/07/19 F.Kanehori	do_swigall.projs �� python script ��.
# ==============================================================================
version = '1.0'
debug = False

import sys
import os

# ----------------------------------------------------------------------
#  ���̃X�N���v�g�� ".../core/src/SprCSharp/RunSwig_CSharp" �ɒu��.
#
ScriptFileDir = os.sep.join(os.path.abspath(__file__).split(os.sep)[:-1])
prog = sys.argv[0].replace('/', os.sep).split(os.sep)[-1].split('.')[0]
SrcDir = '/'.join(ScriptFileDir.split(os.sep)[:-2])

# ----------------------------------------------------------------------
#  �����̏���
#
projs_dept = False
make_batch = False
if len(sys.argv) == 2:
	projs_dept = True
if len(sys.argv) >= 3:
	make_batch = True
	cmake = False
	if (len(sys.argv) == 4 and sys.argv[3] == '--cmake'):
		cmake = True
if not projs_dept and not make_batch:
	print('Error: invalid number of arguments')
	print('Usage: %s out-file             # for projs-dept' % prog)
	print('       %s out-file env-name    # for make-batch' % prog)
	sys.exit(1)
if projs_dept:
	out_file = sys.argv[1]
	if debug:
		print('  make project depndency list file.')
		print('    out-file: %s' % out_file)
if make_batch:
	out_file = sys.argv[1]
	env_name = sys.argv[2]
	if debug:
		print('  make setting all projects batch file.')
		print('    out-file: %s' % out_file)
		print('    env-name: %s' % env_name)
		print('    cmake:    %s' % cmake)

# ----------------------------------------------------------------------
#  �v���W�F�N�g�ˑ��֌W��`�t�@�C������荞��.
#
sys.path.append('%s/RunSwig' % SrcDir)
from project_dependencies import *

# ----------------------------------------------------------------------
#  �t�@�C���𐶐�����.
#
if projs_dept:
	with open(out_file, 'w') as f:
		for key in sorted(projs):
			depts = proj_depts[key]
			if depts == 'None':
				depts = ''
			f.write('%s %s\n' % (key, depts))
			if debug:
				print('  %s %s' % (key, depts))

if make_batch:
	#  ���ϐ� env-name ��ݒ肷��o�b�`�t�@�C��
	#	set <env-name>=<proj proj ... proj>
	#  �܂���
	#  cmake �� include �ł���e�L�X�g�t�@�C��
	#	set(<env-name> <proj proj ... proj>)
	#
	all_projects = ['Base']
	all_projects.extend(sorted(projs))
	project_list = ' '.join(all_projects)
	if debug:
		print('  projects: %s' % project_list)
	#  �t�@�C���ɏ����o��.
	#
	with open(out_file, 'w') as f:
		if cmake:
			f.write('set(%s %s)\n' % (env_name, project_list))
		else:
			f.write('set %s=%s\n' % (env_name, project_list))

sys.exit(0)

#end: make_projs_file.py
