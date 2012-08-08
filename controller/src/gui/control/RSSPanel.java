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

import javax.swing.BorderFactory;
import javax.swing.JLabel;
import javax.swing.JPanel;

/**
 * Panel to control a RSS feed
 * 
 * @author julus
 * 
 */
public class RSSPanel extends JPanel {
	private static final long serialVersionUID = -1682827000691560614L;

	/**
	 * Create the panel
	 */
	public RSSPanel() {
		setBorder(BorderFactory.createTitledBorder("RSS Ticker"));

		// temp text
		JLabel jlbHelloWorld = new JLabel("Future RSS control");
		add(jlbHelloWorld);
	}

	/**
	 * Reset panel
	 */
	public void reset() {
	}
}
