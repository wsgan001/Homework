package ui;

import java.awt.BasicStroke;
import java.awt.Color;
import java.awt.Dimension;
import java.awt.Graphics;
import java.awt.Graphics2D;
import java.awt.event.MouseAdapter;
import java.awt.event.MouseEvent;

import javax.swing.BorderFactory;
import javax.swing.BoxLayout;
import javax.swing.JButton;
import javax.swing.JFrame;
import javax.swing.JLabel;
import javax.swing.JOptionPane;
import javax.swing.JPanel;
import javax.swing.JTextField;
import javax.swing.JToolBar;
import javax.swing.SwingUtilities;

import algorithm.Algorithm;
import algorithm.Point;

public class Main {

	public static void main(String[] args) {
		SwingUtilities.invokeLater(new Runnable() {
			public void run() {
				createAndShowGUI();
			}
		});
	}

	private static void createAndShowGUI() {
		JFrame f = new JFrame("Closest Points");
		f.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
		f.setLayout(new BoxLayout(f.getContentPane(), BoxLayout.Y_AXIS));
		final JToolBar toolbar = new JToolBar();
		final ContentPanel contentPanel = new ContentPanel();

		final JButton clearBtn = new JButton("清空");
		final JButton getcloseBtn = new JButton("标记最近点");
		final JLabel modeLabel = new JLabel("当前模式：鼠标选取模式");
		final JLabel positionLabel = new JLabel("鼠标位置：0,0");
		final JLabel genRandomLabel = new JLabel("输入要随机生成的点数");
		final JTextField genRandomTextField = new JTextField();
		final JLabel timeLabel = new JLabel("查找最近点耗时：0毫秒");
		timeLabel.setPreferredSize(new Dimension(200, 30));
		final JButton genRandomBtn = new JButton("生成");
		toolbar.setLayout(new BoxLayout(toolbar, BoxLayout.X_AXIS));
		toolbar.add(modeLabel);
		toolbar.addSeparator();
		toolbar.add(clearBtn);
		toolbar.addSeparator();
		toolbar.add(getcloseBtn);
		toolbar.add(timeLabel);
		toolbar.addSeparator();
		toolbar.add(genRandomLabel);
		toolbar.add(genRandomTextField);
		genRandomTextField.setMaximumSize(new Dimension(150, 30));
		toolbar.add(genRandomBtn);
		toolbar.addSeparator();
		toolbar.add(positionLabel);
		toolbar.setAlignmentX(0);
		toolbar.setMinimumSize(new Dimension(999, 30));
		toolbar.setFloatable(false);

		clearBtn.addMouseListener(new MouseAdapter() {
			@Override
			public void mouseClicked(MouseEvent e) {
				contentPanel.clear();
				ContentPanel.currentIndex = 0;
				contentPanel.repaint();
			}
		});
		getcloseBtn.addMouseListener(new MouseAdapter() {
			@Override
			public void mouseClicked(MouseEvent e) {
				if (ContentPanel.currentIndex <= 1) {
					return;
				}
				long startTime = System.currentTimeMillis();
				Algorithm.sortbyx(Algorithm.points, 0,
						ContentPanel.currentIndex - 1);
				Algorithm.copyxy();
				Algorithm.sortbyy(Algorithm.pointsY, 0,
						ContentPanel.currentIndex - 1);
				Point[] closes = Algorithm.getClosest(Algorithm.points, 0,
						ContentPanel.currentIndex - 1);
				long endTime = System.currentTimeMillis();
				timeLabel.setText("查找最近点耗时："+(endTime-startTime)+"毫秒");
				ContentPanel.curMinPair[0] = closes[0];
				ContentPanel.curMinPair[1] = closes[1];
				ContentPanel.curMinPair[2] = new Point(1,1);
				contentPanel.repaint();
			}
		});
		genRandomBtn.addMouseListener(new MouseAdapter(){
			public void mouseClicked(MouseEvent e) {
				String proposedValue = genRandomTextField.getText();
				int parsedValue = -1;
				try {
				       parsedValue = Integer.parseInt(proposedValue);
				       if(parsedValue <= 0 || parsedValue > 1000000){
				    	   throw new NumberFormatException();
				       }
				} catch (NumberFormatException nfe) {
				       JOptionPane.showMessageDialog(null, "请输入正确随机点数量，必须为正整数,不超过1000000");
				       return;
				}
				contentPanel.clear();
				ContentPanel.currentIndex = parsedValue;
				Algorithm.generatedRandom(Algorithm.points, parsedValue,1000);
				contentPanel.repaint();
				
			}
		});
		contentPanel.addMouseMotionListener(new MouseAdapter() {

			@Override
			public void mouseMoved(MouseEvent e) {
				// TODO Auto-generated method stub
				positionLabel.setText("鼠标位置：" + e.getX() + "," + e.getY());
			}

		});
		f.add(toolbar);
		f.add(contentPanel);
		f.pack();
		f.setVisible(true);
	}
}

class ContentPanel extends JPanel {
	private static final int dotSize = 3;
	private static final int width = 1000;
	private static final int height = 1000;

	public static int currentIndex = 0;
	private static boolean shouldClear = false;
	public static Point[] curMinPair = new Point[] { null, null, new Point(-1,-1) };

	public ContentPanel() {
		setBorder(BorderFactory.createLineBorder(Color.black));
		addMouseListener(new MouseAdapter() {
			public void mousePressed(MouseEvent e) {
				addPoint(e.getX(), e.getY());
			}
		});
	}

	private void addPoint(int x, int y) {
		if(currentIndex >= Algorithm.MaxNum){
			return;
		}
		Algorithm.points[currentIndex++] = new Point(x, y);
		repaint(x, y, dotSize + 1, dotSize + 1);
	}

	public Dimension getPreferredSize() {
		return new Dimension(width, height);
	}

	public void clear() {
		shouldClear = true;
		currentIndex = 0;
		curMinPair[2].x = curMinPair[2].y = -1;
		repaint();
	}

	protected void paintComponent(Graphics g) {
		super.paintComponent(g);
		if (shouldClear) {
			g.setColor(Color.WHITE);
			g.fillRect(0, 0, getWidth(), getHeight());
			shouldClear = false;
		}
		g.setColor(Color.WHITE);
		g.fillRect(0, 0, getWidth(), getHeight());
		g.setColor(Color.BLACK);
		
		for (int i = 0; i < currentIndex; i++) {
			g.fillOval(Algorithm.points[i].x, Algorithm.points[i].y,
					dotSize, dotSize);
		}

		if (curMinPair[2].x != -1) {
			
			((Graphics2D) g).setStroke(new BasicStroke(2));
			g.setColor(Color.BLUE);
			g.drawOval(curMinPair[0].x, curMinPair[0].y, 5, 5);
			g.drawOval(curMinPair[1].x, curMinPair[1].y, 5, 5);
			g.setColor(Color.RED);
			g.drawLine(curMinPair[0].x,
					curMinPair[0].y,
					curMinPair[1].x,
					curMinPair[1].y);
			((Graphics2D) g).setStroke(new BasicStroke(1));
		}
	}
}
