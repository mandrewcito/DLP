package afd;

public class Transicion {
	private String ini;
	private String fin;
	private String simb;
	public Transicion(String inicio,String fin,String simbolo){
		this.ini=inicio;
		this.fin=fin;
		this.simb=simbolo;
	}
	
	public String getInicio(){
		return this.ini;
	}
	public String getFin(){
		return this.fin;
	}
	public String getSimb(){
		return this.simb;
	}
	
	public String toString(){
		return this.ini+" -> "+this.simb+" -> "+this.fin;
	}
}
