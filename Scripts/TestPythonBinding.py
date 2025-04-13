import sys, os
print (os.getcwd())
sys.path.append("~/lib")
#sys.path.append(os.getcwd() + "/../WinBuild/Debug")
sys.path.append(os.getcwd() + "/../WinBuild/RelWithDebInfo")
import PyOpenJt

#jtFile = "opening_protection_plate1_jt9.5.jt"           # test part with instances
#jtFile = "fmsr_0b42017833aa11eb8d60005056bcb115----jt2jt.jt" # small connector FMS Version
#jtFile = "fmsr_0835c8aca23f11e5ab4a2c44fd845ad4----jt2jt.jt" # Big one with layer filter FMS Version
jtFile = "fmsr_fc3c7da8dc8211e5ab4a2c44fd845ad4----jt2jt.jt" # Big one with layer filter FMS Version

jtObj = PyOpenJt.JtFile(os.getcwd()+"/../Data/ExampleFiles/" + jtFile)

jtObj.open()

#jsonFile = open("c:/temp/test2.json", "w")
#jsonFile.write(jtObj.json())
#jsonFile.close()


jtObj.writeGlTf(os.getcwd()+"/../Data/ExampleFiles/" + jtFile )



