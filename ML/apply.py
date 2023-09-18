### 
### Classification of duplicated collisions with ML
### Usage of hipe4ml library (https://github.com/hipe4ml/hipe4ml)
### Script to perform the application
###

import pandas as pd
import numpy as np
import xgboost as xgb
import matplotlib.pyplot as plt
import uproot
from hipe4ml.model_handler import ModelHandler
from hipe4ml.tree_handler import TreeHandler

import os
import argparse

########################

########################
def main(model_path='',
         app_tree_path='', my_case='',
         suffix=''):

    print('\n=== main function for ML model APPLICATION ===')
    
    ### retrieve the trained model
    model_path = model_path + '/modelHandler.pickle'
    model_path = os.path.expanduser(model_path)
    print(f'   Getting the trained model from {model_path}')
    ModelHandl = ModelHandler()
    ModelHandl.load_model_handler(model_path)

    ### retrieve the application ROOT TTree and convert it to be handled
    app_tree = TreeHandler(app_tree_path, 'dpgCollsBigML')
    
    ### application
    #yPred = ModelHandl.predict(app_tree)
    #app_tree.get_data_frame()['BDT'] = yPred
    app_tree.apply_model_handler(ModelHandl, False)
    app_tree.write_df_to_root_files(f'appliedBDT_{my_case}{suffix}','dpgCollsBigML',"./")


########################
if __name__ == "__main__":
    ### arguments
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('train_dir',
                        help='Directory where the trained model (.pickle) is saved')
    parser.add_argument('--suffix', '-s',
                        default='',
                        help='Suffix to be added to the output files')
    parser.add_argument('--app_tree',
                        #default='../produceMLTrees/treeLHC23d1f.root',
                        #default='../produceMLTrees/treeLHC22l1b2_all_397_399_403_414.root',
                        default='../produceMLTrees/PbPb/data/treeLHC22s_pass5.root',
                        help='ROOT TTree to be used for the application')
    parser.add_argument('--case', '-c',
                        default='LHC22s_pass5',
                        help='Case the chosen application TTree refers to')

    args = parser.parse_args()
    main(args.train_dir, args.app_tree, args.case, args.suffix)

