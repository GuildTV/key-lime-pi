package gui.control;
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

import java.awt.BorderLayout;

import javax.swing.BorderFactory;
import javax.swing.JPanel;
import javax.swing.JScrollPane;
import javax.swing.JTextArea;

/**
 * Log panel
 * 
 * @author julus
 * 
 */
public class LogPanel extends JPanel {
	private static final long serialVersionUID = -5018864002113251061L;

	// text area
	private JTextArea textArea;
	// scroll pane
	private JScrollPane scrollPane;

	/**
	 * Create log panel
	 */
	public LogPanel() {
		super(new BorderLayout());
		setBorder(BorderFactory.createTitledBorder("Message Log"));

		// create text area and add
		textArea = new JTextArea(4, 20);
		scrollPane = new JScrollPane(textArea, JScrollPane.VERTICAL_SCROLLBAR_ALWAYS,
				JScrollPane.HORIZONTAL_SCROLLBAR_AS_NEEDED);
		textArea.setEditable(false);
		add(scrollPane, BorderLayout.CENTER);
	}

	/**
	 * Log a message
	 * 
	 * @param str
	 *            Message to log
	 */
	public void log(String str) {
		textArea.insert(str + "\n", 0);
	}

	/**
	 * Reset log panel
	 */
	public void reset() {
		// empty textarea
		textArea.setText("");
	}
}
