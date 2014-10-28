import System.IO
import Data.List.Split


parseInput inpStr=
	inh<-openFile ("../test/"++inpStr) ReadMode
	confAutomata<- hGetLine inh
	hClose inh
	return confAutomata

main:: IO()
main = do
	putStrLn " introduce fichero a leer " 
	inpStr <- getLine
	conf<- $ (parseInput inpStr)
	putStrLn config
