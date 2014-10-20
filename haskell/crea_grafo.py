#/usr/bin/env python
# -*- cofing: utf-8 -*-
import os
f=open("config.txt")
fichero=f.read()
lineas=fichero.split("\n")
estados=lineas[0][1:len(lineas[0])-1]
estados=estados.split(",")
nuevos=[]
for e in estados:
  nuevos.append(e.replace("\"",""))
estados=nuevos
print estados
alfabeto=lineas[1][1:len(lineas[1])-1]
alfabeto=alfabeto.split(",")
nuevos=[]
for e in alfabeto:
  nuevos.append(e.replace("\"",""))
alfabeto=nuevos
print alfabeto
estadoInicial=lineas[2][1:len(lineas[2])-1].replace("\"","")
print estadoInicial
finales=lineas[3][1:len(lineas[3])-1]
finales=finales.split(",")
nuevos=[]
for e in finales:
  nuevos.append(e.replace("\"",""))
estadosFinales=nuevos
print finales
nuevos=[]
transiciones=lineas[4][1:len(lineas[4])-1]
for n,e in enumerate(transiciones.split("Transicion {inicio = \"")):
  if not e=="" and not e[0]=="\"":
    nuevos.append([str(e[0])])
i=0
for n,e in enumerate(transiciones.split("fin = \"")[1:]):
  if not e=="" and not e[0]=="\"" and n<=len(nuevos)-1:
    nuevos[i].append(e[0])
    i+=1
i=0
for n,e in enumerate(transiciones.split("simbolo = \"")[1:]):
  if not e=="" and not e[0]=="\""and n<=len(nuevos)-1:
    nuevos[i].append(e[0])
    i+=1
transicionesNoConexas=nuevos
print transicionesNoConexas
nuevos=[]
i=0
transiciones=lineas[5][1:len(lineas[5])-1]
for n,e in enumerate(transiciones.split("Transicion {inicio = \"")):
  if not e=="" and not e[0]=="\"":
    nuevos.append([str(e[0])])
i=0
for n,e in enumerate(transiciones.split("fin = \"")[1:]):
  if not e=="" and not e[0]=="\"" and n<=len(nuevos)-1:
    nuevos[i].append(e[0])
    i+=1
i=0
for n,e in enumerate(transiciones.split("simbolo = \"")[1:]):
  if not e=="" and not e[0]=="\""and n<=len(nuevos)-1:
    nuevos[i].append(e[0])
    i+=1
transicionesConexas=nuevos
print transicionesConexas
estadosC=lineas[6][1:len(lineas[6])-1]
estadosC=estadosC.split(",")
nuevos=[]
for e in estadosC:
  nuevos.append(e.replace("\"",""))
estadosConexos=nuevos
print estadosConexos
transiciones=transicionesNoConexas
png="conexo"
print "creando grafo"
inicio="subgraph \""+png+"\" { \n"
fin="}"
cuerpo=""
for trans in transiciones : 
  cuerpo+="\""+png+"-"+trans[0]+"\"->\""+png+"-"+trans[1]+"\"[label=\""+trans[2]+"\"]"+";\n"
finales=""
for e in estadosFinales:
  finales+="\""+png+"-"+e+"\"[peripheries=2];\n"
inicial="node [style=invis]; \""+png+"-00\";\n \""+png+"-00\"->\""+png+"-"+estadoInicial+"\";"
f = open (png+"-conf.txt", "w")
f.write(inicio)
f.write(cuerpo)
f.write(finales)
for e in estados:
  f.write("\""+png+"-"+e+"\"[label = \""+e+"\"];")
f.write(inicial)
f.write("label = \""+png+"\";")
f.write(fin)
f.close()
#creamos no conexo 
transiciones=transicionesConexas
png="Noconexo"
print "creando grafo"
inicio="subgraph \""+png+"\" { \n"
fin="}"
cuerpo=""
for trans in transiciones : 
  cuerpo+="\""+png+"-"+trans[0]+"\"->\""+png+"-"+trans[1]+"\"[label=\""+trans[2]+"\"]"+";\n"
finales=""
for e in estadosFinales:
  finales+="\""+png+"-"+e+"\"[peripheries=2];\n"
inicial="node [style=invis]; \""+png+"-00\";\n \""+png+"-00\"->\""+png+"-"+estadoInicial+"\";"
f = open (png+"-conf.txt", "w")
f.write(inicio)
f.write(cuerpo)
f.write(finales)
for e in estados:
  if e in estadosConexos:
    f.write("\""+png+"-"+e+"\"[label = \""+e+"\"];")
f.write(inicial)
f.write("label = \""+png+"\";")
f.write(fin)
f.close()

f=open("par-AFD.txt","w")
f.write("digraph{compound=true; \n")
f.close()
os.system("cat conexo-conf.txt >> par-AFD.txt")
f=open("par-AFD.txt","a")
f.write("\n")
f.close()
os.system("cat Noconexo-conf.txt >> par-AFD.txt")
f=open("par-AFD.txt","a")
f.write("\n}")
f.close()
os.system("dot par-AFD.txt -Tpng -o AFD-conexo.png")
os.system("eog AFD-conexo.png && rm par-AFD.txt && rm Noconexo-conf.txt && rm conexo-conf.txt")
