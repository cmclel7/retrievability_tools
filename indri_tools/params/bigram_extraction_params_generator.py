import sys

coll=sys.argv[1]
topic=sys.argv[2]

filename = '/Users/kojayboy/Workspace/retrievability_tools/indri_tools/params/%s.%s.bigram.params' % (coll, topic)
file = open(filename, 'w')
file.write('<parameters>\n\
<index>/Users/kojayboy/Workspace/retrievability_tools/indri_tools/indexes/%s/</index>\n\
<outFile>/Users/kojayboy/Workspace/retrievability_tools/indri_tools/result_files/%s/%s.%s.bigrams</outFile>\n\
<docsFile>/Users/kojayboy/Workspace/topics/%s/qrels.%s</docsFile>\n\
<maxdocs>0</maxdocs> \n\
<cutoff>5</cutoff> \n\
</parameters>' %(coll,coll,coll,topic,coll,topic))
file.close()
