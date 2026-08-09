# Intro
This is my first AI, I am making it without AI libraries. It is an AI that tries to find changes in Apple stock (Apple was chosen randomly) and based on data predicts the outcome of the current day. It is not good, I mostly just made it to understand AI. I finished it but there are a lot of WIP files in here. The finished project is "final?.cpp".
# Details
My dataset is Apple stock daily from 9/7/21 to 8/7/26. It is attached in the file "aapl_prices.csv". I got it for free from Stooq.
All numbers are normalized with their Z score, which gives you the amount of standard deviations away from the mean a certain data point is.
I used an in sample and out of sample performance difference:
==== TRAINED WEIGHTS - BETA ====
Intercept            : 0.000649
Return Today         : 0.000782
Return Yesterday     : 0.000531
5-Day Trend          : -0.001203
Vol Ratio            : -0.000767
5-Day EMA Ratio      : -0.000646

==== IN-SAMPLE PERFORMANCE ====
Mean Squared Error   : 0.000330
Directional Accuracy : 52.851324%

==== OUT OF SAMPLE PERFORMANCE ====
Mean Squared Error   : 0.000309
Directional Accuracy : 56.50465%
Next Prediction      : 0.005117
Actual Next Change   : 0.004534
What this data shows you (I believe) is that this model is not over- or under-fitted. This is because of the fact that the MSE is relatively low, and it stays almost identical when tested on non training data. In addition, ~56% directional accuracy means that it was accurate a lot of the time when predicting just to "buy" or "sell", in the sense that this isn't actually simulating buying or selling, just predicting. 

# Acknowledgements
Thank you to my dad for being a coder.
Thank you to my mom for being kind and doing a lot of work.
Thank you to Marvin Gaye for making such good music.
