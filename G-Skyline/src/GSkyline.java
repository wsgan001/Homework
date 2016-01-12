import java.io.BufferedReader;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStreamReader;
import java.util.ArrayList;
import java.util.Collections;
import java.util.Comparator;
import java.util.HashMap;
import java.util.HashSet;
import java.util.Iterator;
import java.util.Vector;

public class GSkyline {
	public static final String datafolder = "./dataset/";
	public static final String[] filepaths = new String[] { "corr_2.txt",
			"corr_4.txt", "corr_6.txt", "corr_8.txt", "inde_2.txt",
			"inde_4.txt", "inde_6.txt", "inde_8.txt", "anti_2.txt",
			"anti_4.txt", "anti_6.txt", "anti_8.txt", };
	public static final String output_folder = "./output/";

	public GSkyline() {

	}

	private Vector<PData> readPoints(String filePath) throws IOException {
		Vector<PData> points = new Vector<PData>();
		File file = new File(datafolder + filePath);
		BufferedReader reader = new BufferedReader(new InputStreamReader(
				new FileInputStream(file)));
		String line = null;
		while ((line = reader.readLine()) != null) {
			String[] attrsStrings = line.split(" ");
			Float[] attrs = new Float[attrsStrings.length];
			for (int i = 0; i < attrsStrings.length; i++) {
				attrs[i] = Float.parseFloat(attrsStrings[i]);
			}
			points.addElement(new PData(attrs));
		}
		reader.close();
		return points;
	}

	private Vector<Layer> buildLayer(Vector<PData> points) throws Exception {
		if (points == null || points.size() <= 0) {
			throw new Exception("Empty points");
		}
		// 暂时不考虑一维
		if (points.get(0).getDimension() == 2) {
			return buildLayer2D(points);
		} else {
			return buildLayerHD(points);
		}
	}

	private Vector<Layer> buildLayer2D(Vector<PData> points) {
		Collections.sort(points, new Comparator<PData>() {
			@Override
			public int compare(PData o1, PData o2) {
				// TODO Auto-generated method stub
				if (o2.attr[0] - o1.attr[0] > 0) {
					return -1;
				} else if (o2.attr[0] - o1.attr[0] == 0) {
					return 0;
				} else {
					return 1;
				}
			}

		});
		for (int i = 0; i < points.size(); i++) {
			points.get(i).index = i;
		}
		Vector<Layer> layers = new Vector<Layer>();
		Layer layer0 = new Layer();
		int maxLayer = 0;
		layer0.tailPoint = points.get(0);
		layer0.points.addElement(points.get(0));
		layers.addElement(layer0);

		for (int i = 1; i < points.size(); i++) {
			if (!layers.get(0).getTailPoint().dominate(points.get(i))) {
				points.get(i).layer = 0;
				layers.get(0).tailPoint = points.get(i);
				layers.get(0).points.addElement(points.get(i));
			} else if (layers.get(maxLayer).getTailPoint()
					.dominate(points.get(i))) {

				points.get(i).layer = ++maxLayer;
				Layer newLayer = new Layer();
				layers.add(maxLayer, newLayer);
				layers.get(maxLayer).tailPoint = points.get(i);
				layers.get(maxLayer).points.addElement(points.get(i));
			} else {
				int layerIndex = bSearchLayer(layers, points.get(i), maxLayer);
				if (layerIndex == -1) {
					System.err.println("ERROR BSERACH");
				}

				points.get(i).layer = layerIndex;
				layers.get(layerIndex).tailPoint = points.get(i);
				layers.get(layerIndex).points.addElement(points.get(i));
			}
		}
		return layers;
	}

	/*
	 * 二分法查找合适的layer, 找不到返回-1
	 */
	private int bSearchLayer(Vector<Layer> layers, PData p, int maxlayer) {
		int s = 1;
		int e = maxlayer;
		if (maxlayer < s) {
			return -1;
		}
		while (e - s >= 0) {
			int m = (s + e) / 2;
			if (!layers.get(m).getTailPoint().dominate(p)) {
				if (m > 0 && layers.get(m - 1).getTailPoint().dominate(p)) {
					return m;
				} else {
					e = m - 1;
				}
			} else {
				s = m + 1;
			}
		}
		return -1;
	}

