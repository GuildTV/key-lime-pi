package gui;

import java.awt.Graphics;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;

import javax.swing.JFileChooser;
import javax.swing.JMenu;
import javax.swing.JMenuBar;
import javax.swing.JMenuItem;

/**
 * Menubar for lime controller
 * 
 * @author julus
 * 
 */
public class MenuBar extends JMenuBar implements ActionListener {
	private static final long serialVersionUID = -5511353684955783810L;

	// frame containing menubar
	private MainFrame owner;

	// file menu
	private JMenu fileMenu;
	// help menu
	private JMenu helpMenu;

	// menu items
	private JMenuItem newShow;
	private JMenuItem save;
	private JMenuItem load;
	private JMenuItem connect;
	private JMenuItem disconnect;
	private JMenuItem close;

	/**
	 * Create menu
	 * 
	 * @param owner
	 *            Frame owner
	 */
	public MenuBar(MainFrame owner) {
		this.owner = owner;

		// build file menu
		buildFileMenu();

		// build help menu
		buildHelpMenu();
	}

	/**
	 * Build the gile menu
	 */
	private void buildFileMenu() {
		// create file menu
		fileMenu = new JMenu("File");
		add(fileMenu);

		// create new button
		newShow = new JMenuItem("New Show");
		newShow.addActionListener(this);
		newShow.setActionCommand("New");
		fileMenu.add(newShow);

		// create save button
		save = new JMenuItem("Save Show");
		save.addActionListener(this);
		save.setActionCommand("Save");
		fileMenu.add(save);

		// create load button
		load = new JMenuItem("Load Show");
		load.addActionListener(this);
		load.setActionCommand("Load");
		fileMenu.add(load);

		// create connect button
		connect = new JMenuItem("Connect");
		connect.addActionListener(this);
		connect.setActionCommand("Connect");
		fileMenu.add(connect);

		// create disconnect button
		disconnect = new JMenuItem("Disconnect");
		disconnect.addActionListener(this);
		disconnect.setActionCommand("Disconnect");
		fileMenu.add(disconnect);

		// create close button
		close = new JMenuItem("Close");
		fileMenu.add(close);
	}

	/**
	 * Modify painting
	 */
	protected void paintComponent(Graphics g) {
		super.paintComponent(g);

		// if in live mode, disable most buttons
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

	/**
	 * Create help menu
	 */
	private void buildHelpMenu() {
		// create help menu
		helpMenu = new JMenu("Help");
		add(helpMenu);

		// create button
		JMenuItem menuItem = new JMenuItem("HAHAHAHA!!");
		helpMenu.add(menuItem);
	}

	/**
	 * Handle button clicks
	 */
	@Override
	public void actionPerformed(ActionEvent e) {
		// save titles
		if (e.getActionCommand().equals("Save")) {
			saveDialog();

			// load titles
		} else if (e.getActionCommand().equals("Load")) {
			loadDialog();

			// connect to master
		} else if (e.getActionCommand().equals("Connect")) {
			owner.getControl().showConnect();

			// disconnect to master
		} else if (e.getActionCommand().equals("Disconnect")) {
			owner.getControl().disconnect();

			// create new show
		} else if (e.getActionCommand().equals("New")) {
			owner.resetControl();
			owner.revalidate();
			owner.repaint();
		}
	}

	/**
	 * Create save dialog box
	 */
	public void saveDialog() {
		// create dialog with filter
		JFileChooser chooser = new JFileChooser(".");
		ExampleFileFilter filter = new ExampleFileFilter();
		filter.addExtension("lime");
		filter.setDescription("Lime files");
		chooser.setFileFilter(filter);
		int optionSelected = chooser.showSaveDialog(this);

		// check save button was clicked
		if (optionSelected == JFileChooser.APPROVE_OPTION) {
			String fileName = chooser.getSelectedFile().getName();

			// enforce file extension
			if (fileName.lastIndexOf(".lime") != fileName.length() - 5)
				fileName += ".lime";

			// start save
			owner.saveTitles(fileName);
		}
	}

	/**
	 * Create load dialog box
	 */
	public void loadDialog() {
		// create dialog with filter
		JFileChooser chooser = new JFileChooser(".");
		ExampleFileFilter filter = new ExampleFileFilter();
		filter.addExtension("lime");
		filter.setDescription("Lime files");
		chooser.setFileFilter(filter);
		int optionSelected = chooser.showOpenDialog(this);

		// check load button was clicked
		if (optionSelected == JFileChooser.APPROVE_OPTION) {
			String fileName = chooser.getSelectedFile().getName();

			// enforce file extension
			if (fileName.lastIndexOf(".lime") != fileName.length() - 5)
				fileName += ".lime";

			// start load
			owner.loadTitles(fileName);
		}

	}

	/**
	 * Set enabled state of connect
	 * 
	 * @param b
	 *            Boolean state
	 */
	public void setVisConnect(boolean b) {
		connect.setEnabled(b);
	}

	/**
	 * Set enabled state of disconnect
	 * 
	 * @param b
	 *            Boolean state
	 */
	public void setVisDisconnect(boolean b) {
		disconnect.setEnabled(b);
	}
}
