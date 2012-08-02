package gui;

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

	public MenuBar(MainFrame owner) {
		this.owner = owner;

		buildFileMenu();

		buildHelpMenu();
	}

	private void buildFileMenu() {
		fileMenu = new JMenu("File");
		add(fileMenu);

		JMenuItem save = new JMenuItem("Save Titles");
		save.addActionListener(this);
		save.setActionCommand("Save");
		fileMenu.add(save);

		JMenuItem load = new JMenuItem("Load Titles");
		load.addActionListener(this);
		load.setActionCommand("Load");
		fileMenu.add(load);

		JMenuItem disconnect = new JMenuItem("Disconnect");
		fileMenu.add(disconnect);

		JMenuItem close = new JMenuItem("Close");
		fileMenu.add(close);
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
}
