import sys

def split_file(res_file, qs):
    path_split=res_file.split('/')
    path_split.insert(-1,qs)
    out_file='/'.join(path_split)
    with open(res_file) as rf:
        with open(out_file, 'w') as of:
            for line in rf:
                line_split=line.split()
                if line_split[0] <= qs:
                    of.write(line)
    return ;

split_file(sys.argv[1],sys.argv[2])
