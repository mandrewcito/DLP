package afd;

public class Main {

	public static void main(String[] args) {
		Automata a = new Automata("conf-isla.txt");
		System.out.println(a);
		a.conexo();
		System.out.println(a);
	}

}
