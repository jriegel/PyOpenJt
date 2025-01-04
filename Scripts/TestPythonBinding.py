import sys, os
print (os.getcwd())
sys.path.append("~/lib")
sys.path.append(os.getcwd() + "/../WinBuild/Debug")
import PyOpenJt

f = PyOpenJt.JtFile(os.getcwd()+"/../Data/ExampleFiles/example_block_jt9.5.jt")

f.FileName = os.getcwd()+"/../Data/ExampleFiles/example_block_jt9.5.jt"
f.open()

j = f.json()
print (j)
