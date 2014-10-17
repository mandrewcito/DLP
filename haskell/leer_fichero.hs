import System.IO
main = do
	putStrLn " introduce fichero a leer " 
	inpStr <- getLine
	inh<-openFile "../test/"++inpStr ReadMode
	confAutomata<- hGetLine inh
	hClose inh
	putStrLn $confAutomata
