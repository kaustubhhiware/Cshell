from random import randint
#
## python in.py | ./a.out
#
n = randint(1,9) # get number of elements in array
print n,"\n"
x = 0
for i in range(0,n):
    toss = randint(0,4)
    if toss < 2:
        print x
    else:
        x = randint(0,1000)
        print x
