package gui.control;

import javax.swing.BorderFactory;
import javax.swing.JLabel;
import javax.swing.JPanel;

/**
 * Panel to control a RSS feed
 * 
 * @author julus
 * 
 */
public class RSSPanel extends JPanel {
	private static final long serialVersionUID = -1682827000691560614L;

	/**
	 * Create the panel
	 */
	public RSSPanel() {
		setBorder(BorderFactory.createTitledBorder("RSS Ticker"));

		// temp text
		JLabel jlbHelloWorld = new JLabel("Future RSS control");
		add(jlbHelloWorld);
	}

	/**
	 * Reset panel
	 */
	public void reset() {
	}
}
