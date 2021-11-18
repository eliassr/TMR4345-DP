from numpy import *
from matplotlib.pyplot import *
from pyplot_config import params
rcParams.update(params)

cases = ["case1","case2","case3","case4","case5"]
for case in cases:
  arr = loadtxt(case + ".txt")
  pos,dev,input,t = [arr[:,i] for i in range(size(arr,1))]

  fig, (p1,p2,p3) = subplots(3,sharex=True)
  p1.plot(t,pos,color='royalblue')
  p2.plot(t,dev,color='darkorange')
  p3.plot(t,input,color='chartreuse')

  p1.set(ylabel='Position')
  p2.set(ylabel='Deviation')
  p3.set(ylabel='Motor input')
  xlabel(r'Time [s]')

  savefig(fname=(case +".pdf"))
