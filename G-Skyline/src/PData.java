import java.util.Arrays;
import java.util.HashSet;

/*
 * 数据类
 * */
public class PData {
	public Float[] attr;
	public int layer;
	public int index;
	public HashSet<PData> parents;
	public HashSet<PData> children;

	public int getDimension() {
		return attr.length;
	}

	public PData(Float[] attr) {
		super();
		this.attr = attr;
		this.parents = new HashSet<PData>();
		this.children = new HashSet<PData>();
	}

	@SuppressWarnings("unchecked")
	public PData(PData other) {
		this.attr = other.attr.clone();
		this.layer = other.layer;
		this.index = other.index;
		this.parents = (HashSet<PData>) other.parents.clone();
		this.children = (HashSet<PData>) other.children.clone();
	}

	@Override
	public String toString() {
		return "PData: "+"layer=" + layer + ", attr="
				+ Arrays.toString(attr) + "]";
	}

	public boolean dominate(PData other) {
		boolean flag = false;
		for (int i = 0; i < other.attr.length; i++) {
			if (this.attr[i] > other.attr[i]) {
				return false;
			} else if (this.attr[i] < other.attr[i]) {
				flag = true;
			}
		}
		if (flag) {
			return true;
		} else {
			return false;
		}
	}

	@Override
	public int hashCode() {
		Integer integer = new Integer(index);
		return integer.hashCode();
	}

	@Override
	public boolean equals(Object obj) {
		if (this == obj)
			return true;
		if (obj == null)
			return false;
		if (getClass() != obj.getClass())
			return false;
		PData other = (PData) obj;
		if (index != other.index)
			return false;
		return true;
	}

}
