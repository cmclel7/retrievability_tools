__author__ = 'Kojayboy'

import sys, fileinput, gc
from abstract_config_reader import AbstractConfigReader
from abstract_config_reader import BadConfigError
from retrievability_ruler import  RetrievabilityMeasure
from retrievability_ruler import  RetrievabilityRuler
from trec_result_handler import process_trec_line

class RetrievabilityConfigReader(AbstractConfigReader):
    '''
    Config reader class for the query generation module
    A derived class from abstract class framework.common.abstract_config_reader.AbstractConfigReader.
    Provides the settings that are expected for an indexing config file.
    '''
    def __init__(self, filename):
        self.settings = {
        'inputs': {
        'results_file_list': (str, None),
        'docid_list': (str, None)
        },
        'variables': {
        'beta': (int, 0),
        'cutoff': (int, 0),
        'queries': (int, 0),
        'reverse': (int, 0)
        },
        'outputs': {
        'results_directory': (str, ''),
        'results_output': (str, 'retrievability_results'),
        'show_titles': (bool, False)
        },
        'batch_settings': {
        'no_cores': (int, 1),
        'memory': (int, 128)
        }
        }

        super(RetrievabilityConfigReader, self).__init__(filename)


def usage():
    '''
    Prints the script's usage details to the console.
    '''
    print "Usage:"
    print "  %s config_file " % (argv[0])
    print "Where:"
    print "  config_file: Location of configuration file"
    # print out the parameters,
    RetrievabilityConfigReader.print_params()

def run_retrievability(config_dict_path):

    config_dict = RetrievabilityConfigReader(config_dict_path)
    b = config_dict.settings.variables.beta
    c = config_dict.settings.variables.cutoff
    titles = config_dict.settings.outputs.show_titles
    directory = config_dict.settings.outputs.results_directory
    output_file = config_dict.settings.outputs.results_output
    docid_file = config_dict.settings.inputs.docid_list
    result_list = config_dict.settings.inputs.results_file_list
    queries = config_dict.settings.variables.queries
    reverse = config_dict.settings.variables.reverse

    # Makes document id list.
    doclist = []
    docid_list_file = open(docid_file, 'r')
    for line in docid_list_file:
        doclist.append(line.rstrip())
    ret_ruler = RetrievabilityRuler(doclist)

    block = queries/10

    # Adds measures to the ruler to perform.
    if c == 0:
        ret_ruler.add_measure(RetrievabilityMeasure(0,10))
        ret_ruler.add_measure(RetrievabilityMeasure(0,20))
        ret_ruler.add_measure(RetrievabilityMeasure(0,50))
        ret_ruler.add_measure(RetrievabilityMeasure(0,100))
        ret_ruler.add_measure(RetrievabilityMeasure(0.5,100))
        ret_ruler.add_measure(RetrievabilityMeasure(1.0,100))
        ret_ruler.add_measure(RetrievabilityMeasure(1.5,100))
        ret_ruler.add_measure(RetrievabilityMeasure(2.0,100))
    elif b >= 0 and c > 0:
        ret_ruler.add_measure(RetrievabilityMeasure(b, c))
    else:
        print "b needs to be  %d >= 0 and c need to be  %d > 0. They are invalid" % (b,c)
        return 2

    input_file_list = open(result_list, 'r')
    i = 1
    for line in input_file_list:    # opens each results file to process
        resfilename = line.rstrip()
        for line2 in fileinput.input([resfilename]):      # performs the process on each doc and rank
            topic, docid, rank, score = process_trec_line(line2)
            ret_ruler.process_document(docid, int(rank))
            if int(topic) == (block * i) and reverse == 0:
                perc=i*10
                filename = directory + str(perc) + "."+ output_file
                print "Saving " + str(perc) + "% File"
                ret_ruler.save_file(filename, titles)
                i = i + 1
            elif int(topic) == (queries - (block * i)) and reverse == 1:
                perc=(10-i)*10
                filename = directory + str(perc) + "."+ output_file
                print "Saving " + str(perc) + "% File"
                ret_ruler.save_file(filename, titles)
                i = i + 1

        fileinput.close()
    input_file_list.close()

    print 'Saving Full File...'

    output_filename = str(directory) + str(output_file)
    ret_ruler.save_file(str(output_filename), titles)

def main(argv=None):

    '''
    The Basic Retrieval application takes an index and query file and returns an result file.

    This main(argv=None) is based on the suggested approach at: http://www.artima.com/weblogs/viewpost.jsp?thread=4829
    '''

    if argv is None:
        argv = sys.argv
    if len(argv) == 2:
        return run_retrievability(argv[1])
    else: # Invalid number of command-line arguments, print usage
        usage(argv)
        return 2

if __name__ == '__main__':
    sys.exit(main())