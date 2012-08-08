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
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;

import javax.swing.JButton;
import javax.swing.JOptionPane;
import javax.swing.JPanel;

/**
 * Button panel in titles list
 * 
 * @author julus
 * 
 */
public class TitleButtonPanel extends JPanel implements ActionListener {
	private static final long serialVersionUID = -2204407876186244418L;

	// panel owner
	private TitlePanel owner;

	// buttons
	private JButton addButton;
	private JButton deleteButton;
	private JButton cloneButton;
	private JButton editButton;
	private JButton moveUpButton;
	private JButton moveDownButton;

	/**
	 * Create the button panel
	 * 
	 * @param owner
	 *            Panel owner
	 */
	public TitleButtonPanel(TitlePanel owner) {
		this.owner = owner;

		// give a bottom border
		setBorder(new BottomBorder(Color.GRAY));

		// add button
		addButton = new JButton("Add");
		addButton.addActionListener(this);
		addButton.setActionCommand("Add");
		add(addButton);

		// edit button
		editButton = new JButton("Edit");
		editButton.addActionListener(this);
		editButton.setActionCommand("Edit");
		add(editButton);

		// clone button
		cloneButton = new JButton("Clone");
		cloneButton.addActionListener(this);
		cloneButton.setActionCommand("Clone");
		add(cloneButton);

		// delete button
		deleteButton = new JButton("Delete");
		deleteButton.addActionListener(this);
		deleteButton.setActionCommand("Delete");
		add(deleteButton);

		// move up button
		moveUpButton = new JButton("Move Up");
		moveUpButton.addActionListener(this);
		moveUpButton.setActionCommand("Move Up");
		add(moveUpButton);

		// move down button
		moveDownButton = new JButton("Move Down");
		moveDownButton.addActionListener(this);
		moveDownButton.setActionCommand("Move Down");
		add(moveDownButton);

		// set the buttons for nothing selected
		setNull();
	}

	/**
	 * Handle button clicks
	 */
	@Override
	public void actionPerformed(ActionEvent e) {
		// add button
		if (e.getActionCommand().equals("Add")) {
			// create a new element
			TitleElement elm = owner.create();
			// repaint
			owner.revalidate();
			owner.repaint();
			// open edit dialog
			TitleElementEditDialog d = new TitleElementEditDialog(owner.getOwner(), elm);
			d.setVisible(true);

			// delete button
		} else if (e.getActionCommand().equals("Delete")) {
			// stop if nothing selected
			if (owner.getSelectedElement() == null)
				return;
			// show confirmation dialog
			Object[] options = { "Yes", "No" };
			int n = JOptionPane.showOptionDialog(owner, "Are you sure you want to delete this?", "Confirm Deletion",
					JOptionPane.YES_NO_OPTION, JOptionPane.WARNING_MESSAGE, null, options, options[1]);
			if (n == JOptionPane.YES_OPTION) {
				// delete on yes button
				owner.removeElm(owner.getSelectedElement());
			}

			// clone button
		} else if (e.getActionCommand().equals("Clone")) {
			// stop if nothing selected
			if (owner.getSelectedElement() == null)
				return;
			// clone element
			TitleElement elm = new TitleElement(owner, owner.getSelectedElement());
			// show new element
			owner.addElm(elm);

			// edit button
		} else if (e.getActionCommand().equals("Edit")) {
			// stop if nothing selected
			if (owner.getSelectedElement() == null)
				return;
			// open edit dialog
			TitleElementEditDialog d = new TitleElementEditDialog(owner.getOwner(), owner.getSelectedElement());
			d.setVisible(true);

			// move up button
		} else if (e.getActionCommand().equals("Move Up")) {
			// stop if nothing selected
			if (owner.getSelectedElement() == null)
				return;

			// move up
			owner.moveUpElm(owner.getSelectedElement());
			// force redraw
			owner.redrawElms();

			// move down button
		} else if (e.getActionCommand().equals("Move Down")) {
			// stop if nothing selected
			if (owner.getSelectedElement() == null)
				return;

			// move down
			owner.moveDownElm(owner.getSelectedElement());
			// force redraw
			owner.redrawElms();
		}

		// force repaint
		owner.revalidate();
		owner.repaint();
	}

	/**
	 * Set buttons for null selected state
	 */
	public void setNull() {
		deleteButton.setEnabled(false);
		cloneButton.setEnabled(false);
		editButton.setEnabled(false);
		moveUpButton.setEnabled(false);
		moveDownButton.setEnabled(false);
	}

	/**
	 * Set buttons for selected state
	 */
	public void setElm() {
		deleteButton.setEnabled(true);
		cloneButton.setEnabled(true);
		editButton.setEnabled(true);
		moveUpButton.setEnabled(true);
		moveDownButton.setEnabled(true);
	}
}
