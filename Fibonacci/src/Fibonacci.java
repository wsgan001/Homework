import java.math.BigInteger;

/*
 * 计算斐波那契数
 * */
public class Fibonacci {
	public BigInteger naiveRecursive(int n){
		if(n == 0)	return new BigInteger("0");
		if(n == 1)	return new BigInteger("1");
		return naiveRecursive(n-1).add(naiveRecursive(n-2));
	}
	public BigInteger bottomUp(int n){
		BigInteger f1=new BigInteger("0"),f2=new BigInteger("1"), result = new BigInteger("0");
		if(n == 0)	return new BigInteger("0");
		if(n == 1)	return new BigInteger("1");
		for(int i =1; i < n; i++){
			result = f1.add(f2);
			f1 = f2;
			f2 = result;
		}
		return result;
	}
	public BigInteger recursiveSquare(int n){
		if(n == 0)	return new BigInteger("0");
		if(n == 1)	return new BigInteger("1");
		Matrix m = calMatrix(n-1);
		return m.a;
	}
	private Matrix calMatrix(int power){
		if(power == 1)	return new Matrix(new BigInteger("1"),new BigInteger("1")
		,new BigInteger("1"),new BigInteger("0")); 
		
		if(power % 2 == 0){
			return Matrix.mul(calMatrix(power/2), calMatrix(power/2));
		}else{
			Matrix tmp = Matrix.mul(calMatrix(power/2), calMatrix(power/2));
			return Matrix.mul(tmp, new Matrix(new BigInteger("1"),new BigInteger("1"),new BigInteger("1"),new BigInteger("0")));
		}
	}
	public static void main(String[] args){
		int[] ts = new int[]{
			1,
			10,
			100,
			1000,
			10000,
			100000,
			1000000
		};
		Fibonacci f = new Fibonacci();
		long startTime;
		long endTime;
		BigInteger tmp;
		
		System.out.println("recursiveSquare:");
		for(int i=0; i < ts.length; i++){
			System.out.println("问题规模:"+ts[i]);
			startTime = System.currentTimeMillis();
			tmp = f.recursiveSquare(ts[i]);
			endTime = System.currentTimeMillis();
			System.out.println("用时："+(endTime - startTime)+"毫秒");
			//System.out.println("计算结果:"+tmp);
		}
		System.out.println("ButtomUp:");
		for(int i=0; i < ts.length; i++){
			System.out.println("问题规模:"+ts[i]);
			startTime = System.currentTimeMillis();
			tmp = f.bottomUp(ts[i]);
			endTime = System.currentTimeMillis();
			System.out.println("用时："+(endTime - startTime)+"毫秒");
			//System.out.println("计算结果:"+tmp);
		}
		
		System.out.println("naiveRecursive:");
		for(int i=0; i < ts.length; i++){
			System.out.println("问题规模:"+ts[i]);
			startTime = System.currentTimeMillis();
			tmp = f.naiveRecursive(ts[i]);
			endTime = System.currentTimeMillis();
			System.out.println("用时："+(endTime - startTime)+"毫秒");
			//System.out.println("计算结果:"+tmp);
		}
		
	}
}
