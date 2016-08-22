#!/usr/bin/python

import sys

def build_map ( pool_file ):
    "Reads in the pooled docs list and adds them to a map"
    pool_map={}
    with open(pool_file) as rf:
        for line in rf:
            print line.strip()
            pool_map[line.strip()]=1
    return pool_map;

def file_namer (res_file):
    path_split=res_file.split('/')
    path_split[-2]='pool'
    file_split=path_split[-1].split('.')
#    file_split[-1]='docid'
    new_file='.'.join(file_split)
    path_split[-1]=new_file
    new_path='/'.join(path_split)
    return new_path

def filter_res ( res_file, pool_map ):
    "takes the pool map and res file and filters accordingly"
    new_file=file_namer(res_file)
    with open(res_file) as rf, open(new_file,'w') as nf:
        for line in rf:
            docid=line.split()[2].strip()
            if docid in pool_map:
                print line
                nf.write(line)
            else:
                print "no match"
    print "Written to: " + new_file
    return ;

if len(sys.argv) < 3:
    print 'Usage: pooled_docs_res.py pooled_docs results_file'
    sys.exit()
print 'pooled documents file: ', sys.argv[1]
print '.res file: ', sys.argv[2]

pool_map=build_map(sys.argv[1])
filter_res(sys.argv[2],pool_map)
