# Feature Future Predictor
# Group: Cesar, Lior, Ethan, Wessley, Kat, Zoe, Richard, Tommy

# Organized as follows:
# - Bimodal Branch Predictor Function
# - Gshare Branch Predictor Function
# - Smith Branch Predictor Function
# - Main function

import math

# Bimodal Branch Predictor
def bimodal(m, list):
    table = []
    for i in range(1 << m):
        table.append(4)

    actual_taken = True
    pred_taken = True

    # Stats
    predictions = 0
    mispredictions = 0
    pc = 0

    # Find and predict at each branch
    for line in list:

        if line[7] == 't':
            actual_taken = True
        else:
            actual_taken = False

        # Get PC
        pc = int(line[0:6], 16)

        index = (pc >> 2) % (1 << m)

        # Predict and update
        value = table[index]
        pred_taken = (value >= 4)

        if actual_taken and value < 7:
            table[index] = table[index] + 1

        elif not actual_taken and value > 0:
            table[index] = table[index] - 1

        predictions = predictions + 1
        if actual_taken != pred_taken:
            mispredictions = mispredictions + 1

    return [(predictions - mispredictions) / predictions, mispredictions, predictions]

# Gshare Branch Predictor
def gshare(m, n, list):
    bhr_largest_bit = 1 << (n - 1)

    table = []
    for i in range(1 << m):
        table.append(4)

    actual_taken = True
    pred_taken = True

    # Stats
    predictions = 0
    mispredictions = 0

    bhr = 0
    pc = 0

    # Find and predict at each branch
    for line in list:
        if line[7] == 't':
            actual_taken = True
        else:
            actual_taken = False

        # Get PC
        pc = int(line[0:6], 16)
        # Remove right 2 bits
        pc = pc >> 2

        # Removing last 2 bits, and using m left
        # Adding N-M 0's to bhr
        # XOR
        # n from bhr
        # n from PC
        # m-n from PC
        # shift all of the above until they all have m bits

        # M to N bits of shifted PC
        # n = 3, m = 6
        # 11111111111111111 Original
        # 00011111111111111 Shift Right by N
        # 11111111111111000 Shift Left By N (now right N bits are 0)
        #           1000000 2^M
        # 00000000000111000 Mod by 2^M (bits before Mth place are 0)
        #            ^^^

        index_A = (((pc >> n) << n) % (1 << m))
        # Rightmost N bits of shifted PC (XOR with bhr)
        index_B = (pc % (1 << n)) ^ bhr

        index = index_A + index_B

        # Predict and update
        value = table[index]
        pred_taken = (value >= 4)

        if actual_taken and value < 7:
            table[index] = table[index] + 1

        elif not actual_taken and value > 0:
            table[index] = table[index] - 1

        bhr >>= 1
        if actual_taken:
            bhr += bhr_largest_bit

        predictions = predictions + 1
        if actual_taken != pred_taken:
            mispredictions = mispredictions + 1

    return [(predictions - mispredictions) / predictions, mispredictions, predictions]

# Smith Branch Predictor
def smith(num_bits, list):
    largest_bit = 1 << (num_bits - 1);
    counter = largest_bit;

    actual_taken = True
    pred_taken = True

    # Stats
    predictions = 0
    mispredictions = 0

    # Find and predict at each branch
    for line in list:
        if line[7] == 't':
            actual_taken = True
        else:
            actual_taken = False

        pred_taken = (counter >= largest_bit)

        if (actual_taken and counter < 7):
            counter = counter + 1
        elif (not actual_taken and counter > 0):
            counter = counter + 1

        predictions = predictions + 1
        if actual_taken != pred_taken:
            mispredictions = mispredictions + 1

    return [(predictions - mispredictions) / predictions, mispredictions, predictions]


