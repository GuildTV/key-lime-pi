package gui.control;

import gui.BottomBorder;

import java.awt.Color;
import java.awt.Dimension;
import java.awt.Font;
import java.awt.Graphics;
import java.awt.GridBagConstraints;
import java.awt.GridBagLayout;
import java.awt.Insets;
import java.awt.event.MouseEvent;
import java.awt.event.MouseListener;

import javax.swing.DefaultListModel;
import javax.swing.JButton;
import javax.swing.JLabel;
import javax.swing.JList;
import javax.swing.JPanel;
import javax.swing.ListSelectionModel;

public class TitleElement extends JPanel implements MouseListener {
	private static final long serialVersionUID = 3610843924974947348L;

	private String nameValue = "";
	private String scriptName = "";

	private TitlePanel parent;

	private JLabel nameLabel;
	private JLabel scriptLabel;
	private JList<TitleData> dataList;
	private JButton previewButton;
	private JButton playButton;

	private DefaultListModel<TitleData> listModel;

	public TitleElement(TitlePanel parent) {
		super(new GridBagLayout());
		this.parent = parent;

		setBorder(new BottomBorder(Color.GRAY));
		addMouseListener(this);

		setMaximumSize(new Dimension(Short.MAX_VALUE, 65));// TODO replace this, but doesnt work otherwise :'(

		nameLabel = new JLabel(nameValue);
		nameLabel.setFont(new Font("Dialog", 1, 20));
		GridBagConstraints c = new GridBagConstraints();
		c.fill = GridBagConstraints.HORIZONTAL;
		c.gridx = 0;
		c.gridy = 0;
		c.insets = new Insets(5, 5, 5, 5);
		add(nameLabel, c);

		scriptLabel = new JLabel(scriptName);
		c = new GridBagConstraints();
		c.fill = GridBagConstraints.HORIZONTAL;
		c.gridx = 0;
		c.gridy = 1;
		c.insets = new Insets(0, 5, 0, 5);
		add(scriptLabel, c);

		listModel = new DefaultListModel<TitleData>();
		dataList = new JList<TitleData>(listModel);
		dataList.setSelectionMode(ListSelectionModel.SINGLE_INTERVAL_SELECTION);
		dataList.setLayoutOrientation(JList.VERTICAL);
		c = new GridBagConstraints();
		c.fill = GridBagConstraints.BOTH;
		c.gridx = 1;
		c.gridy = 0;
		c.gridheight = 2;
		c.weightx = 1.0;
		c.weighty = 1.0;
		c.insets = new Insets(5, 5, 5, 5);
		add(dataList, c);

		previewButton = new JButton("Preview");
		c = new GridBagConstraints();
		c.fill = GridBagConstraints.NONE;
		c.gridx = 2;
		c.gridheight = 2;
		c.gridy = 0;
		c.weighty = 1.0;
		c.insets = new Insets(5, 5, 5, 5);
		add(previewButton, c);

		playButton = new JButton("Load");
		c = new GridBagConstraints();
		c.fill = GridBagConstraints.NONE;
		c.gridx = 3;
		c.gridheight = 2;
		c.gridy = 0;
		c.weighty = 1.0;
		c.insets = new Insets(5, 5, 5, 5);
		add(playButton, c);
	}

	public TitleElement(TitlePanel parent, TitleElement orig) {
		this(parent);

		nameValue = orig.getNameValue();
		scriptName = orig.getScriptName();

		DefaultListModel<TitleData> model = orig.getListModel();
		for (int i = 0; i < model.size(); i++) {
			TitleData d = model.get(i);
			addData(d.getName(), d.getValue());
		}
	}

	@Override
	public void paintComponent(Graphics g) {
		super.paintComponent(g);

		if (getMyParent().getSelectedElement() == this)
			setBackground(Color.GRAY);
		else
			setBackground(Color.decode("#EEEEEE"));

		nameLabel.setText(nameValue);
		scriptLabel.setText(scriptName);
	}

	public void addData(String name, String value) {
		TitleData d = new TitleData(name, value);
		listModel.addElement(d);
	}

	public String getNameValue() {
		return nameValue;
	}

	public void setNameValue(String name) {
		nameValue = name;
	}

	public String getScriptName() {
		return scriptName;
	}

	public void setScriptName(String name) {
		scriptName = name;
	}

	public DefaultListModel<TitleData> getListModel() {
		return listModel;
	}

	public TitlePanel getMyParent() {
		return parent;
	}

	@Override
	public void mouseClicked(MouseEvent e) {
		// TODO Auto-generated method stub

	}

	@Override
	public void mousePressed(MouseEvent e) {
		if (getMyParent().getSelectedElement() == this) {
			getMyParent().setSelectedElement(null);
			getMyParent().repaint();
		} else {
			getMyParent().setSelectedElement(this);
			getMyParent().repaint();
		}
	}

	@Override
	public void mouseReleased(MouseEvent e) {
		// TODO Auto-generated method stub

	}

	@Override
	public void mouseEntered(MouseEvent e) {
		// TODO Auto-generated method stub

	}

	@Override
	public void mouseExited(MouseEvent e) {
		// TODO Auto-generated method stub

	}
}
