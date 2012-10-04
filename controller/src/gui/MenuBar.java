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

import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;

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
	private JMenuItem connect;
	private JMenuItem disconnect;
	private JMenuItem refresh;
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

		// create connect button
		refresh = new JMenuItem("Refresh Burns");
		refresh.addActionListener(this);
		refresh.setActionCommand("Refresh");
		fileMenu.add(refresh);
		
		// create close button
		close = new JMenuItem("Close");
		fileMenu.add(close);
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
		if (e.getActionCommand().equals("Connect")) {
			owner.getControl().showConnect();

			// disconnect to master
		} else if (e.getActionCommand().equals("Disconnect")) {
			owner.getControl().disconnect();

			// refresh listings
		}else if (e.getActionCommand().equals("Refresh")) {
			owner.refreshBurns();
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
