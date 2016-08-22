import sys

coll=sys.argv[1]
topic=int(sys.argv[2])

bm25_measure_list = [0, 0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8, 0.9, 1]

queries_list=[100,200,300,400,500,600]

content_list=['all','retrieved','pool']

for b in bm25_measure_list:
    for q in queries_list:
        for c in content_list:
            filename = '/Users/kojayboy/Workspace/retrievability_tools/indri_tools/experiment_results/%s/%d/%s/%d/bm25.%.2f.list' % (coll,topic,c,q,b)
            file = open(filename, 'w')
            file.write('/Users/kojayboy/Workspace/retrievability_tools/indri_tools/experiment_results/%s/%d/%s/%d/bm25.%.2f.res\n' % (coll,topic,c,q,b))
            file.close()
            
            filename = '/Users/kojayboy/Workspace/retrievability_tools/indri_tools/experiment_results/%s/%d/%s/%d/bm25.%.2f.rdparams' % (coll,topic,c,q,b)
            file = open(filename, 'w')
            file.write('[inputs] \n\
results_file_list = /Users/kojayboy/Workspace/retrievability_tools/indri_tools/experiment_results/%s/%d/%s/%d/bm25.%.2f.list \n\
docid_list = /Users/kojayboy/Workspace/retrievability_tools/indri_tools/experiment_results/%s/%d/%s/bm25.%.2f.docid \n\
[outputs] \n\
results_directory = /Users/kojayboy/Workspace/retrievability_tools/indri_tools/experiment_results/%s/%d/%s/%d/ \n\
results_output = bm25.%.2f.ret \n\
show_titles = False' % (coll,topic,c,q,b,coll,topic,c,b,coll,topic,c,q,b) )
        file.close()
