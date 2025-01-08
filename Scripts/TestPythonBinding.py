import sys, os
print (os.getcwd())
sys.path.append("~/lib")
sys.path.append(os.getcwd() + "/../WinBuild/Debug")
import PyOpenJt

f = PyOpenJt.JtFile(os.getcwd()+"/../Data/ExampleFiles/example_block_jt9.5.jt")

f.FileName = os.getcwd()+"/../Data/ExampleFiles/example_block_jt9.5.jt"
f.open()

#fj = open("c:/temp/test2", "w")
#fj.write(f.json())
#fj.close()


f.writeGlTf("c:/temp/test2")


