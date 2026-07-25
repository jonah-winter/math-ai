import torch as t
import time as i
import numpy as n
import pandas as p
import math as m
def mse(pred, act):
  return (pred - act) ** 2
def main():
  x = n.linspace(-m.pi, m.pi, 2000)
  y = n.sin(x)
  w1 = n.random.randn()
  w2 = n.random.randn()
  w3 = n.random.randn()
  w4 = n.random.randn()
  learning_rate = 0.5
  start = i.perf_counter_ns()
  for t in range(30000):
    y_pred = w1 + w2 * x + w3 * x ** 2 + w4 * x ** 3
    if (y_pred - y).mean() > 0.5:
      loss = n.log(n.cosh(y_pred - y)).mean()
      grad_y_pred = n.tanh(y_pred - y) / len(x)
    else:
      loss = mse(y_pred, y)
      grad_y_pred = 2.0 * (y_pred - y) / len(x)
    relative_error = n.sqrt(loss) / n.std(y)
    if t % 1000 == 0 or t == 100:
      print(f"{t}{relative_error.mean() : .5f}")
    grad_w1 = grad_y_pred.mean()
    grad_w2 = (grad_y_pred * x).mean()
    grad_w3 = (grad_y_pred * x ** 2).mean()
    grad_w4 = (grad_y_pred * x ** 3).mean()
    w1 -= learning_rate * grad_w1
    w2 -= learning_rate * grad_w2
    w3 -= learning_rate * grad_w3
    w4 -= learning_rate * grad_w4
  end = i.perf_counter_ns()
  duration = (end - start) / 100000.0
  print(f'{w1 + w2*x + w3*x**2 + w4*x**3}')
  print(f"\n{duration}")
  return 0
if __name__ == "__main__":
  main()
