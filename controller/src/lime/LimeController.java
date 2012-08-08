package lime;

import javax.swing.JDialog;

import net.NetIO;
import gui.ConnectDialog;
import gui.MainFrame;
import gui.control.Connect;

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

	// is controller in live mode
	private boolean liveMode = false;

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
	}

	/**
	 * Show the connection box to connect to master-pi
	 */
	public void showConnect() {
		// return if already connected
		if (isMasterConnected())
			return;

		// create window and show it
		ConnectDialog dialog = new ConnectDialog(frame, Connect.MASTER);
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
	 * Check if in live mode
	 * 
	 * @return Is it in live mode
	 */
	public boolean isLive() {
		return liveMode;
	}

	/**
	 * Set live mode
	 * 
	 * @param live
	 *            Boolean state
	 */
	public void setLive(boolean live) {
		liveMode = live;
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
