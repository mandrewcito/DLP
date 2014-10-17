module Main ( main ) where
import System.Environment
import System.IO
import System.Directory
import Control.Monad
import Data.Char
import Data.List
import Data.List.Split

splitPto f = do
	conf<-(splitOn ";" f)
	return conf 

splitEsp config=do
	conf<-(splitOn " " config)
	return conf

initAutomata config= do 
	let matriz = splitPto config
	let a = head(matriz)
	let estados= (splitEsp a)
	return estados

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
				let a = confAutomata
				estados <- initAutomata a
				putStrLn head(estados) 
			else putStrLn "el fichero no existe"
		_ -> putStrLn "use: stripper <file>"




