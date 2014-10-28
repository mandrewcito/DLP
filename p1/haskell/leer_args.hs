module Main ( main ) where
import System.Environment
import System.IO
import System.Directory
import Control.Monad
import Data.Char
import Data.List
import Data.List.Split
import Tipos
-- hacer automata
-- divide por ; 
splitPto f = do
	conf<-(splitOn ";" f)
	return conf 
-- divide por espacio
splitEsp config=do
	conf<-(splitOn " " config)
	return conf
-- crea cada transicion 
crea_transicion conf =do
		case conf of
			([])-> Transicion{inicio="",fin="",simbolo=""}
			(x:y:z:l)->Transicion{inicio=x,fin=y,simbolo=z}
--obtiene las transiciones de la matriz de configuracion
get_transiciones trans transiciones = do
	 case trans of 
		([])-> transiciones
		(x:xs)->get_transiciones xs (transiciones++[crea_transicion (tail(splitEsp x))])
--dada la matriz de configuracion, obtiene la configuracion del automata
getConfig matriz = do 
		case matriz of
			(x:x1:x2:x3:xs)-> (splitEsp x, splitEsp (tail x1),head(tail(x2)),splitEsp (tail x3),get_transiciones   xs [])
-- configuracion a partir de un fichero
initAutomata config= do 
	let (est,alf,ini,fin,trans) = getConfig (splitPto config)
	--print (est,alf,ini,fin,trans)
	return Automata{estados=est,alfabeto=alf,inicial=[ini],finales=fin,transiciones=trans}

-- fin hacer automata

-- hallar transiciones conexas	

--obtiene transiciones conexas dado un SI y unas transciones
trans_conexas cola visitados transiciones conexas = do
	case cola of
		[]->(conexas,visitados)
		x:xs->trans_conexas (getNConexas x transiciones (visitados++[x]) xs) (visitados++[x]) transiciones (conexas++(getTConexas x transiciones []))
--obtiene nodos conexos
getNConexas e transiciones visitados cola = do
	case transiciones of
		([])->cola
		(x:xs)->if (getInicio x == e) && not (elem (getFin x) visitados)&& not (elem (getFin x) cola)
			then getNConexas e xs visitados (cola++[getFin x])
			else getNConexas e xs visitados cola
--obtiene transiciones conexas y estados 
getTConexas e transiciones tnew = do 
	case transiciones of
		([])->tnew
		(x:xs)->if (getInicio x) == e then getTConexas e xs tnew++[x]
			else getTConexas e xs tnew
-- fin hallar transiciones conexas
-- main
main :: IO ()
main = do
	args <- getArgs
	case args of
		[] -> putStrLn "introduce un fichero"
		[a] ->do
			exists <- doesFileExist ("../test/"++a)
			if exists then do 
				inh<-openFile ("../test/"++a) ReadMode
				confAutomata<-hGetLine inh
				hClose inh
				let cnf = confAutomata
				automata<-(initAutomata cnf)			
				print (getEstados automata)
				print (getAlfabeto automata)
				print (getInicial automata)
				print (getFinales automata)
				print (getTransiciones automata)
                                let (transC,visitados)= trans_conexas (getInicial automata) [] (getTransiciones automata) []
				print transC
				print visitados
			else putStrLn "el fichero no existe"
		_ -> putStrLn "use: stripper <file>"




