import sys

coll=sys.argv[1]
topic=sys.argv[2]

bm25_measure_list = [0, 0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8, 0.9, 1]

for b in bm25_measure_list:
    filename = '/Users/kojayboy/Workspace/retrievability_tools/indri_tools/experiment_results/%s/%s.%s.bm25.%.2f.list' % (coll,coll,topic, b)
    file = open(filename, 'w')
    file.write('/Users/kojayboy/Workspace/retrievability_tools/indri_tools/experiment_results/%s/%s.%s.bigram.bm25.%.2f.res\n' % (coll, coll,  topic, b))
    file.close()

    filename = '/Users/kojayboy/Workspace/retrievability_tools/indri_tools/experiment_results/%s/%s.%sbm25.%.2f.rdparams' % (coll,coll,topic, b)
    file = open(filename, 'w')
    file.write('[inputs] \n\
results_file_list = /Users/kojayboy/Workspace/retrievability_tools/indri_tools/experiment_results/%s/%s.%s.bm25.%.2f.list \n\
docid_list = /Users/kojayboy/Workspace/retrievability_tools/indri_tools/experiment_results/%s/%s.docid \n\
[outputs] \n\
results_directory = /Users/kojayboy/Workspace/retrievability_tools/indri_tools/experiment_results/%s/ \n\
results_output = %s.%s.bigram.bm25.%.2f.ret\n\
show_titles = False'% (coll, coll, topic, b, coll, coll, coll, coll, topic, b))
    file.close()