	private Vector<Layer> buildLayerHD(Vector<PData> points) {
		Collections.sort(points, new Comparator<PData>() {
			@Override
			public int compare(PData o1, PData o2) {
				// TODO Auto-generated method stub
				if (o2.attr[0] - o1.attr[0] > 0) {
					return -1;
				} else if (o2.attr[0] - o1.attr[0] == 0) {
					return 0;
				} else {
					return 1;
				}
			}

		});
		for (int i = 0; i < points.size(); i++) {
			points.get(i).index = i;
		}
		Vector<Layer> layers = new Vector<Layer>();
		Layer layer0 = new Layer();
		int maxLayer = 0;
		layer0.points.addElement(points.get(0));
		layers.addElement(layer0);

		for (int i = 1; i < points.size(); i++) {
			if (!isLayerDominate(points, layers.get(0), points.get(i))) {
				points.get(i).layer = 0;
				layers.get(0).points.addElement(points.get(i));
			} else if (isLayerDominate(points, layers.get(maxLayer),
					points.get(i))) {
				points.get(i).layer = ++maxLayer;
				Layer newLayer = new Layer();
				newLayer.points.addElement(points.get(i));
				layers.add(maxLayer, newLayer);
			} else {
				for (int j = maxLayer; j > 0; j--) {
					if (!isLayerDominate(points, layers.get(j), points.get(i))
							&& isLayerDominate(points, layers.get(j - 1),
									points.get(i))) {
						points.get(i).layer = j;
						layers.get(j).points.addElement(points.get(i));
						break;
					}
				}
			}
		}
		return layers;
	}

	/*
	 * 高维的时候检测一个Layer是否能够有点能够dominate另一个点p
	 */
	private boolean isLayerDominate(Vector<PData> points, Layer layer, PData p) {
		for (int i = 0; i < layer.points.size(); i++) {
			if (layer.points.get(i).dominate(p)) {
				return true;
			}
		}
		return false;
	}

	private Vector<PData> buildDSG(Vector<Layer> layers, Vector<PData> points,
			int groupSize) {
		/*
		 * 只使用前 groupSize 层的元素
		 */
		Vector<PData> newPoints = new Vector<PData>();
		for (int i = 0; i < layers.size() && i < groupSize; i++) {
			Layer layer = layers.get(i);
			for (int j = 0; j < layer.points.size(); j++) {
				newPoints.add(layer.points.get(j));
			}
		}
		points = newPoints;

		for (int i = 1; i < layers.size() && i < groupSize; i++) {// 层循环
			Layer layer = layers.get(i); // 本层
			Layer flayer = layers.get(i - 1);// 父层
			for (int j = 0; j < layer.points.size(); j++) {// 本层元素
				for (int k = 0; k < flayer.points.size(); k++) {// 父层元素
					if (flayer.points.get(k).dominate(layer.points.get(j))) {
						layer.points.get(j).parents.add(flayer.points.get(k));
						layer.points.get(j).parents
								.addAll(flayer.points.get(k).parents);
						flayer.points.get(k).children.add(layer.points.get(j));
					}
				}
			}
		}

		return points;
	}

