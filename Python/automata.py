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
        print trans,e
        if (trans[0]==e):
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
