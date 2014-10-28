package afd;

import java.io.Console;
import java.io.FileWriter;
import java.io.IOException;
import java.io.PrintWriter;

public class Main {
	public static void escribeFichero(String fichero,String cadena,boolean append){
        FileWriter f = null;
        PrintWriter pw = null;
        try
        {
            f = new FileWriter(fichero,append);
            pw = new PrintWriter(f);
            pw.println(cadena);
 
        } catch (Exception e) {
            e.printStackTrace();
        } finally {
           try {
           if (null != fichero)
              f.close();
           } catch (Exception e2) {
              e2.printStackTrace();
           }
        }
	}
	public static void main(String[] args) {
		Automata a = new Automata(args[0]);
		System.out.println(a);
		escribeFichero("conf-dot.txt","digraph{\ncompound=true;\n ",false);
		escribeFichero("conf-dot.txt",a.generaDot("AFD"),true);
		a.conexo();
		System.out.println(a);
		escribeFichero("conf-dot.txt",a.generaDot("AFD-conexo"),true);
		escribeFichero("conf-dot.txt","\n }",true);
		try {
			Runtime.getRuntime().exec("dot conf-dot.txt -Tpng -o AFD-conexo.png");
			Runtime.getRuntime().exec("eog AFD-conexo.png &");
		} catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
	}

}
