import java.util.HashSet;
import java.util.Iterator;
import java.util.Vector;

/*
 * 存放unit group 的并集
 * */
public class Group {
	@Override
	public String toString() {
		return "Group [unitGroup=" + unitGroup + "]";
	}

	// unit group 合并后的结果
	public HashSet<PData> unitGroup;
	// 记录是由几个unit group 组合得来的
	public int level;
	
	//记录最原始的PData
	public HashSet<PData> originals;

	// 本group最下端的元素,level1 的时候使用
	public PData original;
	
	public Group() {
		level = 0;
		unitGroup = new HashSet<PData>();
		originals = new HashSet<PData>();
	}

	// 合并一个新的 unit group
	public Group(Group g1, PData u) {
		unitGroup = new HashSet<PData>();
		originals = new HashSet<PData>();
		
		this.originals.addAll(g1.originals);
		this.originals.add(u);
		
		this.unitGroup.addAll(g1.unitGroup);
		this.unitGroup.add(u);
		this.unitGroup.addAll(u.parents);
		this.level = g1.level + 1;
	}
	
	@Override
	public int hashCode() {
		final int prime = 31;
		int result = 1;
		result = prime * result
				+ ((originals == null) ? 0 : originals.hashCode());
		return result;
	}

	@Override
	public boolean equals(Object obj) {
		if (this == obj)
			return true;
		if (obj == null)
			return false;
		if (getClass() != obj.getClass())
			return false;
		Group other = (Group) obj;
		if (originals == null) {
			if (other.originals != null)
				return false;
		} else if (!originals.equals(other.originals))
			return false;
		return true;
	}

	
}
