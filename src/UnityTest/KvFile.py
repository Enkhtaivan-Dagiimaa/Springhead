#!/usr/bin/env python
# -*- coding: utf-8 -*-
# ======================================================================
#  FILE:
#	KvFile.py
#
#  DESCRIPTION:
#	Make dictionary by reading key-value pair file.
#
#  VERSION:
#	Ver 1.0  2016/06/13 F.Kanehori	First version
#	Ver 1.1  2016/06/20 F.Kanehori  Add %include function
#					Allow continued line
# ======================================================================
import sys
import re

# ----------------------------------------------------------------------
#  class KvFile			Key-value pair file class
# ----------------------------------------------------------------------
#  constructor
#	KvFile(filename)
#	    filepath		Key-value pair file path
#
#  methods
#	num_pairs = read()	Read the file and returns effective
#				key-value pair number
#	set(key, value)		Register value corresponding the key
#	value = get(key)	Return the value for the key
# ----------------------------------------------------------------------
class KvFile:
	def __init__(self, path, verbose = 0):
		self.path = path
		self.dict = {}
		self.verbose = verbose

	def set(self, key, value):
		self.dict[key] = value

	def get(self, key):
		try:
			value = self.dict[key]
		except KeyError:
			# key undefined
			value = None
		return value	

	def read(self, path=None):
		if path is None:
			path = self.path
		count = 0
		nlines = 0
		try:
			f = open(path, 'r')
		except IOError as err:
			# can't open file
			self.__error('%s' % (err))
			return -1
		prev_line = ''
		for line in f:
			nlines += 1
			if re.match('^\s*$', line): continue
			if re.match('^\s*#', line): continue
			if self.verbose > 1:
				print(str(nlines) + ': ' + line.rstrip())
			# continued line
			m = re.match('(.*)(.)$', line)
			if m and m.group(len(m.groups())) == '\\':
				prev_line += m.group(1)
				continue
			else:
				line = prev_line + line
				if self.verbose and prev_line != '':
					print('CONT\'D: [%s]' % (line.rstrip()))
				prev_line = ''
			# %include
			m = re.match('^%include\s+([\w.]+)', line)
			if m:
				if self.verbose:
					print('%%include %s' % m.group(1))
				self.read(m.group(1))
				continue
			# key-value pair
			pair = line.rstrip().split()
			if len(pair) < 2:
				msg = self.path + ': ' + str(nlines)
				self.__error(msg)
				continue
			key = pair[0]
			val = ' '.join(pair[1:])
			val = self.__expand(val)
			self.dict[key] = val
			count += 1
		f.close()
		return count

	def check(self, keys):
		result = 0
		for key in keys:
			val = self.get(key)
			if val is None:
				self.__error("required key '" + key + "' missing")
				result = -1
		return result

	def show(self, line_by_line = 0):
		if line_by_line:
			for key in sorted(self.dict):
				key_part = ' ' * line_by_line + key
				val_part = ' \t' + self.dict[key]
				print(key_part + val_part)
		else:
			print(self.dict)

	def __expand(self, str):
		m = re.match('([^\$]*)\$\(([^\)]+)\)(.*$)', str)
		#print(m)
		if m:
			if self.verbose:
				group = [m.group(1), m.group(2), m.group(3)]
				msg = 'MATCH: \'' + m.group(0) + '\' => '
				msg += '[ ' + ', '.join(group) + ' ]'
				print(msg)
			val = self.get(m.group(2))
			if not val is None:
				str = m.group(1) + val + m.group(3)
				str = self.__expand(str)
		return str

	def __error(self, msg):
		sys.stderr.write('KvFile: Error: ' + msg + '\n')

# ----------------------------------------------------------------------
#  �e�X�g���C��
# ----------------------------------------------------------------------
if __name__ == '__main__':

	verbose = 0
	kvf = KvFile('UnityTestMain.ini', verbose)
	kvf.read()
	kvf.show(2)
	keys = ['Springhead2', 'UnityProject', 'OutFile', 'GhostKey']
	if kvf.check(keys) != 0:
		print('Not all keys are defined !!')
	for key in keys:
		val = kvf.get(key)
		if not val is None:
			print(key + ':\t' + kvf.get(key))

	sys.exit(0)

# end: KvFile.py