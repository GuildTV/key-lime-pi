package gui;

import java.awt.GridBagConstraints;
import java.awt.GridBagLayout;

import javax.swing.JLabel;
import javax.swing.JPanel;
import javax.swing.JProgressBar;

/**
 * Generic panel to show something is loading/working
 * 
 * @author julus
 * 
 */
public class LoadingPanel extends JPanel {
	private static final long serialVersionUID = 3810958755385078315L;

	// progress bar
	private JProgressBar progress;
	// text label
	private JLabel label;

	LoadingPanel() {
		// define layout manager
		super(new GridBagLayout());
		// load constants
		GridBagConstraints c = new GridBagConstraints();

		// create progress bar
		progress = new JProgressBar();
		progress.setIndeterminate(true);
		c.fill = GridBagConstraints.VERTICAL;
		c.gridx = 0;
		c.gridy = 0;
		add(progress, c);

		// create label
		label = new JLabel();
		c.fill = GridBagConstraints.VERTICAL;
		c.gridx = 0;
		c.gridy = 1;
		add(label, c);
	}

	/**
	 * Set the text of the JLabel
	 * 
	 * @param str
	 *            String to display
	 */
	public void setLabel(String str) {
		label.setText(str);
	}
}
