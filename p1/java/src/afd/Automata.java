package afd;

import java.io.BufferedReader;
import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileReader;
import java.io.IOException;
import java.util.ArrayList;
import java.util.Iterator;
import java.util.List;

public class Automata {
	private String conf;
	private String estadoInicial;
	private List<String> estados= new ArrayList<String>();
	private List<String> estadosFinales= new ArrayList<String>();
	private List<String> alfabeto= new ArrayList<String>();
	private List<Transicion> transiciones=new ArrayList<Transicion>();
	
	public Automata(String conf){
		this.conf=this.readFile(conf);
		String[] parts = this.conf.split(";");
		//estados
		String[] tmp=parts[0].split(" ");
		int i;
	    for (i=0;i<tmp.length;i++){
	    	this.estados.add(tmp[i]);
	    }
	  //alfabeto
	    tmp=parts[1].split(" ");
	    for (i=1;i<tmp.length;i++){
	      this.alfabeto.add(tmp[i]);
	   }
	  //estado inicial
	    tmp=parts[2].split(" ");
	    this.estadoInicial=tmp[1];
	  //estados finales
	    tmp=parts[3].split(" ");
	    for (i=1;i<tmp.length;i++){
	      this.estadosFinales.add(tmp[i]);
	   }
	  //transiciones
	    for (i=4;i<parts.length;i++){
	    	tmp=parts[i].split(" ");
	    	this.transiciones.add(new Transicion(tmp[1],tmp[2],tmp[3]));
	    }
	}
	
	public String toString(){
		String automata="";
		Iterator i;
		automata+="estados:";
		i=this.estados.iterator();
		while(i.hasNext())
			automata+=i.next()+" ";
		automata+="\nalfabeto: ";
		i=this.alfabeto.iterator();
		while(i.hasNext())
			automata+=i.next()+" ";
		automata+="\nestado inicial: "+this.estadoInicial+" \nestados finales:";
		i=this.estadosFinales.iterator();
		while(i.hasNext())
			automata+=i.next()+" ";
		automata+="\ntransiciones :\n";
		i=this.transiciones.iterator();
		while(i.hasNext())
			automata+=i.next()+" \n";
		automata+="\n";
		return automata;
		
	}
	
	private String readFile(String src){
		File file = new File("../test/"+src);
		BufferedReader reader = null;

		try {
		    reader = new BufferedReader(new FileReader(file));
		    String text = null;

		    text = reader.readLine();
		    return text;
		    
		} catch (FileNotFoundException e) {
		    e.printStackTrace();
		} catch (IOException e) {
		    e.printStackTrace();
		} finally {
		    try {
		        if (reader != null) {
		            reader.close();
		        }
		    } catch (IOException e) {
		    }
		}
		return null;
	}
	public String generaDot(String prefix){
		String conf="";
	    String inicio="subgraph \""+prefix+"\" { \n";
	    String  fin="label = \""+prefix+"\";}";
	    Iterator <Transicion>i = this.transiciones.iterator();
	    String cuerpo="";
	    while (i.hasNext()){
	    	Transicion t = i.next();
	    	cuerpo+="\""+prefix+"-"+t.getInicio()+"\"->\""+prefix+"-"+t.getFin()+"\"[label=\""+t.getSimb()+"\"]"+";\n";
	    }
	    String finales="";
	    Iterator <String>it= this.estadosFinales.iterator();
	    while(it.hasNext()){
	    	String e= it.next();
	    	finales+="\""+prefix+"-"+e+"\"[peripheries=2];\n";
	    }
	    String inicial="node [style=invis]; \""+prefix+"-00\";\n \""+prefix+"-00\"->\""+prefix+"-"+this.estadoInicial+"\";";
		it = this.estados.iterator();
		String estadoslbl="";
	    while(it.hasNext()){
	    	String e= it.next();
	    	estadoslbl+="\""+prefix+"-"+e+"\"[label = \""+e+"\"];";
	    }
	    conf=inicio+cuerpo+finales+estadoslbl+inicial+fin;
	    return conf;
	}
	public void conexo(){
		List <String> listaEstados=new ArrayList<String>();
		listaEstados.add(this.estadoInicial);
		List <Transicion> nuevasTransiciones = new ArrayList<Transicion>();
		List <String> visitados = new ArrayList<String>();
		while (listaEstados.size()>0)
		{
			String e = listaEstados.get(0);
			listaEstados.remove(e);
			visitados.add(e);
			Iterator<Transicion> i= this.transiciones.iterator();
			Transicion t;
			while (i.hasNext()){
				t = i.next();
				if (t.getInicio().equalsIgnoreCase(e))
				{
					nuevasTransiciones.add(t);
					if (!listaEstados.contains(t.getFin()) && !visitados.contains(t.getFin()))
						listaEstados.add(t.getFin());
				}
			}
		}
		this.estados=visitados;
		this.transiciones=nuevasTransiciones;	
	}
}
