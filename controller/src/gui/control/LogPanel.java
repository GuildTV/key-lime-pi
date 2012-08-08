package gui.control;

import java.awt.BorderLayout;

import javax.swing.BorderFactory;
import javax.swing.JPanel;
import javax.swing.JScrollPane;
import javax.swing.JTextArea;

/**
 * Log panel
 * 
 * @author julus
 * 
 */
public class LogPanel extends JPanel {
	private static final long serialVersionUID = -5018864002113251061L;

	// text area
	private JTextArea textArea;
	// scroll pane
	private JScrollPane scrollPane;

	/**
	 * Create log panel
	 */
	public LogPanel() {
		super(new BorderLayout());
		setBorder(BorderFactory.createTitledBorder("Message Log"));

		// create text area and add
		textArea = new JTextArea(4, 20);
		scrollPane = new JScrollPane(textArea, JScrollPane.VERTICAL_SCROLLBAR_ALWAYS,
				JScrollPane.HORIZONTAL_SCROLLBAR_AS_NEEDED);
		textArea.setEditable(false);
		add(scrollPane, BorderLayout.CENTER);
	}

	/**
	 * Log a message
	 * 
	 * @param str
	 *            Message to log
	 */
	public void log(String str) {
		textArea.insert(str + "\n", 0);
	}

	/**
	 * Reset log panel
	 */
	public void reset() {
		// empty textarea
		textArea.setText("");
	}
}
