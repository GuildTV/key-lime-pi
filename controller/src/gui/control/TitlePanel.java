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

import javax.swing.BorderFactory;
import javax.swing.BoxLayout;
import javax.swing.JPanel;
import javax.swing.JScrollPane;

/**
 * Panel containing the all of the TitleElement
 * 
 * @author julus
 * 
 */
public class TitlePanel extends JPanel {
	private static final long serialVersionUID = 8021157860217699741L;

	// amount to increase storage array by
	private static final int ARRAY_INCREMENT = 10;

	// frame owner
	private MainFrame owner;

	// lock for array
	private Object arrayLock = new Object();

	// scroll pane
	private JScrollPane pane;
	// panel inside scroll pane
	private JPanel panel;

	// array of elements
	private TitleElement elements[];
	// element count
	private int elmCount = 0;

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

		// create and add the scroll pane
		panel = new JPanel();
		panel.setLayout(new BoxLayout(panel, BoxLayout.PAGE_AXIS));
		pane = new JScrollPane(panel, JScrollPane.VERTICAL_SCROLLBAR_ALWAYS, JScrollPane.HORIZONTAL_SCROLLBAR_NEVER);
		pane.setBorder(BorderFactory.createEmptyBorder());
		add(pane, BorderLayout.CENTER);

		// create new element storages
		elements = new TitleElement[ARRAY_INCREMENT];
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
		// get lock
		synchronized (arrayLock) {
			try {
				// try adding and increase count
				elements[elmCount] = elm;
				elmCount++;
			} catch (ArrayIndexOutOfBoundsException aoe) {
				// create a bigger array
				TitleElement elms[] = new TitleElement[elements.length + ARRAY_INCREMENT];
				// copy data into it
				System.arraycopy(elements, 0, elms, 0, elmCount);
				// add new item
				elms[elmCount] = elm;
				// increate count
				elmCount++;
				// overwrite old array
				elements = elms;
			}
		}

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
		// create new array
		elements = new TitleElement[ARRAY_INCREMENT];
		// remove all the elements
		panel.removeAll();
	}
}
