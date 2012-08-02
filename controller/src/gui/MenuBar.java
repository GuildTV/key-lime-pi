package gui;

import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;

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
		if(e.getActionCommand().equals("Save")){
			owner.saveTitles("out.json");
			
		} else if(e.getActionCommand().equals("Load")){
			owner.loadTitles("out.json");
			
		}
		
	}
}
