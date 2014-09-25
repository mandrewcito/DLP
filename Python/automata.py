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
      self.transiciones.append(e[1:].split(" "))

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
        if (trans[0]==e and trans not in nuevasTransiciones):
          nuevasTransiciones.append(trans)
          eliminar.append(trans)
          if(trans[1] not in visitados):
            colaEstados.append(trans[1])
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

  def tabular(self):
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
          if (trans[0]==estado and trans[2]==simbolo):
            linea=linea+[trans[1]]
            continue
      if linea not in tabla:
        tabla.append(linea)
    print cabecera
    for fila in tabla:
      print fila