	/*
	 * points 是一个建立了children和parent的DSG
	 */
	public int UGWSkyline(Vector<PData> points, int groupSize, String filepath)
			throws IOException {
		// 结果集
		int outputSize = 0;
		File file = new File(output_folder + "groupSize_" + groupSize + "_"
				+ filepath);
		FileOutputStream fo;
		if (!file.exists()) {
			file.createNewFile();
		}
		fo = new FileOutputStream(file);

		// 倒排序
		Collections.sort(points, new Comparator<PData>() {
			@Override
			public int compare(PData o1, PData o2) {
				// TODO Auto-generated method stub
				return o1.index - o2.index;
			}
		});

		Vector<Group> groups = new Vector<Group>();
		// 删除那些超过groupSize的点
		for (PData pData : points) {
			if (pData.parents.size() + 1 > groupSize) {
				continue;
			} else if (pData.parents.size() + 1 == groupSize) {
				Group group = new Group();
				group.unitGroup.add(pData);
				group.unitGroup.addAll(pData.parents);
				group.level = 1;
				fo.write((group.toString() + '\n').getBytes());
				outputSize++;
			} else {
				Group group = new Group();
				group.unitGroup.add(pData);
				group.unitGroup.addAll(pData.parents);
				group.originals.add(pData);
				group.original = pData;
				group.level = 1;
				groups.add(group);
			}
		}
//
//		System.out.println("Level 1 UnitGroup Number= " + groups.size());
//		System.out.println("Current Output Size:" + outputSize);

		for (int i = 0; i < groups.size(); i++) {
			// 计算G-last
			Group glast = new Group();
			for (int j = i; j < groups.size(); j++) {// tail set of each group
				glast.level++;
				glast.unitGroup.addAll(groups.get(j).unitGroup);
				if (glast.unitGroup.size() + 1 > groupSize) {
					break;
				}
			}
			if (glast.unitGroup.size() + 1 < groupSize) {
				break;
			} else if (glast.unitGroup.size() + 1 == groupSize) {
				fo.write((glast.toString() + '\n').getBytes());
				outputSize++;
				break;
			}

			HashMap<Integer, Vector<Group>> lists = new HashMap<Integer, Vector<Group>>();

			// 由一个unit group 组成的集合，只有当前的元素 groups[i]
			Vector<Group> level1 = new Vector<Group>();
			level1.add(groups.get(i));
			lists.put(1, level1);

			for (int level = 1; lists.get(level) != null;) {
				// |G'|_u not empty
				Vector<Group> list = lists.get(level);
				for (int k = 0; k < list.size(); k++) {
					// for each G' in this level, do
					Group group = list.get(k);
					// tail set of list[k]
					for (int j = i + 1; j < groups.size(); j++) {
						// 每次添加一个level1的unitgroup
						Group tailset = groups.get(j);
						PData pData = tailset.original;
						// unit group in each tail set
						if (!group.unitGroup.contains(pData)) {
							Group unionGroup = new Group(group, pData);
							if (unionGroup.unitGroup.size() == groupSize) {
								fo.write((unionGroup.toString() + '\n')
										.getBytes());
								outputSize++;
							} else if (unionGroup.unitGroup.size() < groupSize) {
								if (lists.get(level + 1) == null) {
									lists.put(level + 1, new Vector<Group>());
								}
								lists.get(level + 1).add(unionGroup);
							}
						}
					}

				}
				level++;
				lists.remove(level - 1);
			}
		}
		fo.close();
		return outputSize;
	}

	public static void main(String[] args) {
		GSkyline gSkyline = new GSkyline();
		long st;
		long et;
		try {
				for (int i = 0; i < filepaths.length; i++) {
					for (int gs = 1; gs <= 2; gs++) {
						st = System.currentTimeMillis();
						System.out.println();
						System.out.println("===========Start TestCase: file="
								+ filepaths[i] + ", groupsize=" + gs+"==========");
						Vector<PData> points = gSkyline
								.readPoints(filepaths[i]);
						System.out.println("n(Record)="+points.size());
						try {
							Vector<Layer> layers = gSkyline.buildLayer(points);
							System.out
									.println("Finish Build Layer, Layer Size:"
											+ layers.size());
							Vector<PData> dsg = gSkyline.buildDSG(layers,
									points, gs);
							System.out.println("Finish Build DSG,n(DSG nodes)="
									+ dsg.size());
							Integer outputSize = gSkyline.UGWSkyline(dsg, gs,
									filepaths[i]);

							et = System.currentTimeMillis();
							System.out.println("OutputSize=" + outputSize+" ,Time: "+(et-st)+"ms");
						} catch (Exception e) {
							e.printStackTrace();
						}
					}
				}
			
		} catch (IOException e) {
			e.printStackTrace();
		}
	}

}
