from transiciones import Transicion
class Automata():
  def __init__(self,ficheroConfiguracion):
    f=open(ficheroConfiguracion)
    linea=f.read()
    f.close()
    f=linea.split(";")
    f.remove("\n")
    self.estados=f[0].split(" ")
    self.alfabeto=f[1][1:].split(" ")
    self.estadoInicial=f[2][1]
    self.estadosFinales=f[3][1:].split(" ")
    self.transiciones=[]
    for e in f[4:]:
      trans=e[1:].split(" ")
      self.transiciones.append(Transicion(trans[0],trans[1],trans[2]))

  def minimoConexo(self):
    colaEstados=[]
    colaEstados.append(self.estadoInicial)
    nuevasTransiciones=[]
    visitados=[]
    viejasTransiciones=self.transiciones
    while (len(colaEstados)!=0):
      e=colaEstados[0]
      colaEstados.remove(e)
      visitados.append(e)
      eliminar=[]
      for trans in viejasTransiciones:
        if (trans.ini==e):
          nuevasTransiciones.append(trans)
          eliminar.append(trans)
          if(trans.fin not in visitados and trans.fin not in colaEstados):
            colaEstados.append(trans.fin)
      for e in eliminar:
        viejasTransiciones.remove(e)
      eliminar=[]
    self.estados=visitados
    interseccion = []
    for n in self.estadosFinales:
      if n in visitados:
        interseccion.append(n)
    self.estadosFinales=interseccion
    self.transiciones=nuevasTransiciones

  def tabular(self,png):
    cabecera=["    "]+self.alfabeto
    tabla=[]
    for estado in self.estados:
      linea=[estado]
      if estado in self.estadosFinales:
        linea=["*"+linea[0]]
      else:
        linea=[" "+linea[0]]
      if estado == self.estadoInicial:
        linea=["->"+linea[0]]
      else:
        linea=["  "+linea[0]]
      for simbolo in self.alfabeto:
        for trans in self.transiciones:
          if (trans.ini==estado and trans.simb==simbolo):
            linea=linea+[trans.fin]
            continue
      if linea not in tabla:
        tabla.append(linea)
    print cabecera
    for fila in tabla:
      print fila

  def grafo(self,png):
    print "creando grafo"
    inicio="subgraph \""+png+"\" { \n"
    fin="}"
    cuerpo=""
    for trans in self.transiciones : 
      cuerpo+="\""+png+"-"+trans.ini+"\"->\""+png+"-"+trans.fin+"\"[label=\""+trans.simb+"\"]"+";\n"
    finales=""
    for e in self.estadosFinales:
      finales+="\""+png+"-"+e+"\"[peripheries=2];\n"
    inicial="node [style=invis]; \""+png+"-00\";\n \""+png+"-00\"->\""+png+"-"+self.estadoInicial+"\";"
    f = open (png+"-conf.txt", "w")
    f.write(inicio)
    f.write(cuerpo)
    f.write(finales)
    for e in self.estados:
      f.write("\""+png+"-"+e+"\"[label = \""+e+"\"];")
    f.write(inicial)
    f.write("label = \""+png+"\";")
    f.write(fin)
    f.close()
