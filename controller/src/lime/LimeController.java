package lime;

import javax.swing.SwingUtilities;

import net.CloseListener;
import net.NetIO;
import gui.MainFrame;

public class LimeController implements CloseListener {
	private static final int MASTER_PORT = 6789;
	MainFrame frame;
	NetIO net;

	public LimeController() {
		final LimeController me = this;
		net = new NetIO();
		SwingUtilities.invokeLater(new Runnable() {
			public void run() {
				frame = new MainFrame(me);
				connect("127.0.0.1");
			}
		});
	}

	public boolean connect(String addr) {
		frame.showLoading("Connecting to: '" + addr + "'");
		if (net.connect(addr, MASTER_PORT)) {
			frame.resetControl();
			frame.showControl();
			frame.log("Connected to: '" + addr + "'");
			return true;
		} else {
			frame.showConnect("Failed to connect");
			return false;
		}
	}

	public static void main(String args[]) {
		new LimeController();
	}

	@Override
	public void socketClose() {
		frame.showConnect("Lost Connection");
	}
}
