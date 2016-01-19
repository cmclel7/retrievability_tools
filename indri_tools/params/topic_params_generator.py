import sys

coll=sys.argv[1]
topic=sys.argv[2]

bm25_measure_list = [0, 0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8, 0.9, 1]

for b in bm25_measure_list:
    filename = '/Users/kojayboy/Workspace/retrievability_tools/indri_tools/experiment_results/%s/%s.%s.bm25.%.2f.params' % (coll,coll,topic, b)
    file = open(filename, 'w')
    file.write('<parameters> \n\
<index>/Users/kojayboy/Workspace/retrievability_tools/indri_tools/indexes/%s/</index> \n\
<queryFile>/Users/kojayboy/Workspace/retrievability_tools/indri_tools/result_files/%s/%s.%s.bigrams.qry</queryFile> \n\
<resultCount>100</resultCount> \n\
<model>1</model> \n\
<b>%.2f</b> \n\
<maxdocs>0</maxdocs> \n\
<resultFile>/Users/kojayboy/Workspace/retrievability_tools/indri_tools/experiment_results/%s/%s.%s.bigram.bm25.%.2f.res</resultFile> \n\
</parameters>'% (coll,coll,coll,topic,b,coll,coll,topic,b))
    file.close()
