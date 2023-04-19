import matplotlib.pyplot as plt
import random
import pandas as pd
import xgboost
from sklearn.model_selection import train_test_split
from sklearn.metrics import accuracy_score
from predictors import *
from os.path import exists
from xgboost import plot_importance, plot_tree

# CONFIG
m = 4 # m sets the size of the predictor tables
n = 3 # n is the bit amounts for the predictors (n <= m)
smith_n = 2 # n for smith predictor
W = 12 # length of the features window
S = 4 # query the model every S segments
tracefile = "traces/gcc_big.txt"
data_amount = 500_000
use_other_input = True
pickle_data = "./data_pickle.pkl"

# Create Pandas Dataframe (if not already created)
if not exists(pickle_data):
    # Read CSV to Dataframe
    if not use_other_input:
        df = pd.read_csv(tracefile, sep = " ", header = None, names =  ["addresses","outcome"])
        # Transform taken or not taken to binary
        df['binary'] = df.apply(lambda row: 1 if row['outcome'] == "t" else 0, axis = 1)
    else:
        df = pd.read_csv(tracefile, sep = "\t", header = None, names = ["target", "outcome", "conditional", "call", "ret", "direct", "garbage", "addresses"])
        df['binary'] = df['outcome']
        print(df.head())

    # Convert the binary and address columns to normal lists (for use in the predictors)
    binary_list = df["binary"].values.tolist()
    address_list = df["addresses"].values.tolist()

    # Take the rolling sum for the past W rows (for each row)
    df['taken'] = df['binary'].rolling(window = W).sum()
    df.fillna(0)
 
    # Reverse taken and place in not_taken
    df['not_taken'] = W - df['taken']

    # Volatility is determined how many times the binary flips from 0 to 1 or 1 to 0 in the past W rows.
    df['volatility'] = df['binary'].diff().abs().rolling(window = W).sum()

    # Get the mispredict list from the bimodal predictor
    mispredicts = bimodal(m, binary_list, address_list)
    # Set the percentage correct for each window
    df['bimodal_misspredicts'] = pd.Series(mispredicts)
    df['bimodal_window'] = 1-(df['bimodal_misspredicts'].rolling(window = W).sum()/W)

    # Get the mispredict list from the gshare predictor
    mispredicts = gshare(m, n, binary_list, address_list)
    # Set the percentage correct for each window
    df['gshare_misspredicts'] = pd.Series(mispredicts)
    df['gshare_window'] = 1-(df['gshare_misspredicts'].rolling(window = W).sum()/W)
    
    # Get the mispredict list from the smith predictor
    mispredicts = smith(smith_n, binary_list)
    # Set the percentage correct for each window
    df['smith_misspredicts'] = pd.Series(mispredicts)
    df['smith_window'] = 1-(df['smith_misspredicts'].rolling(window = W).sum()/W)

    print("bimodal ", df['bimodal_misspredicts'].sum())
    print("gshare ", df['gshare_misspredicts'].sum())
    print("smith ", df['smith_misspredicts'].sum())
    
    # Choose the best predictor from each row (will be saved as a string)
    df['best_predictor'] = df[['bimodal_window', 'gshare_window', 'smith_window']].idxmax(axis='columns')
    df.fillna(value = 0, inplace = True)
    df['oracle'] = df.apply(lambda row: row[row['best_predictor']], axis = 1)

    # Bimodal, Gshare, and Smith accuracies for next S segments, for the window
    df['bimodal_next_S'] = 1 - (df['bimodal_misspredicts'].shift(-(S)).rolling(window = S).sum() / S)
    df['gshare_next_S'] = 1 - (df['gshare_misspredicts'].shift(-(S)).rolling(window = S).sum() / S)
    df['smith_next_S'] = 1 - (df['smith_misspredicts'].shift(-(S)).rolling(window = S).sum() / S)

    # Choose the best future predictor from each row (will be saved as a string)
    df['best_predictor_next_S'] = df[['bimodal_next_S', 'gshare_next_S', 'smith_next_S']].idxmax(axis='columns')

    # Replace all NaN values with 0
    df.fillna(value = 0, inplace = True)

    # Convert the string to a number
    def best_predictor_index(row):
        if row['best_predictor'] == 0: return 2
        if row['best_predictor'][0] == "b": # bimodal
            return 0
        if row['best_predictor'][0] == "g": # gshare
            return 1
        if row['best_predictor'][0] == "s": #smith
            return 2
        return 2

    # Convert the string to a number (again)
    def best_predictor_index_2(row):
        if row['best_predictor_next_S'] == 0: return 2
        if row['best_predictor_next_S'][0] == "b": # bimodal
            return 0
        if row['best_predictor_next_S'][0] == "g": # gshare
            return 1
        if row['best_predictor_next_S'][0] == "s": #smith
            return 2
        return 2

    # Use the above functions to convert the strings in the two columns to numbers (one hot encoding basically)
    df['best_predictor'] = df.apply(best_predictor_index,axis = 1) 
    df['best_predictor_next_S'] = df.apply(best_predictor_index_2,axis = 1) 

    # Save the dataframe
    df.to_pickle(pickle_data)
