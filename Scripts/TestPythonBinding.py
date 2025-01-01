import sys, os
print (os.getcwd())
sys.path.append("~/lib")
sys.path.append(os.getcwd() + "/../WinBuild/Debug")
import bca
bca.init()
w = bca.World(0)
w.count

bca.getStatistic("NodeCount")

w.getBit(0,0,0);nw = w.setBit(0,0,0);w.getBit(0,0,0);w.count

bca.getStatistic("WorldCount")
def main():
    bca.setValue("test","hallo")
    print (bcapy.getValue("test"))
    
bca.runSelfTest()



if __name__ == "__main__":
    main()
