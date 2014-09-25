#/usr/bin/env python
# -*- cofing: utf-8 -*-
from automata import Automata
fichero=raw_input("introduce nombre fichero configuracion\n")
a=Automata(fichero)
print "forma tabular del automata\n"
a.tabular()
a.minimoConexo()
print "forma tabular del minimo conexo\n"
a.tabular()


