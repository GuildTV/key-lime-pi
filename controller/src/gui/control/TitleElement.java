package gui.control;

import gui.MainFrame;

import java.awt.Dimension;
import java.awt.Font;
import java.awt.Graphics;
import java.awt.GridBagConstraints;
import java.awt.GridBagLayout;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;

import javax.swing.BorderFactory;
import javax.swing.DefaultListModel;
import javax.swing.JButton;
import javax.swing.JLabel;
import javax.swing.JList;
import javax.swing.JPanel;
import javax.swing.ListSelectionModel;

public class TitleElement extends JPanel implements ActionListener {
	private static final long serialVersionUID = 3610843924974947348L;

	private String nameValue = "NAME";
	private String scriptName = "test";

	private MainFrame owner;
	private TitlePanel parent;

	private JLabel nameLabel;
	private JLabel scriptLabel;
	private JButton editButton;
	private JList<TitleData> dataList;
	private JButton loadButton;
	private JButton playButton;

	private DefaultListModel<TitleData> listModel;

	public TitleElement(TitlePanel parent, MainFrame owner) {
		super(new GridBagLayout());

		this.owner = owner;
		this.parent = parent;

		setBorder(BorderFactory.createTitledBorder("Temp Border"));
		this.setMinimumSize(new Dimension(600,20));

		setMaximumSize(new Dimension(Short.MAX_VALUE, 100));// TODO replace this, but doesnt work otherwise :'(

		nameLabel = new JLabel(nameValue);
		nameLabel.setFont(new Font("Dialog", 1, 20));
		GridBagConstraints cn = new GridBagConstraints();
		cn.fill = GridBagConstraints.HORIZONTAL;
		cn.gridx = 0;
		cn.gridy = 0;
		cn.ipadx = 10;
		add(nameLabel, cn);

		scriptLabel = new JLabel(scriptName);
		GridBagConstraints cs = new GridBagConstraints();
		cs.fill = GridBagConstraints.HORIZONTAL;
		cs.gridx = 0;
		cs.gridy = 1;
		cs.ipadx = 10;
		add(scriptLabel, cs);

		editButton = new JButton("Edit");
		editButton.addActionListener(this);
		GridBagConstraints ce = new GridBagConstraints();
		ce.fill = GridBagConstraints.NONE;
		ce.gridx = 0;
		ce.gridy = 2;
		ce.ipadx = 10;
		add(editButton, ce);

		listModel = new DefaultListModel<TitleData>();
		dataList = new JList<TitleData>(listModel);
		dataList.setSelectionMode(ListSelectionModel.SINGLE_INTERVAL_SELECTION);
		dataList.setLayoutOrientation(JList.VERTICAL);
		GridBagConstraints cd = new GridBagConstraints();
		cd.fill = GridBagConstraints.BOTH;
		cd.gridx = 1;
		cd.gridy = 0;
		cd.gridheight = 3;
		cd.weightx = 1.0;
		cd.weighty = 1.0;
		cd.ipadx = 10;
		add(dataList, cd);

		loadButton = new JButton("Load");
		GridBagConstraints cl = new GridBagConstraints();
		cl.fill = GridBagConstraints.NONE;
		cl.gridx = 2;
		cl.gridheight = 3;
		cl.gridy = 0;
		cl.weighty = 1.0;
		cl.ipadx = 10;
		add(loadButton, cl);

		playButton = new JButton("Play");
		GridBagConstraints cp = new GridBagConstraints();
		cp.fill = GridBagConstraints.NONE;
		cp.gridx = 3;
		cp.gridheight = 3;
		cp.gridy = 0;
		cp.weighty = 1.0;
		cp.ipadx = 10;
		add(playButton, cp);

		addData("Hi", "Boo");
		addData("Test", "123");
	}

	@Override
	public void paintComponent(Graphics g) {
		super.paintComponent(g);

		nameLabel.setText(nameValue);
		scriptLabel.setText(scriptName);
	}

	public void addData(String name, String value) {
		TitleData d = new TitleData(name, value);
		listModel.addElement(d);
	}

	@Override
	public void actionPerformed(ActionEvent e) {
		TitleElementEditDialog d = new TitleElementEditDialog(owner, this);
		d.setVisible(true);

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
}
