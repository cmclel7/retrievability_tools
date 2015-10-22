__author__ = 'leif'

def is_array(var):
    return isinstance(var, (list))

def is_float(var):
    return isinstance(var, (float))

def is_int(var):
    return isinstance(var, (int))

def is_dict(var):
    return isinstance(var, (dict))

def print_param_line(s,d,f):
    if is_dict(d):
        val = d[s]
    else:
        val = d

    if str(val):
        if is_float(val):
            f.write('<%s>%.2f</%s>\n' % (s,val,s))
        else:
            if is_int(val):
                f.write('<%s>%d</%s>\n' % (s,val,s))
            else:
                f.write('<%s>%s</%s>\n'  % (s, val, s))




test_cols = {
    'aq': {
        'index':'/Users/leif/Code/indri-test/index',
        'queryFile':'/Users/leif/Code/seekiir-test/queries.trec.title.2005',
        'qrels': '/Users/leif/Code/seekiir-test/TREC2005.qrels.txt',
        'dlens': '/Users/leif/Code/seekiir-test/aquaint.dlens.txt',
        'ddups': '/Users/leif/Code/seekiir-test/aquaint.ddups.txt',
        }
}


models = {
    'bm25': {
        'fixed': {
            'type': '0',
            'model' : '1',
            },
        'param' : 'b'
    },
    'pl2': {
        'fixed':{
            'type': '0',
            'model': '2',
            },
        'param' : 'c'
    },
    'lm': {
        'fixed':{
            'type': '1',
            'model': '1',
            },
        'param' : 'beta'
    },
    'dph': {
        'fixed':{
            'type': '0',
            'model': '4',
            },
        'param' : 'none'
    },
    'tfidf': {
        'fixed':{
            'type': '0',
            'model': '3',
            },
        'param' : 'none'
    }
}
