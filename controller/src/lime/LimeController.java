package lime;

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

import javax.swing.JDialog;

import net.NetIO;
import gui.ConnectDialog;
import gui.MainFrame;

/**
 * Controller for key-lime-pi
 * 
 * @author julus
 * 
 */
public class LimeController implements Logger, NetHandler {
	// port to connect to
	private static final int MASTER_PORT = 6789;
	// window frame
	MainFrame frame;
	// network connection
	NetIO net;
	// message handler
	MessageHandler handler;

	/**
	 * Create this controller
	 */
	public LimeController() {
		// setup network
		net = new NetIO(this);
		// set logger for network
		net.setLogger(this);
		// create window frame
		frame = new MainFrame(this);
		// set logger for commandbuilder
		CommandBuilder.setLog(this);
		// setup message handler
		handler = new MessageHandler(this, net.getMessageQueue());
	}

	/**
	 * Connect to address
	 * 
	 * @param addr
	 *            Address to connect to
	 * @return String error message
	 */
	public String connect(String addr) {
		frame.log("Connecting to: '" + addr + "'");
		// try connecting
		if (net.connect(addr, MASTER_PORT)) {
			// log handshaking
			frame.log("Handshaking...");
			return null;
		} else {
			// set not connected in connection panel
			frame.getControlPanel().getConnectionPanel().setMasterConnected(false);
			frame.log("Failed to connect");
			// return error message
			return "Failed to connect";
		}
	}

	/**
	 * Handshake is completed
	 */
	@Override
	public void handshaken() {
		// toggle menubar items
		frame.getMyMenu().setVisConnect(false);
		frame.getMyMenu().setVisDisconnect(true);
		// set master as conencted in connection panel
		frame.getControlPanel().getConnectionPanel().setMasterConnected(true);
		// log
		frame.log("Connected");

		// force title panel repaint
		frame.getControlPanel().getTitlePanel().repaint();
	}

	/**
	 * Show the connection box to connect to master-pi
	 */
	public void showConnect() {
		// return if already connected
		if (isMasterConnected())
			return;

		// create window and show it
		ConnectDialog dialog = new ConnectDialog(frame);
		dialog.setDefaultCloseOperation(JDialog.DISPOSE_ON_CLOSE);
		dialog.setVisible(true);
	}

	/**
	 * Entry point for program
	 * 
	 * @param args
	 *            Command line arguments
	 */
	public static void main(String args[]) {
		new LimeController();
	}

	/**
	 * Check if we are connected to master-pi
	 * 
	 * @return Connection status
	 */
	public boolean isMasterConnected() {
		return net.isConnected();
	}

	/**
	 * Write to log frame
	 */
	@Override
	public void log(String str) {
		frame.log(str);
	}

	/**
	 * Send a message to master-pi
	 * 
	 * @param msg
	 *            String message to send
	 * @return boolean success
	 */
	public boolean sendMessage(String msg) {
		return net.sendMessage(msg);
	}

	/**
	 * Get the frame
	 * 
	 * @return Frame being used
	 */
	public MainFrame getFrame() {
		return frame;
	}

	/**
	 * Connection has been closed
	 */
	@Override
	public void closed() {
		// inform connection panel that the connection is closed
		frame.getControlPanel().getConnectionPanel().closed();
		// toggle menu buttons
		frame.getMyMenu().setVisDisconnect(false);
		frame.getMyMenu().setVisConnect(true);
	}

	/**
	 * Disconnect from master-pi
	 */
	public void disconnect() {
		net.close();
	}
}
