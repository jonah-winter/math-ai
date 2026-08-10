# Intro
This is my first AI, I am making it without AI libraries because pytorch seems pretty boring. It is an AI that tries to find changes in Apple stock (Apple was chosen randomly) and based on data predicts the outcome of the current day. It is not good, I mostly just made it to understand AI. I finished it but there are a lot of WIP files in here. The finished project file is "final?.cpp", and the final dataset is "aapl_prices.csv".
# Details
My dataset is Apple stock daily from 9/7/21 to 8/7/26. It is attached in the file "aapl_prices.csv". I got it for free from Stooq.
All numbers are normalized with their Z score, which gives you the amount of standard deviations away from the mean a certain data point is.
EMA stands for Exponential Moving Average, which weighs the most recent days more heavily on a 5 day trend to catch sudden rises or falls better.
I used an in sample and out of sample performance difference:

``` text
==== TRAINED WEIGHTS - BETA ====

Intercept            : 0.000649

Return Today         : 0.000782

Return Yesterday     : 0.000531

5 Day Trend          : -0.001203

Vol Ratio            : -0.000767

5 Day EMA Ratio      : -0.000646

==== IN-SAMPLE PERFORMANCE ====

Mean Squared Error   : 0.000330

Directional Accuracy : 52.851324%

==== OUT OF SAMPLE PERFORMANCE ====

Mean Squared Error   : 0.000309

Directional Accuracy : 56.50465%

Next Prediction      : 0.005117

Actual Next Change   : 0.004534
```
## Closing Details
What this data shows you (I believe) is that this model is not over- or under-fitted. This is because of the fact that the MSE is relatively low, and it stays almost identical when tested on non training data. In addition, ~56% directional accuracy means that it was accurate a lot of the time when predicting just to "buy" or "sell", in the sense that this isn't actually simulating buying or selling, just predicting. Overall, this was very exciting to make and stare at after I finished (and brag about).
The actual final equation looks like ($\beta_{0,0}$)

# Conclusion
This was pretty fun. I learned way more than from any of my C++ books making this (although I only have one C++ book). I had to look up much more than I wanted to, but I suppose that's part of being a developer. The part with all the matrix stuff was easy, it was mostly the actual AI making where i had to look up how to make it learn, what beta was, how to actually get accurate predictions, and what specific data to use to get these predictions. It was really hard overall because I don't think I really grasped how simple (asterisk) AI is. What I mean by that is all it ended up being was slightly convoluted math. This project definitely humbled me, I thought I knew much more AI concepts than I did. I also thought finance concepts at this level were much simpler than they really are. I would give this project a 100000/10 for effort and learning.

# Acknowledgements
Thank you to my dad for being a coder.
Thank you to my mom for being kind and doing a lot of work.
Shoutout to John Darnielle and the rest of the Mountain Goats for making great music.
