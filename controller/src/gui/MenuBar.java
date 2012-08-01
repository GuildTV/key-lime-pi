package gui;

import javax.swing.JMenu;
import javax.swing.JMenuBar;
import javax.swing.JMenuItem;

public class MenuBar extends JMenuBar {
	private static final long serialVersionUID = -5511353684955783810L;

	private JMenu fileMenu;
	private JMenu helpMenu;

	public MenuBar() {

		buildFileMenu();

		buildHelpMenu();
	}

	private void buildFileMenu() {
		fileMenu = new JMenu("File");
		add(fileMenu);
		
		JMenuItem save = new JMenuItem("Save Titles");
		fileMenu.add(save);
		
		JMenuItem load = new JMenuItem("Load Titles");
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
}
