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

import java.awt.GridBagConstraints;
import java.awt.GridBagLayout;

import gui.control.*;

import javax.swing.JPanel;

/**
 * Create default control panel
 * 
 * @author julus
 * 
 */
public class ControlPanel extends JPanel {
	private static final long serialVersionUID = -2342321883822171371L;

	// connection status panel
	private ConnectionPanel connection;
	// title panel
	private TitlePanel title;
	// log panel
	private LogPanel log;

	/**
	 * Create the control panel
	 * 
	 * @param owner
	 *            Frame parent
	 */
	ControlPanel(MainFrame owner) {
		super(new GridBagLayout());

		// create connection panel
		GridBagConstraints c = new GridBagConstraints();
		connection = new ConnectionPanel(owner);
		c.fill = GridBagConstraints.HORIZONTAL;
		c.gridx = 0;
		c.weightx = 1.0;
		c.gridy = 0;
		add(connection, c);

		// create log panel
		c = new GridBagConstraints();
		log = new LogPanel();
		c.fill = GridBagConstraints.BOTH;
		c.gridx = 0;
		c.weightx = 1.0;
		c.weighty = 0.2;
		c.gridy = 2;
		add(log, c);

		// create title panel
		c = new GridBagConstraints();
		title = new TitlePanel(owner);
		c.fill = GridBagConstraints.BOTH;
		c.gridx = 0;
		c.weightx = 1.0;
		c.weighty = 1.0;
		c.gridy = 3;
		add(title, c);
	}

	/**
	 * Write message to log
	 * 
	 * @param str
	 *            Message to log
	 */
	public void log(String str) {
		log.log(str);
	}

	/**
	 * Reset this control pane
	 */
	public void reset() {
		// reset connect
		connection.reset();
		// reset log
		log.reset();
		// reset title
		title.reset();
	}

	/**
	 * Get the connection panel
	 * 
	 * @return Connection panel
	 */
	public ConnectionPanel getConnectionPanel() {
		return connection;
	}

	/**
	 * Get the title panel
	 * 
	 * @return Title panel
	 */
	public TitlePanel getTitlePanel() {
		return title;
	}
}
