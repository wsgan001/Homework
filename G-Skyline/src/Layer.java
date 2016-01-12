import java.util.Vector;

/*
 * DSG的Layer层
 * */
public class Layer {
	PData tailPoint;
	Vector<PData> points;

	public Layer() {
		this.points = new Vector<PData>();
	}

	public PData getTailPoint() {
		return tailPoint;
	}
}
