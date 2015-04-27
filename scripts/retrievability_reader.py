#!/usr/bin/python

import sys

#---------------------------------

def build_map ( rd_file ):
    "Reads in the r(d) scores for each doc and stores them into a map."
    rd_map={}
    with open(rd_file) as rf:
        for line in rf:
            line_split=line.split()
            print line_split[0] + " " + line_split[1]
            rd_map[line_split[0]] = int(line_split[1])
    return rd_map;

#---------------------------------

def get_rd_ranks( res_file, rd_map ):
    "Takes in the map of retrievability scores and the document scores and returns the documents with their r(d) score."
    rd_list=[]
    res_list=[]
    query_map={}
    res_map={}
    prev_q=1
    with open(res_file) as rf:
        for line in rf: 
            line_split=line.split()
            q = int(line_split[0])
            if prev_q != q:
                #rd_ranking=sorted(rd_list,key=lambda x: x[1])
                query_map[prev_q]=rd_list
                res_map=[prev_q]=res_list
                rd_list=[]
                res_list=[]
                prev_q=q
            rd_list.append((line_split[2], rd_map.get(line_split[2])))
            res_list.append((line_split[2], float(line_split[4])))
    print len(res_map.get(1))
    return query_map;

#---------------------------------

if len(sys.argv) < 4:
    print 'Usage: retrievability_re-ranker.py results_file retrievability_scores alpha'
    sys.exit()
print 'Retrieval Result File: ', sys.argv[1]
print 'Retrievability File: ', sys.argv[2]
print 'Alpha: ', sys.argv[3]

#---------------------------------

rd_map=build_map(sys.argv[2])
rd_ranking=get_rd_ranks(sys.argv[1], rd_map)