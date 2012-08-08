package gui;
/*
 *      Copyright (C) 2012 GuildTV
 *      http://www.guildtv.co.uk
 *
 *  This Program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2, or (at your option)
 *  any later version.
 *
 *  This Program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; see the file COPYING.  If not, write to
 *  the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.
 *  http://www.gnu.org/copyleft/gpl.html
 *
 */

import java.awt.event.WindowAdapter;
import java.awt.event.WindowEvent;

import javax.swing.JFrame;

import common.StringUtil;

import lime.LimeController;

/**
 * Frame for controller
 * 
 * @author julus
 * 
 */
public class MainFrame extends JFrame {
	private static final long serialVersionUID = -7114459738550431366L;

	// base controller
	private LimeController control;
	// control panel
	private ControlPanel controlPanel;
	// menu bar
	private MenuBar menuBar;
	// valid script names - TODO dynamicify
	private String validScripts[] = { "new", "test" };

	/**
	 * Create frame
	 * 
	 * @param control
	 *            Controller parent
	 */
	public MainFrame(LimeController control) {
		this.control = control;

		// set window title
		setTitle("key-lime-pi Controller");

		// create and add menu bar
		menuBar = new MenuBar(this);
		setJMenuBar(menuBar);

		// create and add control panel
		controlPanel = new ControlPanel(this);
		add(controlPanel);

		// set window size
		setSize(800, 600);
		setVisible(true);

		// set window closing action
		addWindowListener(new WindowAdapter() {
			public void windowClosing(WindowEvent we) {
				System.exit(0);
			}
		});
	}

	/**
	 * Get the controller
	 * 
	 * @return Base controller
	 */
	public LimeController getControl() {
		return control;
	}

	/**
	 * Log message
	 * 
	 * @param str
	 *            Message to log
	 */
	public void log(String str) {
		controlPanel.log(str);
	}

	/**
	 * Reset the control panel
	 */
	public void resetControl() {
		controlPanel.reset();
	}

	/**
	 * Get the list of valid scripts
	 * 
	 * @return List of valid script names
	 */
	public String[] getValidScripts() {
		return validScripts;
	}

	/**
	 * Save titles
	 * 
	 * @param path
	 *            Filename to save to
	 */
	public void saveTitles(String path) {
		controlPanel.saveTitles(path);
	}

	/**
	 * Load titles
	 * 
	 * @param path
	 *            Filename to load from
	 */
	public void loadTitles(String path) {
		controlPanel.loadTitles(path);
	}

	/**
	 * Check if script name is valid
	 * 
	 * @param script
	 *            Script name to verify
	 * @return Whether name is valid
	 */
	public boolean isScriptValid(String script) {
		return StringUtil.in_array(script, validScripts);
	}

	/**
	 * Get the control panel
	 * 
	 * @return Return control panel
	 */
	public ControlPanel getControlPanel() {
		return controlPanel;
	}

	/**
	 * Get the menu bar
	 * 
	 * @return Get Menu bar
	 */
	public MenuBar getMyMenu() {
		return menuBar;
	}
}
