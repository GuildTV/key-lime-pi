package lime;

import javax.swing.JDialog;

import net.NetIO;
import gui.ConnectDialog;
import gui.MainFrame;
import gui.control.Connect;

public class LimeController implements Logger, NetHandler {
	private static final int MASTER_PORT = 6789;
	MainFrame frame;
	NetIO net;
	MessageHandler handler;
	
	private boolean liveMode = false;

	public LimeController() {
		net = new NetIO(this);
		net.setLogger(this);
		frame = new MainFrame(this);
		CommandBuilder.setLog(this);
		handler = new MessageHandler(this, net.getMessageQueue());
	}

	public String connect(String addr) {
		frame.log("Connecting to: '" + addr + "'");
		if (net.connect(addr, MASTER_PORT)) {
			frame.log("Handshaking...");
			return null;
		} else {
			frame.getControlPanel().getConnectionPanel().setMasterConnected(false);
			frame.log("Failed to connect");
			return "Failed to connect";
		}
	}
	
	public void handshaken() {
		frame.getMyMenu().setVisConnect(false);
		frame.getMyMenu().setVisDisconnect(true);
		frame.getControlPanel().getConnectionPanel().setMasterConnected(true);
		frame.log("Connected");
	}
	
	public void showConnect() {
		if (isMasterConnected())
			return;
		
		ConnectDialog dialog = new ConnectDialog(frame, Connect.MASTER);
        dialog.setDefaultCloseOperation(JDialog.DISPOSE_ON_CLOSE);
        dialog.setVisible(true);
	}

	public static void main(String args[]) {
		new LimeController();
	}
	
	public boolean isLive(){
		return liveMode;
	}
	
	public void setLive(boolean live){
		liveMode = live;
	}
	
	public boolean isMasterConnected() {
		return net.isConnected();
	}

	@Override
	public void log(String str) {
		frame.log(str);
	}
	
	public boolean sendMessage(String msg) {
		return net.sendMessage(msg);
	}
	
	public MainFrame getFrame() {
		return frame;
	}

	@Override
	public void closed() {
		frame.getControlPanel().getConnectionPanel().closed();
		frame.getMyMenu().setVisDisconnect(false);
		frame.getMyMenu().setVisConnect(true);
	}
	
	public void disconnect(){
		net.close();
	}
}
