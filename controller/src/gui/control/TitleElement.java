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
import java.awt.IllegalComponentStateException;
import java.awt.Insets;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.awt.event.MouseEvent;
import java.awt.event.MouseListener;

import javax.swing.DefaultListModel;
import javax.swing.JButton;
import javax.swing.JLabel;
import javax.swing.JList;
import javax.swing.JPanel;
import javax.swing.ListSelectionModel;

import lime.CommandBuilder;

import JSON.JSONArray;
import JSON.JSONException;
import JSON.JSONObject;
import JSON.JSONStringer;

/**
 * Element representing a title that can be played
 * 
 * @author julus
 * 
 */
public class TitleElement extends JPanel implements MouseListener, ActionListener {
	private static final long serialVersionUID = 3610843924974947348L;

	// name of entry
	private String nameValue = "";
	// name of script to use
	private String scriptName = "";

	// panel parent
	private TitlePanel parent;

	// panel items
	private JLabel nameLabel;
	private JLabel scriptLabel;
	private JList<TitleData> dataList;
	private JButton previewButton;
	private JButton playButton;

	// model for list
	private DefaultListModel<TitleData> listModel;

	// monitor mouse clicks on this
	private boolean clicked = false;
	private boolean clickedThisTime = false;

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
		addMouseListener(this);
		setMaximumSize(new Dimension(Short.MAX_VALUE, 65));// TODO replace this, but doesnt work otherwise :'(

		// name label
		nameLabel = new JLabel(nameValue);
		nameLabel.setFont(new Font("Dialog", 1, 20));
		GridBagConstraints c = new GridBagConstraints();
		c.fill = GridBagConstraints.HORIZONTAL;
		c.gridx = 0;
		c.gridy = 0;
		c.insets = new Insets(5, 5, 5, 5);
		add(nameLabel, c);

		// script label
		scriptLabel = new JLabel(scriptName);
		c = new GridBagConstraints();
		c.fill = GridBagConstraints.HORIZONTAL;
		c.gridx = 0;
		c.gridy = 1;
		c.insets = new Insets(0, 5, 0, 5);
		add(scriptLabel, c);

		// list of data
		listModel = new DefaultListModel<TitleData>();
		dataList = new JList<TitleData>(listModel);
		dataList.setSelectionMode(ListSelectionModel.SINGLE_INTERVAL_SELECTION);
		dataList.setLayoutOrientation(JList.VERTICAL);
		c = new GridBagConstraints();
		c.fill = GridBagConstraints.BOTH;
		c.gridx = 1;
		c.gridy = 0;
		c.gridheight = 2;
		c.weightx = 1.0;
		c.weighty = 1.0;
		c.insets = new Insets(5, 5, 5, 5);
		add(dataList, c);

		// preview button
		previewButton = new JButton("Preview");
		previewButton.setActionCommand("preview");
		previewButton.addActionListener(this);
		c = new GridBagConstraints();
		c.fill = GridBagConstraints.NONE;
		c.gridx = 2;
		c.gridheight = 2;
		c.gridy = 0;
		c.weighty = 1.0;
		c.insets = new Insets(5, 5, 5, 5);
		add(previewButton, c);

		// load/play button
		playButton = new JButton("Load");
		playButton.setActionCommand("load-play");
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
		// copt script
		scriptName = orig.getScriptName();

