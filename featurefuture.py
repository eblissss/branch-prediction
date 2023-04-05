import pandas as pd
import xgboost
from sklearn.model_selection import train_test_split
from sklearn.metrics import accuracy_score
from predictors import *
from os.path import exists

# CONFIG
m = 5 # m sets the size of the predictor tables
n = 4 # n is the bit amounts for the predictors (n <= m)
W = 50 # length of the features window
S = 5 # query the model every S segments
tracefile = "traces/gcc_trace.txt"
pickle_data = "./data_pickle.pkl"
data_amount = 700_000

# UNUSED CODE:
# class Counter:
#     def __init__(self, largest_bit):
#         self.count = largest_bit
#     def inc(self):
#         self.count += 1
#     def dec(self):
#         self.count -= 1

# # State for predictors
# smith_counter = Counter(largest_bit = 1 << (n - 1))
# bimodal_table = [4] * (1 << m)
# gshare_table = [4] * (1 << m)
# gshare_bhr = 0

# Create Pandas Dataframe (if not already created)
if not exists(pickle_data):
    # Read CSV to Dataframe
    df = pd.read_csv(tracefile, sep = " ", header = None, names =  ["addresses","outcome"])

    # Transform taken or not taken to binary
    df['binary'] = df.apply(lambda row: 1 if row['outcome'] == "t" else 0, axis = 1)

    # Take the rolling sum for the past W rows (for each row)
    df['taken'] = df['binary'].rolling(window = W).sum()
    df.fillna(0)

    # Reverse taken and place in not_taken
    df['not_taken'] = W - df['taken']

    # Volatility is determined how many times the binary flips from 0 to 1 or 1 to 0 in the past W rows.
    df['volatility'] = df['binary'].diff().abs().rolling(window = W).sum()

    # Convert the binary and address columns to normal lists (for use in the predictors)
    binary_list = df["binary"].values.tolist()
    address_list = df["addresses"].values.tolist()

    # Get the mispredict list from the bimodal predictor
    mispredicts = bimodal(m, binary_list, address_list)
    # Convert the list to a Pandas series
    df['bimodal_misspredicts'] = pd.Series(mispredicts)
    # Set the percentage correct for each window
    df['bimodal_window'] = 1-(df['bimodal_misspredicts'].rolling(window = W).sum()/W)

    print("bimodal ", df['bimodal_misspredicts'].sum())

    # Get the mispredict list from the gshare predictor
    mispredicts = gshare(m, n, binary_list, address_list)
    # Convert the list to a Pandas series
    df['gshare_misspredicts'] = pd.Series(mispredicts)
    # Set the percentage correct for each window
    df['gshare_window'] = 1-(df['gshare_misspredicts'].rolling(window = W).sum()/W)

    print("gshare ", df['gshare_misspredicts'].sum())

    # Get the mispredict list from the smith predictor
    mispredicts = smith(n, binary_list)
    # Convert the list to a Pandas series
    df['smith_misspredicts'] = pd.Series(mispredicts)
    # Set the percentage correct for each window
    df['smith_window'] = 1-(df['smith_misspredicts'].rolling(window = W).sum()/W)

    print("smith ", df['smith_misspredicts'].sum())

    # Choose the best predictor from each row (will be saved as a string)
    df['best_predictor'] = df[['bimodal_window', 'gshare_window', 'smith_window']].idxmax(axis='columns')

    # Bimodal, Gshare, and Smith accuracies for next S segments, for the window
    df['bimodal_next_S'] = 1 - (df['bimodal_misspredicts'].shift(-(S+W)).rolling(window = W+S).sum() / W)
    df['gshare_next_S'] = 1 - (df['gshare_misspredicts'].shift(-(S+W)).rolling(window = W+S).sum() / W)
    df['smith_next_S'] = 1 - (df['smith_misspredicts'].shift(-(S+W)).rolling(window = W+S).sum() / W)

    # Choose the best future predictor from each row (will be saved as a string)
    df['best_predictor_next_S'] = df[['bimodal_next_S', 'gshare_next_S', 'smith_next_S']].idxmax(axis='columns')

    # Replace all NaN values with 0
    df.fillna(value = 0, inplace = True)

    # Convert the string to a number
    def best_predictor_index(row):
        if row['best_predictor'] == 0: return 0
        if row['best_predictor'][0] == "b": # bimodal
            return 0
        if row['best_predictor'][0] == "g": # gshare
            return 1
        if row['best_predictor'][0] == "s": #smith
            return 2
        return 0

    # Convert the string to a number (again)
    def best_predictor_index_2(row):
        if row['best_predictor_next_S'] == 0: return 0
        if row['best_predictor_next_S'][0] == "b": # bimodal
            return 0
        if row['best_predictor_next_S'][0] == "g": # gshare
            return 1
        if row['best_predictor_next_S'][0] == "s": #smith
            return 2
        return 0

    # Use the above functions to convert the strings in the two columns to numbers (one hot encoding basically)
    df['best_predictor'] = df.apply(best_predictor_index,axis = 1) 
    df['best_predictor_next_S'] = df.apply(best_predictor_index_2,axis = 1) 

    print(df['best_predictor'].head())
    #print(df)
    #df.head(10)
    #with pd.option_context('display.max_rows', None,'display.max_columns', None, 'display.precision', 2):
    #    print(df)

    # Save the dataframe
    df.to_pickle(pickle_data)
else:
    # Load the dataframe
    df = pd.read_pickle(pickle_data)

# Split the dataframe into training and testing sets
train, test = train_test_split(df[:data_amount], test_size=0.2)

# Choose the features the model will use
features = ["taken", "not_taken", "volatility", "best_predictor"]

# Seperate the inputs and outputs to the training and testing data
x_train = train[features]
y_train = train['best_predictor_next_S']

x_test = test[features]
y_test = test['best_predictor_next_S']

if not exists('trained_model1.model'):
    # Create, fit, and save the model
    model = xgboost.XGBClassifier()
    model.fit(x_train, y_train)
    model.save_model('trained_model1.model')
else:
    # Load the model
    model = xgboost.XGBClassifier()
    model.load_model('trained_model1.model')

# Predict which predictor to use!
predictions = model.predict(x_test)

# print(predictions[:50])

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
    # Every segment
    if (i % S == 0):
        # Choose the predictor for the next S segments
        using_predictor = predictions[i - train.__len__()]
    # Get the misprediction value from the location (current row, column of current predictor)
    mispredictions += df.iloc[i, df.columns.get_loc(column_list[using_predictor])]

# Final results
print("RESULTS: ")
print("Smith Predictor %: ", 1 - df[train.__len__():data_amount]['smith_misspredicts'].sum() / test.__len__())
print("Bimodal Predictor %: ", 1 - df[train.__len__():data_amount]['bimodal_misspredicts'].sum() / test.__len__())
print("Gshare Predictor %: ", 1 - df[train.__len__():data_amount]['gshare_misspredicts'].sum() / test.__len__())
print("Our method %: ", 1 - mispredictions / test.__len__())
