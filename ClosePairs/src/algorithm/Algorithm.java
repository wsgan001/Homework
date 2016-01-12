package algorithm;

import java.util.Random;

public class Algorithm {
	public static final int MaxNum = 1000000;
	public static Point[] points = new Point[MaxNum];
	public static Point[] pointsY = new Point[MaxNum];
	// 默认获取points中最近的点的下标
	public static Point[] getClosest() {
		return getClosest(points, 0, points.length - 1);
	}

	public static void generatedRandom() {
		Random random1 = new Random(System.currentTimeMillis());
		Random random2 = new Random(System.currentTimeMillis() / 2);
		
		for (int i = 0; i < MaxNum; i++) {
			points[i] = new Point(Math.round(random1.nextFloat() * MaxNum),
					Math.round(random2.nextFloat() * MaxNum));
		}
	}

	// 获取传入的points中的最近点下标，不更改ps
	public static Point[] getClosest(final Point[] ps, final int s, final int e) {
		if (e - s == 1) {
			return new Point[] { ps[s], ps[e] };
		}
		int m = (s + e) / 2;
		Point[] lminpair = getClosest(ps, s, m);
		double ldist = Point.squaredist(lminpair[0], lminpair[1]);

		Point[] rminpair = getClosest(ps, m, e);
		double rdist = Point.squaredist(rminpair[0],rminpair[1]);

		double mindist = Math.min(ldist, rdist);
		Point[] minpair;
		if (ldist < rdist) {
			mindist = ldist;
			minpair = lminpair;
		} else {
			mindist = rdist;
			minpair = rminpair;
		}
		Point[] Ypis = new Point[e-s+1];
		int yk = 0;
		for(int i=s; i <=e; i++){
			if(Algorithm.pointsY[i].x - Algorithm.points[i].x <= mindist && 
					Algorithm.pointsY[i].x - Algorithm.points[i].x >= -mindist){
				Ypis[yk++] = pointsY[i];
			}
		}
		for(int i=0; i < yk; i++){
			for(int j=i+1; j < 8 && j < yk ; j++){
				if(Ypis[i].squaredist(Ypis[j]) < mindist){
					mindist = Ypis[i].squaredist(Ypis[j]);
					minpair[0] = Ypis[i];
					minpair[1] = Ypis[j];
				}
			}
		}
		
		return minpair;
	}


	public static void sortbyx(Point[] ps, int s, int e) {
		if (s >= e || e >= ps.length || s < 0) {
			return;
		}
		int lastless = s - 1;
		Point pivot = ps[e];
		boolean isInterrupted = true;// 是否连续小，是的话就不交换了
		Point tmp = new Point();
		for (int i = s; i < e; i++) {
			if (ps[i].x <= pivot.x) {
				lastless++;
				if (!isInterrupted) {// swap
					tmp = ps[lastless];
					ps[lastless] = ps[i];
					ps[i] = tmp;
					isInterrupted = false;
				}
				continue;
			} else {
				isInterrupted = false;
			}
		}
		tmp = ps[++lastless];
		ps[lastless] = ps[e];
		ps[e] = tmp;

		sortbyx(ps, s, lastless - 1);
		sortbyx(ps, lastless + 1, e);
	}

	public static void sortbyy(Point[] ps, int s, int e) {
		if (s >= e || e >= ps.length || s < 0) {
			return;
		}
		int lastless = s - 1;
		Point pivot = ps[e];
		boolean isInterrupted = true;// 是否连续小，是的话就不交换了
		Point tmp = new Point();
		for (int i = s; i < e; i++) {
			if (ps[i].y <= pivot.y) {
				lastless++;
				if (!isInterrupted) {// swap
					tmp = ps[lastless];
					ps[lastless] = ps[i];
					ps[i] = tmp;
					isInterrupted = false;
				}
				continue;
			} else {
				isInterrupted = false;
			}
		}
		tmp = ps[++lastless];
		ps[lastless] = ps[e];
		ps[e] = tmp;

		sortbyy(ps, s, lastless - 1);
		sortbyy(ps, lastless + 1, e);
	}

	public static Point[] generatedRandom(Point[] ps, int num, int r) {
		Random random1 = new Random(System.currentTimeMillis());
		Random random2 = new Random(System.currentTimeMillis() << 20);
		
		for (int i = 0; i < num; i++) {
			ps[i] = new Point(Math.round(random1.nextFloat() * r),
					Math.round(random2.nextFloat() * r));
		}
		return ps;
	}
	public static void copyxy(){
		System.arraycopy(points, 0, pointsY, 0, points.length);
	}
	public static Point[] getClosest_2(){//n_2次的算法
		double minDist = Double.MAX_VALUE;
		Point[] result = new Point[2];
		for(int i =0; i < points.length; i++){
			for(int j=i+1; j < points.length; j++){
				double dist = points[i].dist(points[j]);
				if(dist < minDist){
					minDist = dist;
					result[0] = points[i];
					result[1] = points[j];
				}
			}
		}
		return result;
	}
	public static Point[] getClosest_2(int s, int e){//n_2次的算法
		double minDist = Double.MAX_VALUE;
		Point[] result = new Point[2];
		for(int i =s; i <= e; i++){
			for(int j=i+1; j <=e; j++){
				double dist = points[i].dist(points[j]);
				if(dist < minDist){
					minDist = dist;
					result[0] = points[i];
					result[1] = points[j];
				}
			}
		}
		return result;
	}
	public static void main(String[] args) {
		int[] ps = new int[]{
				10,
				100,
				1000,
				10000,
				100000,
				1000000
		};
		
		long startTime;
		long endTime;
		for(int i = 0; i < ps.length; i++){
			Algorithm.generatedRandom(points, ps[i], MaxNum);
			startTime = System.currentTimeMillis();
			Algorithm.sortbyx(Algorithm.points, 0, ps[i]-1);
			Algorithm.copyxy();
			Algorithm.sortbyy(Algorithm.pointsY, 0, ps[i]-1);
			Point[] result = Algorithm.getClosest(points, 0, ps[i]-1);
			endTime = System.currentTimeMillis();
			System.out.println("处理方法：nlog(n),点数:"+ps[i]+":");
			System.out.println("最小距离为："+result[0].dist(result[1]));
			System.out.println("两最近点分别为： "+result[0]+" "+result[1]);
			System.out.println("用时:"+(endTime - startTime)+"毫秒");
		}
		
		for(int i = 0; i < ps.length; i++){
			Algorithm.generatedRandom(points, ps[i], MaxNum);
			startTime = System.currentTimeMillis();
			Point[] result = Algorithm.getClosest_2(0, ps[i]-1);
			endTime = System.currentTimeMillis();
			System.out.println("处理方法：n平方,点数:"+ps[i]+":");
			System.out.println("最小距离为："+result[0].dist(result[1]));
			System.out.println("两最近点分别为： "+result[0]+" "+result[1]);
			System.out.println("用时:"+(endTime - startTime)+"毫秒");
		}
		
	}
};
