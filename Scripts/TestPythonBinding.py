import sys, os
print (os.getcwd())
sys.path.append("~/lib")
sys.path.append(os.getcwd() + "/../WinBuild/Debug")
import PyOpenJt

#jtFile = "opening_protection_plate1_jt9.5.jt"           # test part with instances
jtFile = "fmsr_0b42017833aa11eb8d60005056bcb115----.jt" # small connector
#jtFile = "fmsr_0835c8aca23f11e5ab4a2c44fd845ad4----.jt" # Big one with layer filter

jtObj = PyOpenJt.JtFile(os.getcwd()+"/../Data/ExampleFiles/" + jtFile)

jtObj.open()

#jsonFile = open("c:/temp/test2.json", "w")
#jsonFile.write(jtObj.json())
#jsonFile.close()


jtObj.writeGlTf("c:/temp/test2")


