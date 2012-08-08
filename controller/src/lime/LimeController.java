package lime;

import javax.swing.JDialog;

import net.CloseListener;
import net.NetIO;
import gui.ConnectDialog;
import gui.MainFrame;
import gui.control.Connect;

public class LimeController implements CloseListener, Logger, NetHandler {
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
		if (net.connect(addr, MASTER_PORT)) {
			frame.getControlPanel().getConnectionPanel().setMasterConnected(true);
			frame.log("Connected to: '" + addr + "'");
			return null;
		} else {
			frame.getControlPanel().getConnectionPanel().setMasterConnected(false);
			return "Failed to connect";
		}
	}
	
	public void showConnect() {
		ConnectDialog dialog = new ConnectDialog(frame, Connect.MASTER);
        dialog.setDefaultCloseOperation(JDialog.DISPOSE_ON_CLOSE);
        dialog.setVisible(true);
	}

	public static void main(String args[]) {
		new LimeController();
	}

	@Override
	public void socketClose() {
		
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
		// TODO Auto-generated method stub
		
	}
}
