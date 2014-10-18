--los tipos estan bien!! 
module Tipos where
	data Transicion = Transicion {inicio :: String , fin :: String, simbolo :: String}deriving (Eq, Show, Read)
	getInicio(Transicion inicio _ _ )=inicio
	getFin(Transicion _ fin _ )=fin
	getSimbolo(Transicion _ _ simbolo )=simbolo

	data Automata = Automata {estados :: [String], alfabeto :: [String], inicial :: String, finales :: [String], transiciones ::[Transicion]} deriving (Eq, Show, Read)
	getEstados(Automata estados _ _ _ _)=estados
	getAlfabeto(Automata _ alfabeto _ _ _)=alfabeto
	getInicial(Automata _ _ inicial _ _)=inicial
	getFinales(Automata _ _ _ finales _)=finales
	getTransiciones(Automata _ _ _ _ transiciones)=transiciones
