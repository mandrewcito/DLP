#/usr/bin/env python
# -*- cofing: utf-8 -*-
from automata import Automata
import os
fichero=raw_input("introduce nombre fichero configuracion en carpeta /test \n")
a=Automata("../test/"+fichero)
print "forma tabular del automata\n"
a.tabular("AFD")
a.grafo("AFD")
a.minimoConexo()
print "forma tabular del minimo conexo\n"
a.tabular("AFD-conexo")
a.grafo("AFD-conexo")
f=open("par-AFD.txt","w")
f.write("digraph{compound=true; \n")
f.close()
os.system("cat AFD-conf.txt >> par-AFD.txt")
f=open("par-AFD.txt","a")
f.write("\n")
f.close()
os.system("cat AFD-conexo-conf.txt >> par-AFD.txt")
f=open("par-AFD.txt","a")
f.write("\n}")
f.close()
os.system("dot par-AFD.txt -Tpng -o AFD-conexo.png")
os.system("eog AFD-conexo.png && rm par-AFD.txt && rm AFD-conf.txt && rm AFD-conexo-conf.txt")

