__author__ = 'leif'

from helpers import *

ret_app = '/Users/leif/Code/seekiir/indri/retrieval/IndriRetrieval'
eval_app = 'trec_eval'
tbg_app = 'perl /Users/leif/Code/seekiir-test/tbg.pl'
pres_app = ''

out_dir = '/Users/leif/Code/test/'


params = {
    'b': [0.0,0.1,0.2,0.25,0.3,0.4,0.5,0.55,0.6,0.65,0.7,0.8,0.9,1.0],
    'c': [0.0,1.0,2.0,3.0,5.0,10.0],
    'beta':[100,250,500,750],
    'resultCount': 1000,
    'none':[0],
}


model = 'tfidf'
col = 'aq'

param = models[model]['param']
fixed = models[model]['fixed']
test_col = test_cols[col]

ret_file = ('%s_%s_ret.sh' % (col,model) )
eval_file = ('%s_%s_eval.sh'  % (col,model) )

fr = open(ret_file, 'w')
fe = open(eval_file, 'w')

for p in params[param]:
    result_file = ('%s_%s_%.2f.res' % (col,model,p))
    perf_file = ('%s_%s_%.2f.pr' % (col,model,p))
    tbg_file  = ('%s_%s_%.2f.tbg' % (col,model,p))
    param_file = ('%s_%s_%.2f.param' % (col,model,p))

    fp = open(param_file,'w')
    fp.write('<parameters>\n')
    print_param_line('index', test_col,fp)
    print_param_line('queryFile', test_col,fp)
    print_param_line('resultFile', result_file,fp)
    print_param_line('resultCount', params,fp)
    for i in fixed:
        print_param_line(i, fixed,fp)

    print_param_line(param,p,fp)

    fp.write('</parameters>\n')

    fr.write('%s %s\n' % (ret_app, param_file))
    fe.write('%s %s %s > %s\n' % (eval_app, test_col['qrels'], result_file, perf_file))
    fe.write('%s %s %s %s %s > %s\n' % (tbg_app, test_col['dlens'],test_col['ddups'], test_col['qrels'], result_file, tbg_file))



