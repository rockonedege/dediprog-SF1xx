from os import walk
from os.path import join
from os.path import basename
from string import capwords
import datetime 
import sys
import os
    
def GoRename(path):
    """ rename file 111-222-a.b.c.bin -->222-a.b.c-111.bin
        Copyright @ Tomgee, 2008
    """

    print "Default encoding:%s "%sys.getdefaultencoding()
    print "Default encoding:%s "%sys.getfilesystemencoding()
    logfile = path + r'\log.txt'
    print "Going through %s to rename files, \nsubdirectory included..."%(path)
    print "a log will be saved to %s"%logfile
    log = open(logfile, 'a') # open a log file
    sys.stdout = log   # un-comment this line out if the output to a file is preferred 
    
    now = datetime.datetime.now()
    print "\n***********************************************"
    print "Timestamp: %s"%now.strftime("%y-%m-%d, %H:%M:%S")

    totalCount = 0
    for root,dirs, files in walk(path):
        for f in files:
            if (f != os.path.basename(sys.argv[0])) and (f != "log.txt"):
                c = os.path.splitext(f)[0].split("-") 
                c.append(c[0])
                c.pop(0)
                newfile = "-".join(c[:])
                newfile = newfile + ".bin"
                os.rename(os.path.join(root,f), os.path.join(root,newfile))
                print "File #%d: %s --> %s"%(totalCount,f, newfile)
                totalCount += 1
    print "Totally %d files/directories under %s have been searched"%(totalCount, path)
    print r'********* The End *********'

def main():
    argCurDir = os.getcwd()
    GoRename(argCurDir)        
if __name__ == '__main__': main()
    
