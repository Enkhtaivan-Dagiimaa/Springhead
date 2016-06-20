#!/usr/bin/env python
# -*- coding: utf-8 -*-
# ======================================================================
#  FILE:
#	UnityTestMain.py
#
#  DESCRIPTION:
#	Build and run sepcified scenes automatically.
#
#  VERSION:
#	Ver 1.0  2016/06/20 F.Kanehori	First version
# ======================================================================
version = 1.0
import sys
import subprocess
from optparse import OptionParser
from KvFile import KvFile

# ----------------------------------------------------------------------
def s16(value):
	return -(value & 0b1000000000000000) | (value & 0b0111111111111111)

def logmsg(msg, file):
	cmd = 'echo ' + msg + ' >> ' + file
	verbose(cmd, 0)
	return subprocess.call(cmd, shell=True)

def verbose(msg, level=0):
	if options.verbose > level:
		print(msg)

def error(msg):
	sys.stderr.write(script + ': Error: ' + msg + '\n')

# ----------------------------------------------------------------------
#  Process for command line
#
usage = "Usage: %prog [options]"
parser = OptionParser(usage = usage)
parser.add_option('-i', '--init',
			dest='inifile', default='UnityTestMain.ini',
			help='init file name [default: %default]', metavar='FILE')
parser.add_option('-l', '--log',
			dest='logfile',
			help='log file name', metavar='FILE')
parser.add_option('-t', '--timeout',
			dest='timeout', default=15, metavar='TIME',
			help='execution limit time in seconds [default: %default]')
parser.add_option('-T', '--timestamp',
			dest='timestamp', action='store_true', default=False,
			help='add time stamp')
parser.add_option('-1', '--onlyone',
			dest='onlyone', action='store_true', default=False,
			help='execute only one scene at the top of the scene list')
parser.add_option('-v', '--verbose',
			dest='verbose', action='count', default=0,
			help='set verbose mode')
parser.add_option('-V', '--version',
			dest='version', action='store_true', default=False,
			help='show version')
(options, args) = parser.parse_args()

script = sys.argv[0].split('\\')[-1].split('.')[0]
if options.version:
	print('%s: Version %s' % (script, version))
	sys.exit(0)
#
if len(args) != 0:
	parser.error("incorrect number of arguments")

# Scene name
if options.verbose:
	print('  inifile:\t%s' % (options.inifile))

# ----------------------------------------------------------------------
#  Read init file
#
mandatory_keys = ['UnityTestScript', 'Python', 'Springhead2', 'SceneList' ]

kvf = KvFile(options.inifile)
if (kvf.read() < 0):
	sys.exit(-1)
if kvf.check(mandatory_keys) != 0:
	sys.exit(-1)

# set defaults
defaults = {}
spr2root = kvf.get('Springhead2')
defaults['UnityProject'] = spr2root + '/src/Unity'
defaults['TestRoot']	 = spr2root + '/src/UnityTest'
defaults['IniFile']	 = defaults['TestRoot'] + '/UnityTest.ini'
defaults['ScenesDir']	 = 'Assets/Scenes'
defaults['TestMainLogFile'] = defaults['TestRoot'] + '/log/TestMain.log'
for key in defaults.keys():
	if kvf.get(key) is None:
		kvf.set(key, defaults[key])
if options.verbose:
	print('  ------------------------------------------------')
	for key in sorted(mandatory_keys):
		print('  %s:\t%s' % (key, kvf.get(key)))
	print('  ------------------------------------------------')
	for key in sorted(defaults.keys()):
		print('  %s:\t%s' % (key, kvf.get(key)))

# ----------------------------------------------------------------------
#  Execute 'UnityTestScript' for all scenes in 'SecneList'
#
scenes_dir = kvf.get('ScenesDir')
scene_list = kvf.get('SceneList')
if options.verbose:
	print('  scenes_dir:\t%s' % (scenes_dir))
	print('  scene_list:\t%s' % (scene_list))

# command for unity test
script = kvf.get('UnityTestScript')
args = ' -i ' + kvf.get('IniFile')
if options.verbose:	args += ' -v'
if options.timeout:	args += ' -t ' + str(options.timeout)
if options.timestamp:	args += ' -T'

# clear log file
ifile = kvf.get('LogFile')
if ifile is None:
	ifile = kvf.get('TestRoot') + '/log/build.log'
ofile = kvf.get('TestMainLogFile')
print('ofile: %s' % (ofile))
cmd = 'del ' + ofile + ' >NUL 2>&1'
result = subprocess.call(cmd.replace('/', '\\'), shell=True)

# command for log extraction
extract = kvf.get('Python') + ' ' + kvf.get('ExtractScript')
extract += ' ' + ifile + ' >> ' + ofile

# execute build and run
scenes = scene_list.split()
for scene in scenes:
	cmd = script + args + ' ' + scene
	verbose(cmd, 0)
	result = subprocess.call(cmd.replace('/', '\\'), shell=True)
	if result != 0:
		error('unity returns ' + str(s16(result)))
	# log extraction
	logmsg('======================', ofile)
	logmsg(' ' + scene, ofile)
	logmsg('======================', ofile)
	verbose(extract, 0)
	result = subprocess.call(extract.replace('/', '\\'), shell=True)
	# for debug
	if options.onlyone:
		break

sys.exit(s16(result))

# end: UnityTestMain.py
