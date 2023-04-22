# BPTP (Branch Predictor Technique Predictor)

## A Machine Learning-based Branch Prediction Technique for Performance Optimization

## Abstract

Modern computer processors generally use static or dynamic branch prediction to accurately and efficiently predict branch outcomes. Here, we present a new method for branch prediction. This method was designed so that machine learning could choose the optimal method and pattern of prediction for a specific program or thread, for a discrete number of branches, in order to yield the most accurate results. Our results show that we can obtain a similar or greater level of performance than a traditional predictor when employing a feature-training machine learning predictor. In short, our method aims to determine the best standard branch predictor among the G-Share, Bimodal and Smith Predictors for a section of a program execution to maximize prediction accuracy across the entirety of program execution.

Evaluation results <br>
<img src="https://lh6.googleusercontent.com/vq2sv6IzrneMcSXHK4Ov-IwI3n2pYtmkE1u4CsimZMWRQPPaKKKiige5FpiViFjLpHOrOWpULKEsFxXzr-PsRZZjumUzSLSMAfOInGaI8_rDL2YU7uBX5kg1bGEQvQewRze3O7_Dh7XnqfLv-GJqTKPMsQ=s2048" width="400px" />

Feature importance for a sample trace <br>
<img src="https://lh5.googleusercontent.com/wvGC56ZUfCT5zcOlSaZOgBad865ABsTiMx450nn-ls3c5gmy2oc-OyyN13Lk0guMIgY85WdBCneUfZy8DFJiFHSXCgGd2UdbEVK4d29rjT22ybfANUAXh8BiDHWu1A6Nwx6Hew7QyUb6wk1W2AnUzUPfOw=s2048" width="400px" />

### Libraries Used

- Matplotlib
- NumPy
- Scikit-learn
- Pandas
- XGBoost

Data collected with Intel Pin Tool

### How to use

- Install required libraries (`pip install -r requirements.txt`)
- Configure and run featurefuture.py

### C++ Files

A collection of basic simulators

Compile with `make clean` && `make sim`
