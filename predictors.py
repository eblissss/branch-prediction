import pandas as pd

def bimodal(m, binary, addresses):
    table = []
    for i in range(1 << m):
        table.append(4)

    actual_taken = True
    pred_taken = True

    # Stats
    predictions = 0
    mispredictions = 0
    pc = 0
    print(binary[0:20], addresses[0:20])
    mispredicts = []
    # Find and predict at each branch
    for i, line in enumerate(binary):
        actual_taken = (line == 1)
        
        # Get PC
        pc = int(addresses[i], 16)

        index = (pc >> 2) % (1 << m)

        # Predict and update
        value = table[index]
        pred_taken = (value >= 4)

        if actual_taken and value < 7:
            table[index] += 1

        elif not actual_taken and value > 0:
            table[index] -= 1
        
        mispredicts.append(1 if actual_taken != pred_taken else 0)
        predictions += 1
        if actual_taken != pred_taken:
            mispredictions += 1
            
    return mispredicts
            
# Gshare Branch Predictor
def gshare(m, n, binary, addresses):
    bhr_largest_bit = 1 << (n - 1)

    table = []
    for i in range(1 << m):
        table.append(4)

    actual_taken = True
    pred_taken = True

    # Stats
    predictions = 0
    mispredictions = 0
    mispredicts = []

    bhr = 0
    pc = 0

    # Find and predict at each branch
    for i, line in enumerate(binary):
        actual_taken = line == 1

        # Get PC
        pc = int(addresses[i], 16)
        # Remove right 2 bits
        pc = pc >> 2

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

        predictions += 1
        mispredicts.append(1 if actual_taken != pred_taken else 0)
        if actual_taken != pred_taken:
            mispredictions += 1

    return mispredicts

# Smith Branch Predictor
def smith(num_bits, binary):
    largest_bit = 1 << (num_bits - 1)
    counter = largest_bit

    actual_taken = True
    pred_taken = True

    # Stats
    predictions = 0
    mispredictions = 0
    mispredicts = []

    # Find and predict at each branch
    for line in binary:
        actual_taken = line == 1

        pred_taken = (counter >= largest_bit)

        if (actual_taken and counter < 7):
            counter += 1
        elif (not actual_taken and counter > 0):
            counter -= 1

        predictions += 1
        mispredicts.append(1 if actual_taken != pred_taken else 0)
        if actual_taken != pred_taken:
            mispredictions += 1

    return mispredicts


# Smith Branch Predictor
# def smith_step(num_bits, taken, counter):
#     largest_bit = 1 << (num_bits - 1)

#     # Find and predict at each branch
#     actual_taken = taken == 1

#     pred_taken = (counter >= largest_bit)

#     if (actual_taken and counter.count < 7):
#         counter.inc()
#     elif (not actual_taken and counter.count > 0):
#         counter.dec()

#     return pred_taken

def smith_pred(num_bits, counter):
    largest_bit = 1 << (num_bits - 1)

    return (counter >= largest_bit)

def smith_update(taken, counter):
    actual_taken = taken == 1

    if (actual_taken and counter.count < 7):
        counter.inc()
    elif (not actual_taken and counter.count > 0):
        counter.dec()

# Bimodal Step Predictor
# def bimodal_step(m, taken, address, table):

#     actual_taken = (taken == 1)
    
#     # Get PC
#     pc = int(address, 16)

#     index = (pc >> 2) % (1 << m)

#     # Predict and update
#     value = table[index]
#     pred_taken = (value >= 4)

#     if actual_taken and value < 7:
#         table[index] += 1

#     elif not actual_taken and value > 0:
#         table[index] -= 1
            
#     return pred_taken

def bimodal_pred(m, address, table):
    
    pc = int(address, 16)
    index = (pc >> 2) % (1 << m)

    # Predict
    value = table[index]
    pred_taken = (value >= 4)
            
    return pred_taken

def bimodal_update(m, taken, address, table):

    actual_taken = (taken == 1)
    
    # Get PC
    pc = int(address, 16)
    index = (pc >> 2) % (1 << m)

    # Update
    value = table[index]

    if actual_taken and value < 7:
        table[index] += 1

    elif not actual_taken and value > 0:
        table[index] -= 1


# Gshare Branch Predictor
# def gshare_step(m, n, taken, address, table, bhr):
#     bhr_largest_bit = 1 << (n - 1)

#     actual_taken = taken == 1

#     # Get PC
#     pc = int(address, 16) >> 2

#     index_A = (((pc >> n) << n) % (1 << m))
#     index_B = (pc % (1 << n)) ^ bhr

#     index = index_A + index_B

#     # Predict and update
#     value = table[index]
#     pred_taken = (value >= 4)

#     if actual_taken and value < 7:
#         table[index] += 1

#     elif not actual_taken and value > 0:
#         table[index] -= 1

#     bhr >>= 1
#     if actual_taken:
#         bhr += bhr_largest_bit

#     return pred_taken


def gshare_pred(m, n, address, table, bhr):
    # Get PC
    pc = int(address, 16) >> 2

    index_A = (((pc >> n) << n) % (1 << m))
    index_B = (pc % (1 << n)) ^ bhr

    index = index_A + index_B

    # Predict
    value = table[index]
    return (value >= 4)

def gshare_update(m, n, taken, address, table, bhr):
    bhr_largest_bit = 1 << (n - 1)

    actual_taken = taken == 1

    # Get PC
    pc = int(address, 16) >> 2

    index_A = (((pc >> n) << n) % (1 << m))
    index_B = (pc % (1 << n)) ^ bhr

    index = index_A + index_B

    # Predict and update
    value = table[index]

    if actual_taken and value < 7:
        table[index] += 1

    elif not actual_taken and value > 0:
        table[index] -= 1

    bhr >>= 1
    if actual_taken:
        bhr += bhr_largest_bit