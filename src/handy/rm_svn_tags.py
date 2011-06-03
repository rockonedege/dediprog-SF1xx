import datetime
import sys
import os
from os.path import join

target_dir = os.getcwd()
log = open(join(target_dir, 'remove.log'), 'w')
log.write("This tool removes .svn folders.")
log.write('python version = ' + sys.version + '\n')
log.write('current dir = ' + os.getcwd() + '\n')
log.write('target dir = ' + target_dir + '\n\n')
log.write("\nTimestamp: "+datetime.datetime.now().strftime("%y-%m-%d, %H:%M:%S")+'\n\n')

def rm_everything(folder):
    if os.name == 'nt':
        cmd = 'RD /S/Q "' + folder + '"'
        os.system(cmd);
        log.write(cmd + '\n')
    else:
        pass
        # Delete everything reachable from the directory named in 'top'.
        # CAUTION: This is dangerous! For example, if top == '/', it
        # could delete all your disk files.
##        for root, dirs, files in os.walk(folder, topdown=False):
##            for name in files:
##                os.remove(join(root, name))
##                print(join(root, name))
##            for name in dirs:
##                os.remove(join(root, name))
##                print(join(root, name))
                        

def rm_svnfolder(folder):
    for root, dirs, files in os.walk(folder, topdown=False):
        for f in dirs:
            if f == '.svn':
                rm_everything(join(root, f))
    log.write("\nTimestamp: "+datetime.datetime.now().strftime("%y-%m-%d, %H:%M:%S")+'\n\n')
    
def backup(src, des):
    if os.name == 'nt':
        cmd = 'md ' + des
        os.system(cmd)
        log.write(cmd + '\n')
        cmd = 'XCOPY "' + src + '" "' + des + '" /E/H/R/Y/C ' 
        os.system(cmd);
        log.write(cmd + '\n\n')
    else:
        pass    
    
dedi_src_folder = r'..\dedi-src'
backup(r'..\Deliverables',  dedi_src_folder)
rm_svnfolder( dedi_src_folder )


