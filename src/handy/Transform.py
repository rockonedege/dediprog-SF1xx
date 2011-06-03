#!/usr/bin/python
#
# main.py
# Copyright (C) tomgee 2008 <rockonedge@gmail.com>
# 
# main.py is free software: you can redistribute it and/or modify it
# under the terms of the GNU General Public License as published by the
# Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
# 
# main.py is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
# See the GNU General Public License for more details.
# 
# You should have received a copy of the GNU General Public License along
# with this program.  If not, see <http://www.gnu.org/licenses/>.

import os
import sys
import datetime

from string import Template
from os import walk

target_dir = os.pardir #os.getcwd()
output = open('snippet.xml', 'w')

print(sys.argv)
#output.write([v for v in sys.argv))
#output.write("CWD: "os.getcwd())
#output.write("Script: ",sys.argv[0])
#output.write(".EXE: ",os.path.dirname(sys.executable))
#output.write("Script dir: ", os.path.realpath(os.path.dirname(sys.argv[0])))
#pathname, scriptname = os.path.split(sys.argv[0])
#output.write("Relative script dir: ",pathname)
#output.write("Script dir: ", os.path.abspath(pathname))

# print header
output.write("This tool generates TFIT & firmware file Wix Snippets.")
output.write('python version = ' + sys.version + '\n')
output.write('current dir = ' + os.getcwd() + '\n')
output.write('target dir = ' + target_dir + '\n\n')
output.write("\nTimestamp: "+datetime.datetime.now().strftime("%y-%m-%d, %H:%M:%S")+'\n\n')

# file number
i = 0

# print snippet
each_tfit_line = Template(r'<File Id="${id}" Name="${name}.bin"  DiskId="1" Source="..\DediProSetup\Config\${name}.bin" />')
each_fw_line = Template(r'<File Id="${id}" Name="${name}.bin"  DiskId="1" Source="..\DediProSetup\firmware\${name}.bin" />')
for root,folder, filenames in walk(target_dir):
	for filename in filenames:
		#print(filename)
		if filename.startswith('TFIT') and filename.endswith('.bin'):
			name_ = filename.split('.')[0]
			id_ = name_.replace('(', '_').replace(')', '').replace(' ', '_')
			output.write(each_tfit_line.substitute(id = id_, name = name_) + '\n')
			i = i + 1
		elif filename.startswith('SF') and filename.endswith('.bin'):
			name_ = os.path.splitext(filename)[0]
			id_ = name_.replace('-', '_')
			output.write(each_fw_line.substitute(id = id_, name = name_) + '\n')
			i = i + 1
		else:
			pass

# print log			
output.write(Template('\n\n ${count} files processed').substitute(count = i))
output.write("\nTimestamp: "+datetime.datetime.now().strftime("%y-%m-%d, %H:%M:%S"))


