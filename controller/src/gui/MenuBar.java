package gui;

import java.awt.Graphics;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;

import javax.swing.JFileChooser;
import javax.swing.JMenu;
import javax.swing.JMenuBar;
import javax.swing.JMenuItem;

public class MenuBar extends JMenuBar implements ActionListener {
	private static final long serialVersionUID = -5511353684955783810L;

	private MainFrame owner;

	private JMenu fileMenu;
	private JMenu helpMenu;

	private JMenuItem newShow;
	private JMenuItem save;
	private JMenuItem load;
	private JMenuItem connect;
	private JMenuItem disconnect;
	private JMenuItem close;

	public MenuBar(MainFrame owner) {
		this.owner = owner;

		buildFileMenu();

		buildHelpMenu();
	}

	private void buildFileMenu() {
		fileMenu = new JMenu("File");
		add(fileMenu);

		
		newShow = new JMenuItem("New Show");
		newShow.addActionListener(this);
		newShow.setActionCommand("New");
		fileMenu.add(newShow);
		
		save = new JMenuItem("Save Show");
		save.addActionListener(this);
		save.setActionCommand("Save");
		fileMenu.add(save);

		load = new JMenuItem("Load Show");
		load.addActionListener(this);
		load.setActionCommand("Load");
		fileMenu.add(load);

		connect = new JMenuItem("Connect");
		connect.addActionListener(this);
		connect.setActionCommand("Connect");
		fileMenu.add(connect);
		
		disconnect = new JMenuItem("Disconnect");
		disconnect.addActionListener(this);
		disconnect.setActionCommand("Disconnect");
		fileMenu.add(disconnect);

		close = new JMenuItem("Close");
		fileMenu.add(close);
	}

	protected void paintComponent(Graphics g) {
		super.paintComponent(g);

		if (owner.getControl().isLive()) {
			save.setEnabled(false);
			load.setEnabled(false);
			disconnect.setEnabled(false);
			close.setEnabled(false);
		} else {
			save.setEnabled(true);
			load.setEnabled(true);
			disconnect.setEnabled(true);
			close.setEnabled(true);
		}
	}

	private void buildHelpMenu() {
		helpMenu = new JMenu("Help");
		add(helpMenu);

		JMenuItem menuItem = new JMenuItem("HAHAHAHA!!");
		helpMenu.add(menuItem);
	}

	@Override
	public void actionPerformed(ActionEvent e) {
		if (e.getActionCommand().equals("Save")) {
			saveDialog();
		} else if (e.getActionCommand().equals("Load")) {
			loadDialog();
		} else if (e.getActionCommand().equals("Connect")){
			owner.getControl().showConnect();
		} else if (e.getActionCommand().equals("Disconnect")){
			owner.getControl().disconnect();
		} else if (e.getActionCommand().equals("New")){
			owner.resetControl();
			owner.revalidate();
			owner.repaint();
		}

	}

	public void saveDialog() {
		JFileChooser chooser = new JFileChooser(".");
		ExampleFileFilter filter = new ExampleFileFilter();
		filter.addExtension("lime");
		filter.setDescription("Lime files");
		chooser.setFileFilter(filter);
		int optionSelected = chooser.showSaveDialog(this);
		if (optionSelected == JFileChooser.APPROVE_OPTION) {
			String fileName = chooser.getSelectedFile().getName();

			if (fileName.lastIndexOf(".lime") != fileName.length() - 5)
				fileName += ".lime";

			owner.saveTitles(fileName);
		}

	}

	public void loadDialog() {
		JFileChooser chooser = new JFileChooser(".");
		ExampleFileFilter filter = new ExampleFileFilter();
		filter.addExtension("lime");
		filter.setDescription("Lime files");
		chooser.setFileFilter(filter);
		int optionSelected = chooser.showOpenDialog(this);
		if (optionSelected == JFileChooser.APPROVE_OPTION) {
			String fileName = chooser.getSelectedFile().getName();

			if (fileName.lastIndexOf(".lime") != fileName.length() - 5)
				fileName += ".lime";

			owner.loadTitles(fileName);
		}

	}
	
	public void setVisConnect(boolean b) {
		connect.setEnabled(b);
	}
	public void setVisDisconnect(boolean b) {
		disconnect.setEnabled(b);
	}
}
