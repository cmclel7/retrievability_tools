import sys

def file_namer (res_file):
    path_split=res_file.split('/')
    file_split=path_split[-1].split('.')
    file_split[-1]='gini'
    new_file='.'.join(file_split)
    path_split[-1]=new_file
    new_path='/'.join(path_split)
    return new_path

def gini(list_of_values):
    sorted_list = sorted(list_of_values)
    height, area = 0, 0
    for value in sorted_list:
        height += value
        area += height - value / 2.
    fair_area = height * len(list_of_values) / 2.
    return (fair_area - area) / fair_area

def file_reader(res_file):
    scores=[]
    with open(res_file) as rf:
        for columns in (line.strip().split() for line in rf):
            scores.append(int(columns[4]))
    return scores;
            
scores=file_reader(sys.argv[1])
gini=gini(scores)
print gini
out_file = file_namer(sys.argv[1])
print out_file
with open(out_file, 'w') as nf:
    nf.write(str(gini)+'\n')
