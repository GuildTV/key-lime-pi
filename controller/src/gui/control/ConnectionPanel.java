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

import java.awt.GridBagConstraints;
import java.awt.GridBagLayout;
import java.awt.Insets;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;

import javax.swing.BorderFactory;
import javax.swing.JButton;
import javax.swing.JLabel;
import javax.swing.JPanel;
import javax.swing.JTextField;

/**
 * Panel to manage connection status
 * 
 * @author julus
 * 
 */
public class ConnectionPanel extends JPanel implements ActionListener {
	private static final long serialVersionUID = -1682827000691560614L;

	// frame owner
	private MainFrame owner;

	// text field status messages
	private JTextField masterStatus;
	private JTextField slaveStatus;

	// connect/disconnect buttons
	private JButton masterButton;
	private JButton slaveButton;

	// current action
	private JTextField currentAction;

	// live button
	private JButton liveButton;

	/**
	 * Create this panel
	 * 
	 * @param owner
	 *            Panel owner
	 */
	public ConnectionPanel(MainFrame owner) {
		super(new GridBagLayout());
		setBorder(BorderFactory.createTitledBorder("Pi/Connection Status"));

		this.owner = owner;

		// master label
		JLabel masterLabel = new JLabel("master: ");
		GridBagConstraints c = new GridBagConstraints();
		c.fill = GridBagConstraints.BOTH;
		c.gridx = 0;
		c.gridy = 0;
		c.insets = new Insets(0, 0, 2, 0);
		add(masterLabel, c);

		// master status
		masterStatus = new JTextField(10);
		masterStatus.setText("disconnected");
		masterStatus.setEditable(false);
		c = new GridBagConstraints();
		c.fill = GridBagConstraints.BOTH;
		c.gridx = 1;
		c.gridy = 0;
		c.insets = new Insets(0, 0, 2, 0);
		add(masterStatus, c);

		// master button
		masterButton = new JButton("Connect");
		masterButton.setActionCommand("master-button");
		masterButton.addActionListener(this);
		c = new GridBagConstraints();
		c.fill = GridBagConstraints.BOTH;
		c.gridx = 2;
		c.gridy = 0;
		c.insets = new Insets(0, 4, 2, 0);
		add(masterButton, c);

		// slave label
		JLabel slaveLabel = new JLabel("slave: ");
		c = new GridBagConstraints();
		c.fill = GridBagConstraints.BOTH;
		c.gridx = 0;
		c.gridy = 1;
		c.insets = new Insets(2, 0, 0, 0);
		add(slaveLabel, c);

		// slave status
		slaveStatus = new JTextField(10);
		slaveStatus.setText("disconnected");
		slaveStatus.setEditable(false);
		c = new GridBagConstraints();
		c.fill = GridBagConstraints.BOTH;
		c.gridx = 1;
		c.gridy = 1;
		c.insets = new Insets(2, 0, 0, 0);
		add(slaveStatus, c);

		// slave button
		slaveButton = new JButton("Connect");
		slaveButton.setActionCommand("slave-button");
		slaveButton.addActionListener(this);
		slaveButton.setEnabled(false);
		c = new GridBagConstraints();
		c.fill = GridBagConstraints.BOTH;
		c.gridx = 2;
		c.gridy = 1;
		c.insets = new Insets(2, 4, 0, 0);
		add(slaveButton, c);

		// action text
		currentAction = new JTextField(20);
		currentAction.setText("disconnected");
		currentAction.setEditable(false);
		c = new GridBagConstraints();
		c.fill = GridBagConstraints.BOTH;
		c.gridx = 3;
		c.gridy = 0;
		c.gridheight = 2;
		c.weightx = 1.0;
		c.weighty = 1.0;
		c.insets = new Insets(1, 10, 1, 10);
		add(currentAction, c);

		// live button
		liveButton = new JButton("Go Live");
		liveButton.setEnabled(false);
		liveButton.setActionCommand("live-button");
		liveButton.addActionListener(this);
		c = new GridBagConstraints();
		c.fill = GridBagConstraints.BOTH;
		c.gridx = 4;
		c.gridy = 0;
		c.gridheight = 2;
		c.insets = new Insets(1, 1, 1, 1);
		add(liveButton, c);
	}

	/**
	 * Reset this panel
	 */
	public void reset() {
	}

	@Override
	public void actionPerformed(ActionEvent e) {
		// master-button clock
		if (e.getActionCommand().equals("master-button")) {
			// connect or disconnect
			if (owner.getControl().isMasterConnected()) {
				owner.getControl().disconnect();
			} else
				owner.getControl().showConnect();

			// slave-button click
		} else if (e.getActionCommand().equals("slave-button")) {

			// live-button click
		} else if (e.getActionCommand().equals("live-button")) {
		}
	}

	/**
	 * Set master as connected
	 * 
	 * @param state
	 *            Boolean connected
	 */
	public void setMasterConnected(boolean state) {
		// set master connected
		if (state) {
			masterStatus.setText("connected");
			masterButton.setText("Disconnect");
			currentAction.setText("waiting");
			pollSlave();
			slaveButton.setEnabled(true);

			// set master disconnected
		} else {
			masterStatus.setText("disconnected");
			masterButton.setText("Connect");
			currentAction.setText("disconnected");
			slaveButton.setEnabled(false);
			setSlaveConnected(false);
		}
	}

	/**
	 * Set slave as connected
	 * 
	 * @param state
	 *            Boolean connected
	 */
	public void setSlaveConnected(boolean state) {
		// set slave connected
		if (state) {
			slaveStatus.setText("connected");
			slaveButton.setText("Disconnect");

			// set slave disconnected
		} else {
			slaveStatus.setText("disconnected");
			slaveButton.setText("Connect");
		}
	}

	/**
	 * Set action text
	 * 
	 * @param text
	 *            String text
	 */
	public void setAction(String text) {
		currentAction.setText(text);
	}

	/**
	 * Handle server connection closed
	 */
	public void closed() {
		setMasterConnected(false);
	}

	/**
	 * Poll for slave status
	 */
	public void pollSlave() {
		owner.getControl().sendMessage("{\"type\":\"slaveConnected\"}");
	}

	/**
	 * Slave status response
	 * 
	 * @param str
	 *            String response
	 */
	public void pollSlaveResponse(String str) {
		boolean response = str.equals("true");
		// set action text
		if (!response)
			setAction("waiting for connection to slave-pi");
		// set slave connected status
		setSlaveConnected(response);
	}
}