# Press the green button in the gutter to run the script.
if __name__ == '__main__':

    # Receive input from user
    print("FeatureFuture Predictor\n")
    testing = input("Enter if tracefile is for testing [1 = yes | 0 = no]: ")
    tracefile = input("Enter path of tracefile: ")
    m = input("Enter m integer value: ")
    n = input("Enter n integer value: ")

    # Use provided file for learning
    if testing == 1:
        # Open file to read traces from.
        inputFile = open(tracefile, "r")

        # Create list to hold each line values in tracefile
        list = []

        # Read all lines in trace file.
        while True:
            line = inputFile.readline()
            if line == "":
                break
            list.append(inputFile.readline())

        # Get total number of lines in tracefile
        size = len(list)

        # Test size is 2/3 rds of trace size
        testSize = 2 * math.floor(size * (1 / 3))

        # Get indexes to run three tests on
        segment1 = math.floor(testSize * (1 / 3))
        segment2 = segment1 + segment1
        segment3 = segment2 + segment1

        # Get accuracy information by running tests
        # Returns array:
        #   [(predictions - mispredictions) / predictions, misspredictions, predictions]
        #   index 0: accuracy percentage
        #   index 1: number of miss predictions
        #   index 2: number of total predictions

        # Run tests on first segment
        accuracyGshareSeg1 = gshare(m, n, list[:segment1])
        accuracyBimodalSeg1 = bimodal(m, list[:segment1])
        accuracySmithSeg1 = smith(m, list[:segment1])

        # Record what worked the best on first segment
        results = [accuracyGshareSeg1, accuracyBimodalSeg1, accuracySmithSeg1]
        seg1List = [accuracyGshareSeg1.index(0), accuracyBimodalSeg1.index(0), accuracySmithSeg1.index(0)]
        for i in range(3):
            if seg1List.index(i) == max(seg1List):
                learning = open("LearningTraces/learning.txt", "a")
                learning.write(results[i].index(2) - results[i].index(1) + " " + results[i].index(1) + " " + results[i].index(0))

        # Run tests on second segment
        accuracyGshareSeg2 = gshare(m, n, list[segment1:segment2])
        accuracyBimodalSeg2 = bimodal(m, list[segment1:segment2])
        accuracySmithSeg2 = smith(m, list[segment1:segment2])

        # Record what worked best on second segment
        results = [accuracyGshareSeg2, accuracyBimodalSeg2, accuracySmithSeg2]
        seg2List = [accuracyGshareSeg2.index(0), accuracyBimodalSeg2.index(0), accuracySmithSeg2.index(0)]
        for i in range(3):
            if seg2List.index(i) == max(seg2List):
                learning = open("LearningTraces/learning.txt", "a")
                learning.write(results[i].index(2) - results[i].index(1) + " " + results[i].index(1) + " " + results[i].index(0))

        # Run tests on third segment
        accuracyGshareSeg3 = gshare(m, n, list[segment2:segment3])
        accuracyBimodalSeg3 = bimodal(m, list[segment2:segment3])
        accuracySmithSeg3 = smith(m, list[segment2:segment3])

        # Record what worked best on third segment
        results = [accuracyGshareSeg3, accuracyBimodalSeg3, accuracySmithSeg3]
        seg3List = [accuracyGshareSeg3.index(0), accuracyBimodalSeg3.index(0), accuracySmithSeg3.index(0)]
        for i in range(3):
            if seg3List.index(i) == max(seg3List):
                learning = open("LearningTraces/learning.txt", "a")
                learning.write(results[i].index(2) - results[i].index(1) + " " + results[i].index(1) + " " + results[i].index(0) + " " + i)

        # Use above learning on last segment of trace file

        # Open file that contained learned trace files to use
        learned = open("LearningTraces/learning.txt", "r")

        # Count number of taken and not taken branches in last segment
        taken = 0
        notTaken = 0
        for i in range(len(list) - segment3):
            i = i + segment3
            if list[i].index(7) == "t":
                taken = taken + 1
            else:
                notTaken = notTaken + 1

        # Evaluate what branch predictor to use on the last portion
        # Evaluate what past performance closely resembles this last section
        similarity = []
        algo = []
        while True:
            line = inputFile.readline()

            if line == "":
                break
            similarity.append(abs((taken / notTaken)-(int(line.split()[0]) / int(line.split()[1]))))
            algo.append(line.split()[3])

        # Use optimal branch predictor
        results

        for i in len(similarity):
            if similarity[i] == min(similarity):
                if algo[i] == 1:
                    results = gshare(m, n, list[segment3:])
                elif algo[i] == 2:
                    results = bimodal(m, list[segment3:])
                elif algo[i] == 3:
                    results = smith(m, list[segment3:])

        print("Missed Predictions: " + results[1] + "\n")
        print("Total Predictions: " + results[2] + "\n")
        print("Accuracy: " + results[0]*100 + "%\n")

    # Run best branch predictor on whole trace file
    else:
        # Count number of taken and not taken branches
        inputFile = open(tracefile, "r")
        taken = 0
        notTaken = 0
        for i in range(len(list)):
            if list[i].index(7) == "t":
                taken = taken + 1
            else:
                notTaken = notTaken + 1

        # Read all learned trace file.
        similarity = []
        algo = []

        # Evaluate what branch predictor to use on the last portion
        # Evaluate what past performance closely resembles this last section
        while True:
            line = inputFile.readline()

            if line == "":
                break
            similarity.append(abs((taken / notTaken)-(int(line.split()[0]) / int(line.split()[1]))))
            algo.append(line.split()[3])

        # Use optimal branch predictor
        for i in len(similarity):
            if similarity[i] == min(similarity):
                if algo[i] == 1:
                    results = gshare(m, n, list)
                elif algo[i] == 2:
                    results = bimodal(m, list)
                elif algo[i] == 3:
                    results = smith(m, list)

        print("Missed Predictions: " + results[1] + "\n")
        print("Total Predictions: " + results[2] + "\n")
        print("Accuracy: " + results[0]*100 + "%\n")