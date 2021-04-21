#!/usr/local/bin/python
# -*- coding: utf-8 -*-
# ==============================================================================
#  �v���W�F�N�g�Ƃ��̈ˑ��֌W�̒�`
#	�ˑ�����v���W�F�N�g����������Ƃ��̓J���}�ŋ�؂�.
#	�ˑ�����v���W�F�N�g���Ȃ��Ƃ��� 'None' �Ƃ���
#
# ==============================================================================
#  Version:
#     Ver 1.00	 2021/03/22 F.Kanehori	do_swigall.projs �� python script ��.
# ==============================================================================
proj_depts = {
	'Collision':	    'Foundation',
	'Creature':	    'Foundation',
	'FileIO':	    'Foundation',
	'Foundation':	    'None',
	'Framework':	    'Physics,Foundation,HumanInterface',
	'Graphics':	    'Foundation',
	'HumanInterface':   'Foundation',
	'Physics':	    'Collision,Foundation'
}
projs = proj_depts.keys()

#end: project_dependencies.py
