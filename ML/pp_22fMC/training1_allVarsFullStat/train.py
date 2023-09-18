### 
### Classification of duplicated collisions with ML
### Usage of hipe4ml library (https://github.com/hipe4ml/hipe4ml)
### Script to perform the training
###

import numpy as np
import pandas as pd
import xgboost as xgb
import matplotlib.pyplot as plt
from sklearn.model_selection import train_test_split
from hipe4ml.model_handler import ModelHandler
from hipe4ml.tree_handler import TreeHandler
from hipe4ml.analysis_utils import train_test_generator
from hipe4ml import plot_utils


#################################
###   Preparing the dataset   ###
#################################

### reconstructed PVs in MC
#treePV = TreeHandler('../produceMLTrees/treeLHC23d1e.root', 'dpgCollsBigML')
treePvSingle = TreeHandler('../produceMLTrees/treeLHC23d1e_single.root', 'dpgCollsBigML')
treePvSDuplicate = TreeHandler('../produceMLTrees/treeLHC23d1e_duplicate.root', 'dpgCollsBigML')

### separate duplicates from single-reconstructed
#duplicatePV = treePV.get_subset('fIsDuplicate > 0') #and fFt0PosZ > -900')
#singlePV = treePV.get_subset('fIsDuplicate < 1', size=duplicatePV.get_n_cand()*10) ### N times more than duplicates
#
duplicatePV = treePvSDuplicate.get_subset('fIsDuplicate > 0') #and fFt0PosZ > -900')
singlePV = treePvSingle.get_subset('fIsDuplicate < 1') #, size=duplicatePV.get_n_cand()*10)
print(f'>>> input duplicatePV: {duplicatePV.get_n_cand()}')
print(f'>>> input singlePV: {singlePV.get_n_cand()}')
#

### setup training (50% of each dataset) and testing (the other 50% of each dataset)
### training and testing samples are random sampled
train_test_data = train_test_generator([singlePV, duplicatePV], [0,1], test_size=0.5, random_state=42)

######################################################
###   Draw the variables for the two cathegories   ###
######################################################
#vars_to_draw = singlePV.get_var_names()
#print(">>> variables: {}", vars_to_draw)
vars_to_draw = [#'fIsEventSelected', 'fRunNumber',  \
      'fPosX', 'fPosY', 'fPosZ', 'fCovXX', 'fCovXY', 'fCovXZ', 'fCovYY', 'fCovYZ', 'fCovZZ'  \
    , 'fNumContrib', 'fNumTracksAll', 'fNumTracksFiltered', 'fChi2'  \
    #, 'fGlobalBcInRun' \
    , 'fFt0PosZ', 'fSignalFT0A', 'fSignalFT0C', 'fSignalFT0M', 'fSignalV0A' \
    , 'fCollisionTime', 'fCollisionTimeRes' \
    #, 'fCollIDMC', 'fPosXMC', 'fPosYMC', 'fPosZMC', 'fCollisionTimeMC', 'fIsFakeCollision', 'fRecoPVsPerMcColl', 'fIsPvHighestContribForMcColl', 'fIsDuplicate' \
    ]
leg_labels = ['single-reco', 'duplicates']
# variables
FigVars = plot_utils.plot_distr([singlePV, duplicatePV], vars_to_draw, bins=100, labels=leg_labels, log=True, density=True, figsize=(14, 14), alpha=0.3, grid=False)
plt.savefig(f'figureVars.pdf')
plt.subplots_adjust(left=0.03, bottom=0.1, right=0.99, top=0.96, hspace=0.55, wspace=0.55)
# correlation
FigCorr = plot_utils.plot_corr([singlePV, duplicatePV], vars_to_draw, leg_labels)
for fig, lab in zip(FigCorr, leg_labels):
    fig.savefig(f'figureCorr{lab}.pdf')
#plt.show()


######################################
###   Model training and testing   ###
######################################

# training features
features_for_train = [
      'fPosX', 'fPosY',
      'fPosZ', 
      'fCovXX' , 'fCovXY', 'fCovXZ'
    , 'fCovYY' , 'fCovYZ'
    , 'fCovZZ'  \
    , 
      'fNumContrib', 
     'fNumTracksAll'
    , 'fNumTracksFiltered'
     ,
     'fChi2'  \
    , 'fFt0PosZ'
    , 'fSignalFT0A', 'fSignalFT0C' \
    , 'fSignalFT0M', 'fSignalV0A' \
    , 'fCollisionTime' \
    , 'fCollisionTimeRes' 
    ]

# model
model_clf = xgb.XGBClassifier()
model_hdl = ModelHandler(model_clf, features_for_train)

# hyperparameter optimization with optuna (default: 5 folds for cross validation)
hyper_pars_ranges = {'n_estimators': (200, 1000), 'max_depth': (2, 4), 'learning_rate': (0.01, 0.1)} # see https://xgboost.readthedocs.io/en/latest/python/python_api.html
model_hdl.optimize_params_optuna(train_test_data, hyper_pars_ranges, cross_val_scoring='roc_auc', timeout=120, \
                                 n_jobs=16, n_trials=100, direction='maximize')

# model training
model_hdl.train_test_model(train_test_data)

# training predictions for training and test samples
y_pred_train = model_hdl.predict(train_test_data[0], False)
y_pred_test = model_hdl.predict(train_test_data[2], False)

# plot training response
plt.rcParams["figure.figsize"] = (10, 7)
# ML output
ml_out_fig = plot_utils.plot_output_train_test(model_hdl, train_test_data, 100, False, leg_labels, True, density=True)
ml_out_fig.savefig(f'ml_out_fig.pdf')
# ROC curves
roc_train_test_fig = plot_utils.plot_roc_train_test(train_test_data[3], y_pred_test, train_test_data[1], y_pred_train, None, leg_labels)
roc_train_test_fig.savefig(f'roc_train_test_fig.pdf')
# precision recall
precision_recall_fig = plot_utils.plot_precision_recall(train_test_data[3], y_pred_test, leg_labels)
precision_recall_fig.savefig('precision_recall.pdf')
# feature importance
feature_importance_figs = plot_utils.plot_feature_imp(train_test_data[2][features_for_train], train_test_data[3], model_hdl, leg_labels)
for feature_importance_fig in feature_importance_figs:
    feature_importance_fig.savefig(f'feature_importance_{feature_importance_fig.number}.pdf')
# learning curve
learning_curve_fig = plot_utils.plot_learning_curves(model_hdl, train_test_data)
learning_curve_fig.savefig('learning_curve.pdf')

#plt.show()

# save the model
model_hdl.dump_model_handler('modelHandler.pickle')
model_hdl.dump_original_model('XGBoostModel.model', True)