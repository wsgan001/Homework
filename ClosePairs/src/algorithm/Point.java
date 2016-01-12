package algorithm;

public class Point {
	public int x;
	public int y;

	@Override
	public String toString() {
		return "Point [x=" + x + ", y=" + y + "]";
	}

	public Point() {
	}

	public Point(Point other) {
		this.x = other.x;
		this.y = other.y;
	}

	public Point(int x, int y) {
		super();
		this.x = x;
		this.y = y;
	}

	public double squaredist(Point other) {
		return Math.pow(this.x - other.x, 2) + Math.pow(this.y - other.y, 2);
	}
	
	public double dist(Point other){ return Math.sqrt(squaredist(other)); }
	
	public static double dist(Point p1, Point p2){ 
		return Math.sqrt(p1.squaredist(p2)); 
	}
	public static double squaredist(Point p1, Point p2) {
		return p1.squaredist(p2);
	}
}
