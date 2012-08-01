package gui.control;

import javax.swing.BorderFactory;
import javax.swing.JLabel;
import javax.swing.JPanel;

public class RSSPanel extends JPanel {
	private static final long serialVersionUID = -1682827000691560614L;

	public RSSPanel() {
		setBorder(BorderFactory.createTitledBorder("RSS Ticker"));
		
		JLabel jlbHelloWorld = new JLabel("Future RSS control");
		add(jlbHelloWorld);
	}
	
	public void reset(){
		
	}
}