else:
    # Load the dataframe
    df = pd.read_pickle(pickle_data)

# Split the dataframe into training and testing sets
train, test = train_test_split(df[:data_amount], test_size=0.2, shuffle = False)
train = train.sample(frac = 1).reset_index(drop=True)

# Choose the features the model will use
if not use_other_input:
    features = ["taken", "not_taken", "volatility", "best_predictor"]
else:
    features = ["taken", "not_taken", "volatility", "best_predictor", "conditional", "call", "ret", "direct"]

# Seperate the inputs and outputs to the training and testing data
x_train = train[features]
y_train = train[['best_predictor_next_S']]
print(train[['best_predictor_next_S']][:50])

x_test = test[features]
y_test = test[['best_predictor_next_S']]

PARAMS = {
'booster': "dart",
'base_score': 1,
'gamma': 0.01,
'learning_rate': 0.02,
'max_depth': 6,
'min_child_weight': 1,
'n_estimators': 100,
'seed': 42,
'objective': 'multi:softmax',
'lambda': 0.01,
'seed': 42
}
if not exists('trained_model1.model'):
    # Create, fit, and save the model
    model = xgboost.XGBClassifier(PARAMS)
    model.fit(x_train, y_train)
    model.save_model('trained_model1.model')
else:
    # Load the model
    model = xgboost.XGBClassifier()
    model.load_model('trained_model1.model')

# Predict which predictor to use!
predictions = model.predict(x_test)
# predictions = test['best_predictor_next_S'].values.tolist()

print(predictions[:500])
print(sum(predictions))
# Find the model accuracy
accuracy = accuracy_score(y_test, predictions)
print("Model accuracy: ", accuracy)

# For each row, select chosen predictor and compare with actual
mispredictions = 0
using_predictor = 0
# Using the predictors' prediction data
column_list = ["bimodal_misspredicts", "gshare_misspredicts", "smith_misspredicts"]

# Go over the rows in the testing range (every segment)
for i in range(train.__len__(), train.__len__() + test.__len__()):  
    # Get the misprediction value from the location (current row, column of current predictor)
    mispredictions += df.iloc[i][column_list[using_predictor]]
    if (i % S == 0):
        # Choose the predictor for the next S segments
        using_predictor = predictions[i - train.__len__()]

# Final results
SmithResult = 1 - df[train.__len__():data_amount]['smith_misspredicts'].sum() / test.__len__()
BimodalResult = 1 - df[train.__len__():data_amount]['bimodal_misspredicts'].sum() / test.__len__()
GshareResult = 1 - df[train.__len__():data_amount]['gshare_misspredicts'].sum() / test.__len__()
OracleResult = df[train.__len__():data_amount]['oracle'].sum() / test.__len__()
OurResult = 1 - mispredictions / test.__len__()
print("RESULTS: ")
print("Smith Predictor %: ", SmithResult)
print("Bimodal Predictor %: ", BimodalResult)
print("Gshare Predictor %: ", GshareResult)
print("Oracle Clairvoyance %: ", OracleResult)
print("Our method %: ", OurResult)

# Plotting The Results
# plt.bar(["Smith", "Bimodal", "Gshare", "Our Method", "Oracle"], 
#          [SmithResult, BimodalResult, GshareResult, OurResult, OracleResult],
#           color=["#37474f", "#37474f", "#37474f", "#acbdc5", "#37474f"])
# plt.ylabel("Prediction Percentage (%)")

pd.Series(model.feature_importances_, index=x_train.columns).plot(kind='barh', color="#37474f", figsize=(12, 6))
plt.xlabel("Feature Importance")

plt.show()