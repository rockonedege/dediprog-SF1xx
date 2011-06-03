#!/usr/bin/env python

'''
    zip a whole directory and its sub directories and files
'''
import os,zipfile
from os.path import join
from datetime import date
from time import time

def zipfolder(foldername,filename):
    '''
        zip folder foldername and all its subfiles and folders into
        a zipfile named filename
    '''
    empty_dirs=[]
    zip=zipfile.ZipFile(filename,'w',zipfile.ZIP_DEFLATED)
    for root,dirs,files in os.walk(foldername):
        empty_dirs.extend([dir for dir in dirs if os.listdir(join(root,dir))==[]])
        for filename in files:
            print "compressing",join(root,filename).encode("gbk")
            zip.write(join(root,filename).encode("gbk"))
    for dir in empty_dirs:
        zif=zipfile.ZipInfo(join(root,dir).encode("gbk"+"/"))
        zip.writestr(zif,"")
    zip.close()
    print "Finish compressing"
    
if __name__=="__main__":
    zipfolder("F:\\liferay","liferay.zip")