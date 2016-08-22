#!/usr/bin/python

import sys
from shutil import copyfile

def build_map ( res_file ):
    "Reads in the retrieved docs list and adds them to a map"
    res_map={}
    out = file_namer2(res_file)
    copyfile(res_file, out)
    with open(res_file) as rf:
            for line in rf:
                res_map[line.split()[2].strip()]=1
    return res_map;

def file_namer (res_file):
    path_split=res_file.split('/')
    path_split[-2]='retrieved'
    file_split=path_split[-1].split('.')
    file_split[-1]='docid'
    new_file='.'.join(file_split)
    path_split[-1]=new_file
    new_path='/'.join(path_split)
    return new_path

def file_namer2 (res_file):
    path_split=res_file.split('/')
    path_split[-2]='retrieved'
    file_split=path_split[-1].split('.')
    new_file='.'.join(file_split)
    path_split[-1]=new_file
    new_path='/'.join(path_split)
    return new_path

if len(sys.argv) < 2:
    print 'Usage: pooled_docs_res.py res_file'
    sys.exit()
print '.res file: ', sys.argv[1]

res_map=build_map(sys.argv[1])
out_file = file_namer(sys.argv[1])
print out_file
with open(out_file, 'w') as nf:
    for key in res_map:
        nf.write(key + '\n')
        
