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

import gui.MainFrame;

import java.awt.BorderLayout;
import java.awt.Font;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;

import javax.swing.BorderFactory;
import javax.swing.BoxLayout;
import javax.swing.JButton;
import javax.swing.JLabel;
import javax.swing.JPanel;
import javax.swing.JScrollPane;

import lime.CommandBuilder;

/**
 * Panel containing the all of the TitleElement
 * 
 * @author julus
 * 
 */
public class TitlePanel extends JPanel implements ActionListener {
	private static final long serialVersionUID = 8021157860217699741L;

	// frame owner
	private MainFrame owner;

	// scroll pane
	private JScrollPane pane;
	// panel inside scroll pane
	private JPanel panel;
	
	private JPanel top;
	private JButton playButton;
	private JLabel title;

	/**
	 * Create the TitlePanel
	 * 
	 * @param owner
	 *            Frame owner
	 */
	public TitlePanel(MainFrame owner) {
		super(new BorderLayout());
		setBorder(BorderFactory.createTitledBorder("Burn Control"));

		this.owner = owner;
		
		top = new JPanel();
		top.setLayout(new BoxLayout(top, BoxLayout.PAGE_AXIS));
		top.setBorder(BorderFactory.createTitledBorder("Current Burn"));
		title = new JLabel("Nothing Loaded");
		title.setFont(new Font("Dialog", 1, 20));
		// load/play button
		playButton = new JButton("Play");
		playButton.addActionListener(this);
		top.add(title, BorderLayout.NORTH);
		top.add(playButton, BorderLayout.SOUTH);	
		add(top, BorderLayout.NORTH);

		// create and add the scroll pane
		panel = new JPanel();
		panel.setLayout(new BoxLayout(panel, BoxLayout.PAGE_AXIS));
		pane = new JScrollPane(panel, JScrollPane.VERTICAL_SCROLLBAR_ALWAYS, JScrollPane.HORIZONTAL_SCROLLBAR_NEVER);
		pane.setBorder(BorderFactory.createEmptyBorder());
		add(pane, BorderLayout.CENTER);
	}
	
	public void setCurrent(String t) {
		title.setText(t);
	}

	/**
	 * Create a new element
	 * 
	 * @return New element
	 */
	public TitleElement create() {
		// create it
		TitleElement elm = new TitleElement(this);
		// add to array
		addElm(elm);
		// return it
		return elm;
	}

	/**
	 * Add the element to array
	 * 
	 * @param elm
	 *            Element to add
	 */
	public void addElm(TitleElement elm) {
		// add element to panel
		panel.add(elm);
	}

	/**
	 * Get the frame
	 * 
	 * @return Frame
	 */
	public MainFrame getOwner() {
		return owner;
	}

	/**
	 * Reset the panel
	 */
	public void reset() {
		// remove all the elements
		panel.removeAll();
	}

	@Override
	public void actionPerformed(ActionEvent e) {
		String str = CommandBuilder.createPlay();
		getOwner().getControl().sendMessage(str);
	}
}
