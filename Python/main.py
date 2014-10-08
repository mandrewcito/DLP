#/usr/bin/env python
# -*- cofing: utf-8 -*-
from automata import Automata
import os
fichero=raw_input("introduce nombre fichero configuracion en carpeta /test \n")
a=Automata("../test/"+fichero)
print "forma tabular del automata\n"
a.tabular("AFD")
a.grafo("AFD")
os.system("dot AFD-conf.txt -Tpng -o AFD.png")
os.system("eog AFD.png")
a.minimoConexo()
print "forma tabular del minimo conexo\n"
a.tabular("AFD-conexo")
a.grafo("AFD-conexo")
os.system("dot AFD-conexo-conf.txt -Tpng -o AFD-conexo.png")
os.system("eog AFD-conexo.png")

