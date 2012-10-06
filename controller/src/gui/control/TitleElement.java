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

import gui.BottomBorder;

import java.awt.Color;
import java.awt.Dimension;
import java.awt.Font;
import java.awt.Graphics;
import java.awt.GridBagConstraints;
import java.awt.GridBagLayout;
import java.awt.Insets;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;

import javax.swing.JButton;
import javax.swing.JLabel;
import javax.swing.JPanel;

import lime.CommandBuilder;

/**
 * Element representing a title that can be played
 * 
 * @author julus
 * 
 */
public class TitleElement extends JPanel implements ActionListener {
	private static final long serialVersionUID = 3610843924974947348L;

	// name of entry
	private String nameValue = "";

	// panel parent
	private TitlePanel parent;

	// panel items
	private JLabel nameLabel;
	private JButton playButton;

	/**
	 * Create a new element
	 * 
	 * @param parent
	 *            Panel owner
	 */
	public TitleElement(TitlePanel parent) {
		super(new GridBagLayout());
		this.parent = parent;

		// setup this panel
		setBorder(new BottomBorder(Color.GRAY));
		setMaximumSize(new Dimension(Short.MAX_VALUE, 65));// TODO replace this, but doesnt work otherwise :'(

		// name label
		nameLabel = new JLabel(nameValue);
		nameLabel.setFont(new Font("Dialog", 1, 20));
		GridBagConstraints c = new GridBagConstraints();
		c.fill = GridBagConstraints.HORIZONTAL;
		c.gridx = 0;
		c.gridy = 0;
		c.weightx = 1.0;
		c.insets = new Insets(5, 5, 5, 5);
		add(nameLabel, c);

		// load/play button
		playButton = new JButton("Select");
		playButton.setActionCommand("activate");
		playButton.addActionListener(this);
		c = new GridBagConstraints();
		c.fill = GridBagConstraints.NONE;
		c.gridx = 3;
		c.gridheight = 2;
		c.gridy = 0;
		c.weighty = 1.0;
		c.insets = new Insets(5, 5, 5, 5);
		add(playButton, c);
	}

	/**
	 * Create this from an existing element
	 * 
	 * @param parent
	 *            Panel owner
	 * @param orig
	 *            Element being copied
	 */
	public TitleElement(TitlePanel parent, TitleElement orig) {
		this(parent);

		// copy name
		nameValue = orig.getNameValue();
	}

	@Override
	public void paintComponent(Graphics g) {
		super.paintComponent(g);

		// set name and script
		nameLabel.setText(nameValue);
	}

	/**
	 * Get the name
	 * 
	 * @return String name
	 */
	public String getNameValue() {
		return nameValue;
	}

	/**
	 * Set the name
	 * 
	 * @param name
	 *            New name
	 */
	public void setNameValue(String name) {
		nameValue = name;
	}

	/**
	 * Get my parent panel
	 * 
	 * @return Parent panel
	 */
	public TitlePanel getMyParent() {
		return parent;
	}

	@Override
	public void actionPerformed(ActionEvent e) {
		if (e.getActionCommand().equals("activate")) {
			// play
			String str = CommandBuilder.createSelect(this);
			parent.setCurrent(nameValue);
			parent.getOwner().getControl().sendMessage(str);
		}
	}
}