		// copy all the element data across
		DefaultListModel<TitleData> model = orig.getListModel();
		for (int i = 0; i < model.size(); i++) {
			TitleData d = model.get(i);
			addData(d.getName(), d.getValue());
		}
	}

	/**
	 * Create this from json
	 * 
	 * @param parent
	 *            Panel owner
	 * @param json
	 *            JSON to load from
	 * @throws JSONException
	 */
	public TitleElement(TitlePanel parent, JSONObject json) throws JSONException {
		this(parent);

		// copy name
		nameValue = json.getString("name");
		// script name
		scriptName = json.getString("script");

		// load data from json
		JSONArray data = json.getJSONArray("data");
		for (int i = 0; i < data.length(); i++) {
			try {
				// load and add data
				JSONObject o = data.getJSONObject(i);
				addData(o.getString("name"), o.getString("value"));
			} catch (JSONException e) {
				// catch error
				parent.getOwner().log("Failed to load a data entry for '" + nameValue + "'");
			}
		}
	}

	@Override
	public void paintComponent(Graphics g) {
		super.paintComponent(g);

		// set background it selected
		if (getMyParent().getSelectedElement() == this)
			setBackground(Color.GRAY);
		else
			setBackground(Color.decode("#EEEEEE"));

		// set name and script
		nameLabel.setText(nameValue);
		scriptLabel.setText(scriptName);

		// check if the script is valid, and enable/disable buttons
		if (!parent.getOwner().isScriptValid(scriptName) || !parent.getOwner().getControl().isMasterConnected()) {
			previewButton.setEnabled(false);
			playButton.setEnabled(false);
		} else {
			previewButton.setEnabled(true);
			playButton.setEnabled(true);
		}

		// set load/play button text
		if (parent.getLoaded() == this) {
			playButton.setText("Play");
		} else {
			playButton.setText("Load");
		}
	}

	/**
	 * Add another data pair to element
	 * 
	 * @param name
	 *            Name of pair
	 * @param value
	 *            Value of pair
	 */
	public void addData(String name, String value) {
		TitleData d = new TitleData(name, value);
		listModel.addElement(d);
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
	 * Get the script name
	 * 
	 * @return String script name
	 */
	public String getScriptName() {
		return scriptName;
	}

	/**
	 * Set the script name
	 * 
	 * @param name
	 *            New script name
	 */
	public void setScriptName(String name) {
		scriptName = name;
	}

	/**
	 * Get the list model
	 * 
	 * @return List model
	 */
	public DefaultListModel<TitleData> getListModel() {
		return listModel;
	}

	/**
	 * Get my parent panel
	 * 
	 * @return Parent panel
	 */
	public TitlePanel getMyParent() {
		return parent;
	}

	/**
	 * Handle mouse clicks
	 */
	@Override
	public void mouseClicked(MouseEvent e) {
		// remove me as selected element if i already am, and something else :S
		if (getMyParent().getSelectedElement() == this && !clickedThisTime) {
			getMyParent().setSelectedElement(null);
			getMyParent().repaint();
		} else {
			// otherwise, make me selected
			getMyParent().setSelectedElement(this);
			getMyParent().repaint();
		}

		// set as not clicked
		clickedThisTime = false;
		clicked = false;
	}

	/**
	 * Handle mouse press
	 */
	@Override
	public void mousePressed(MouseEvent e) {
		clicked = true;
		// if im not already selected, mark as clicked just now
		if (getMyParent().getSelectedElement() != this) {
			clickedThisTime = true;
		}

		// set me as selected, and repaint
		getMyParent().setSelectedElement(this);
		getMyParent().repaint();
	}

	/**
	 * Handle mouse releader
	 */
	@Override
	public void mouseReleased(MouseEvent e) {
		// set not clicked
		clicked = false;
	}

	/**
	 * Handle mouse enter
	 */
	@Override
	public void mouseEntered(MouseEvent e) {
	}

	/**
	 * Handle mouse exit
	 */
	@Override
	public void mouseExited(MouseEvent e) {
		try {
			// if being clicked
			if (clicked) {
				// check if moved through top edge
				if (e.getYOnScreen() < getLocationOnScreen().y) {
					// move up
					parent.moveUpElm(this);
					// redraw everything
					parent.redrawElms();
					parent.revalidate();
					parent.repaint();

					// check if moved through bottom edge
				} else if (e.getYOnScreen() >= getLocationOnScreen().y + getHeight()) {
					// move down
					parent.moveDownElm(this);
					// redraw everything
					parent.redrawElms();
					parent.revalidate();
					parent.repaint();
				}
			}
		} catch (IllegalComponentStateException icse) {
			// catch error
		}
	}

	/**
	 * Convert me to json
	 * 
	 * @param stringer
	 *            Json stringer
	 * @throws JSONException
	 */
	public void toJSON(JSONStringer stringer) throws JSONException {
		// create an object
		stringer.object();
		// add name
		stringer.key("name").value(nameValue);
		// add script
		stringer.key("script").value(scriptName);

		// create data array
		stringer.key("data").array();

		for (int i = 0; i < listModel.getSize(); i++) {
			try {
				// add data values
				TitleData d = listModel.get(i);
				stringer.object();
				stringer.key("name").value(d.getName());
				stringer.key("value").value(d.getValue());
				stringer.endObject();
			} catch (JSONException e) {
				// catch error
				parent.getOwner().log("Failed to save data entry for '" + nameValue + "'");
			}
		}

		// close up
		stringer.endArray();
		stringer.endObject();
	}

	@Override
	public void actionPerformed(ActionEvent e) {
		// preview button
		if (e.getActionCommand().equals("preview")) {
			// unload previously loaded
			String str = CommandBuilder.createPreview(this);
			parent.getOwner().getControl().sendMessage(str);

			// load/play button
		} else if (e.getActionCommand().equals("load-play")) {
			// if loaded
			if (parent.getLoaded() == this) {
				// play
				String str = CommandBuilder.createPlay(this);
				parent.getOwner().getControl().sendMessage(str);

			} else {
				// unload previously loaded
				String str = CommandBuilder.createPreload(this);
				parent.getOwner().getControl().sendMessage(str);
			}
		}
	}
}
