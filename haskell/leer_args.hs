module Main ( main ) where
import System.Environment
import System.IO
import System.Directory
import Control.Monad
import Data.Char
import Data.List
import Data.List.Split
import Tipos

splitPto f = do
	conf<-(splitOn ";" f)
	return conf 

splitEsp config=do
	conf<-(splitOn " " config)
	return conf

get_transiciones trans transiciones = do
	 case trans of 
		([])-> transiciones
		(x:xs)->do 
			let c = splitEsp (tail x)
			let t = [Transicion{inicio= head(c),fin=head(tail c),simbolo=head(tail(tail(c)))}]
			get_transiciones xs (transiciones++t)

getConfig matriz = do 
		case matriz of
			(x:x1:x2:x3:xs)-> (splitEsp x, splitEsp (tail x1),head(tail(x2)),splitEsp (tail x3),get_transiciones xs [])

initAutomata config= do 
	let (est,alf,ini,fin,trans) = getConfig (splitPto config)
	print (trans)
	--Automata{estados=est,alfabeto=alf,inicial=ini,finales=fin,transiciones=trans}

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
				--let estados = getEstados(automata)
				putStrLn "fin !" 
			else putStrLn "el fichero no existe"
		_ -> putStrLn "use: stripper <file>"




