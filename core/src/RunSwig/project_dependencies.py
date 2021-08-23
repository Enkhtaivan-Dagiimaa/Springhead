#!/usr/local/bin/python
# -*- coding: utf-8 -*-
# ==============================================================================
#  �v���W�F�N�g�Ƃ��̈ˑ��֌W�̒�`
#	�ˑ�����v���W�F�N�g����������Ƃ��̓J���}�ŋ�؂�.
#	�ˑ�����v���W�F�N�g���Ȃ��Ƃ��� 'None' �Ƃ���
#
# -----------------------------------------------------------------------------
#  VERSION:
#     Ver 1.0	 2021/03/22 F.Kanehori	do_swigall.projs �� python script ��.
#     Ver 1.1	 2021/07/26 F.Kanehori	�ˑ��֌W������.
# ==============================================================================
proj_depts = {
	'Collision':	    'Foundation',
	'Creature':	    'Physics,Foundation',
	'FileIO':	    'Foundation',
	'Foundation':	    'None',
	'Framework':	    'FileIO,Physics,Foundation,Graphics,HumanInterface',
	'Graphics':	    'Foundation',
	'HumanInterface':   'Foundation',
	'Physics':	    'Collision,Foundation'
}
projs = proj_depts.keys()

#end: project_dependencies.py
