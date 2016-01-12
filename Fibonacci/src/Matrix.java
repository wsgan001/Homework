import java.math.BigInteger;


public class Matrix {
/*
 * 两行两列矩阵
 * a,b
 * c,d
 * */
	public BigInteger a;
	public BigInteger b;
	public BigInteger c;
	public BigInteger d;
	public Matrix(BigInteger a, BigInteger b, BigInteger c, BigInteger d) {
		super();
		this.a = a;
		this.b = b;
		this.c = c;
		this.d = d;
	}
	public Matrix(){
		
	}
	public static Matrix mul(Matrix m, Matrix n){
		Matrix result = new Matrix();
		result.a = m.a.multiply(n.a).add(m.b.multiply(n.c));
		result.b = m.a.multiply(n.b).add(m.b.multiply(n.d));
		result.c = m.c.multiply(n.a).add(m.d.multiply(n.c));
		result.d = m.c.multiply(n.b).add(m.d.multiply(n.d));
		return result;
	}
	@Override
	public String toString() {
		return "M:\t [a=" + a + ", b=" + b + "]\n\t["+"c=" + c + ", d=" + d + "]";
	}
	
}
